#include "threadmanager.h"
#include "acceldevice.h" // For AccelDevice class
#include "csvwriter.h"    // Include CsvWriter header
#include "comm.h"        // For communication definitions
#include <QDebug>

ThreadManager::ThreadManager(QObject *parent)
    : QObject(parent),
      _mpAccelDevice(nullptr),
      _mpAccelDeviceThread(nullptr),
      _mCsvWriter(nullptr),        // Initialize CsvWriter pointer
      _mCsvWriterThread(nullptr)  // Initialize CsvWriter thread pointer
{
    qDebug() << "ThreadManager created in thread:" << QThread::currentThreadId();

    // --- CsvWriter Setup ---
    _mCsvWriter = new CsvWriter(); // Create CsvWriter instance (uses default filename)
    _mCsvWriterThread = new QThread(this); // Create thread, parented to manager
    _mCsvWriter->moveToThread(_mCsvWriterThread);

    // Connect thread finished signal to trigger deletion of writer and thread
    connect(_mCsvWriterThread, &QThread::finished, _mCsvWriter, &QObject::deleteLater);
    connect(_mCsvWriterThread, &QThread::finished, _mCsvWriterThread, &QObject::deleteLater);
    // Connect finished signal to manager's cleanup slot (optional)
    connect(_mCsvWriterThread, &QThread::finished, this, &ThreadManager::onCsvWriterThreadFinished);

    _mCsvWriterThread->start();
    qDebug() << "CsvWriter thread started.";
    // --- End CsvWriter Setup ---
}

ThreadManager::~ThreadManager()
{
    qDebug() << "ThreadManager destructor started.";
    // Clean up AccelDevice thread first
    handleClosePort(); 

    // --- CsvWriter Cleanup ---
    if (_mCsvWriterThread) {
        qDebug() << "Requesting CsvWriter thread quit.";
        _mCsvWriterThread->quit();
        if (!_mCsvWriterThread->wait(3000)) { // Wait up to 3 seconds
            qWarning() << "CsvWriter thread did not finish gracefully, terminating.";
            _mCsvWriterThread->terminate();
            _mCsvWriterThread->wait(); // Wait after termination
        }
        // Deletion is handled by deleteLater connections
        qDebug() << "CsvWriter thread cleanup process initiated."; 
    }
     else if (_mCsvWriter) {
         // If thread wasn't created but writer was (should not happen here), delete writer directly
         delete _mCsvWriter;
         _mCsvWriter = nullptr;
    }
    // --- End CsvWriter Cleanup ---

    qDebug() << "ThreadManager destroyed";
}

void ThreadManager::handlePortChange(const QString& portName, QSerialPort::BaudRate baudRate,
                                   QSerialPort::DataBits dataBits, QSerialPort::Parity parity,
                                   QSerialPort::StopBits stopBits, QSerialPort::FlowControl flowControl)
{
    if (_mpAccelDevice) {
        qDebug() << "Closing existing AccelDevice before opening new port";
        // Disconnect existing signal connection before closing/deleting device
        if (_mCsvWriter) { // Check if CsvWriter exists
             disconnect(_mpAccelDevice, &AccelDevice::ImuData, _mCsvWriter, &CsvWriter::writeImuData);
        }
        handleClosePort();
    }

    // --- AccelDevice Setup ---
    _mpAccelDeviceThread = new QThread(this); 
    _mpAccelDevice = new AccelDevice(nullptr); 
    _mpAccelDevice->setPort(portName, baudRate, dataBits, parity, stopBits, flowControl);
    _mpAccelDevice->openPort(QIODeviceBase::ReadWrite);
    _mpAccelDevice->moveToThread(_mpAccelDeviceThread);
    connect(_mpAccelDevice, &AccelDevice::destroyed, this, &ThreadManager::onAccelDeviceDestroyed);
    connect(_mpAccelDevice, &AccelDevice::destroyed, _mpAccelDeviceThread, &QThread::quit);
    connect(_mpAccelDeviceThread, &QThread::finished, _mpAccelDevice, &QObject::deleteLater);
    connect(_mpAccelDeviceThread, &QThread::finished, _mpAccelDeviceThread, &QObject::deleteLater);
    // --- End AccelDevice Setup ---

    // *** Connect AccelDevice's ImuData signal to CsvWriter's slot ***
    if (_mCsvWriter) { // Ensure CsvWriter instance exists before connecting
        connect(_mpAccelDevice, &AccelDevice::ImuData, _mCsvWriter, &CsvWriter::writeImuData, Qt::QueuedConnection);
        qDebug() << "Connected AccelDevice::ImuData to CsvWriter::writeImuData";
    } else {
        qWarning() << "CsvWriter instance is null, cannot connect ImuData signal.";
    }

    _mpAccelDeviceThread->start();

    // Start the AccelDevice's internal timer via a queued connection
    QMetaObject::invokeMethod(
        _mpAccelDevice,
        [this]() { 
            if (_mpAccelDevice) {
                _mpAccelDevice->startTimer(1);
            }
        },
        Qt::QueuedConnection
    );

    qDebug() << "AccelDevice created and moved to thread:" << _mpAccelDeviceThread;
}

void ThreadManager::onAccelDeviceDestroyed()
{
    qDebug() << "AccelDevice was destroyed, cleaning up pointers in ThreadManager";
    // Disconnect signal if necessary (though AccelDevice is already destroyed)
     if (_mpAccelDevice && _mCsvWriter) {
        disconnect(_mpAccelDevice, &AccelDevice::ImuData, _mCsvWriter, &CsvWriter::writeImuData);
     }
    _mpAccelDevice = nullptr; 
}

void ThreadManager::handleClosePort()
{
    if (_mpAccelDeviceThread && _mpAccelDeviceThread->isRunning()) {
        qDebug() << "Stopping AccelDevice thread";
         // Disconnect signal before stopping thread and deleting device
         if (_mpAccelDevice && _mCsvWriter) {
             disconnect(_mpAccelDevice, &AccelDevice::ImuData, _mCsvWriter, &CsvWriter::writeImuData);
             qDebug() << "Disconnected AccelDevice::ImuData signal.";
         }
        if (_mpAccelDevice) {
             QMetaObject::invokeMethod(_mpAccelDevice, "stopTimer", Qt::QueuedConnection);
        }
        _mpAccelDeviceThread->quit();
        if (!_mpAccelDeviceThread->wait(3000)) { 
            qDebug() << "AccelDevice thread did not quit gracefully, terminating...";
            _mpAccelDeviceThread->terminate();
            _mpAccelDeviceThread->wait(); 
        }
    } else if (_mpAccelDeviceThread) {
        _mpAccelDeviceThread->deleteLater();
    }

    if (_mpAccelDevice && !_mpAccelDeviceThread) { 
        delete _mpAccelDevice;
    }
   
    _mpAccelDevice = nullptr;
    _mpAccelDeviceThread = nullptr; 
    qDebug() << "Port closed and AccelDevice resources cleaned up.";
}

// Slot for CsvWriter thread finished signal
void ThreadManager::onCsvWriterThreadFinished()
{
    qDebug() << "Received CsvWriter thread finished signal. Cleanup handled by deleteLater.";
    // Pointers will be nullified after deleteLater executes
    // _mCsvWriter = nullptr; // Not needed immediately, handled by deleteLater's event loop processing
    // _mCsvWriterThread = nullptr;
}

// Implement handlers for other actions (SetMode, SetAngle, SetSamplingRate) - Keep existing implementations
void ThreadManager::handleSetMode(uint8_t mode)
{
    if (!_mpAccelDevice) {
        qDebug() << "SetMode: AccelDevice not available";
        return;
    }
    ComDef_xpu8DeclareBuffer(toAccelDeviceRaw, ComDefMode_TypeDef);
    ComDef_xu8GetHeading(&toAccelDeviceRaw) = 0xAA;
    ComDef_xu8GetCommand(&toAccelDeviceRaw) = ComDef_xu8CommandMask(ComDefCommandMode, ComDefCommandMaskSet);
    ComDef_xu16GetPayloadLength(&toAccelDeviceRaw) = (uint16_t) sizeof(ComDefMode_TypeDef);
    ComDefMode_TypeDef * payload = (ComDefMode_TypeDef *)& ComDef_xpu8GetPayload(&toAccelDeviceRaw);
    payload->u8Mode = mode;
    ComDef_xu8GetCrc(&toAccelDeviceRaw) = 0xFA; // Placeholder CRC
    ComDef_xu8GetEnd(&toAccelDeviceRaw) = 0xBB;
    uint32_t length = ComDef_xpu32CalculateLength(&toAccelDeviceRaw);
    QByteArray commandData(reinterpret_cast<const char*>(toAccelDeviceRaw), length);
    QMetaObject::invokeMethod(
        _mpAccelDevice,
        "writeBuffer",
        Qt::QueuedConnection,
        Q_ARG(QByteArray, commandData),
        Q_ARG(VComHandler::Direction, VComHandler::Direction::Tx)
    );
    qDebug() << "SetMode command sent with mode:" << mode;
}

void ThreadManager::handleSetAngle(uint16_t angle)
{
    if (!_mpAccelDevice) {
        qDebug() << "SetAngle: AccelDevice not available";
        return;
    }
    ComDef_xpu8DeclareBuffer(toAccelDeviceRaw, ComDefAngle_TypeDef);
    ComDef_xu8GetHeading(&toAccelDeviceRaw) = 0xAA;
    ComDef_xu8GetCommand(&toAccelDeviceRaw) = ComDef_xu8CommandMask(ComDefCommandAngle, ComDefCommandMaskSet);
    ComDef_xu16GetPayloadLength(&toAccelDeviceRaw) = (uint16_t) sizeof(ComDefAngle_TypeDef);
    ComDefAngle_TypeDef * payload = (ComDefAngle_TypeDef *)& ComDef_xpu8GetPayload(&toAccelDeviceRaw);
    payload->u16Angle = angle;
    ComDef_xu8GetCrc(&toAccelDeviceRaw) = 0xFA; // Placeholder CRC
    ComDef_xu8GetEnd(&toAccelDeviceRaw) = 0xBB;
    uint32_t length = ComDef_xpu32CalculateLength(&toAccelDeviceRaw);
    QByteArray commandData(reinterpret_cast<const char*>(toAccelDeviceRaw), length);
    QMetaObject::invokeMethod(
        _mpAccelDevice,
        "writeBuffer",
        Qt::QueuedConnection,
        Q_ARG(QByteArray, commandData),
        Q_ARG(VComHandler::Direction, VComHandler::Direction::Tx)
    );
    qDebug() << "SetAngle command sent with angle:" << angle;
}

void ThreadManager::handleSetSamplingRate(uint16_t rate)
{
    if (!_mpAccelDevice) {
        qDebug() << "SetSamplingRate: AccelDevice not available";
        return;
    }
    ComDef_xpu8DeclareBuffer(toAccelDeviceRaw, ComDefSamplingRate_TypeDef);
    ComDef_xu8GetHeading(&toAccelDeviceRaw) = 0xAA;
    ComDef_xu8GetCommand(&toAccelDeviceRaw) = ComDef_xu8CommandMask(ComDefCommandSamplingRate, ComDefCommandMaskSet);
    ComDef_xu16GetPayloadLength(&toAccelDeviceRaw) = (uint16_t) sizeof(ComDefSamplingRate_TypeDef);
    ComDefSamplingRate_TypeDef * payload = (ComDefSamplingRate_TypeDef *)& ComDef_xpu8GetPayload(&toAccelDeviceRaw);
    payload->u16SamplingRate = rate;
    ComDef_xu8GetCrc(&toAccelDeviceRaw) = 0xFA; // Placeholder CRC
    ComDef_xu8GetEnd(&toAccelDeviceRaw) = 0xBB;
    uint32_t length = ComDef_xpu32CalculateLength(&toAccelDeviceRaw);
    QByteArray commandData(reinterpret_cast<const char*>(toAccelDeviceRaw), length);
    QMetaObject::invokeMethod(
        _mpAccelDevice,
        "writeBuffer",
        Qt::QueuedConnection,
        Q_ARG(QByteArray, commandData),
        Q_ARG(VComHandler::Direction, VComHandler::Direction::Tx)
    );
    qDebug() << "SetSamplingRate command sent with rate:" << rate;
} 
