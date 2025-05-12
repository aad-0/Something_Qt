#include "threadmanager.h"
#include "acceldevice.h" // For AccelDevice class
#include "comm.h"        // For communication definitions
#include <QDebug>

ThreadManager::ThreadManager(QObject *parent)
    : QObject(parent),
      _mpAccelDevice(nullptr),
      _mpAccelDeviceThread(nullptr)
{
    qDebug() << "ThreadManager created";
}

ThreadManager::~ThreadManager()
{
    handleClosePort(); // Ensure cleanup on destruction
    qDebug() << "ThreadManager destroyed";
}

void ThreadManager::handlePortChange(const QString& portName, QSerialPort::BaudRate baudRate,
                                   QSerialPort::DataBits dataBits, QSerialPort::Parity parity,
                                   QSerialPort::StopBits stopBits, QSerialPort::FlowControl flowControl)
{
    if (_mpAccelDevice) {
        qDebug() << "Closing existing AccelDevice before opening new port";
        handleClosePort();
    }

    _mpAccelDeviceThread = new QThread(this); // Parent to ThreadManager for auto-deletion if ThreadManager is deleted
    _mpAccelDevice = new AccelDevice(nullptr); // No parent, will be moved to thread and deleted with thread

    _mpAccelDevice->setPort(portName, baudRate, dataBits, parity, stopBits, flowControl);
    // Attempt to open port, check for success if AccelDevice::openPort provides feedback
    _mpAccelDevice->openPort(QIODeviceBase::ReadWrite);

    _mpAccelDevice->moveToThread(_mpAccelDeviceThread);

    // Connect AccelDevice's destroyed signal to a slot for cleanup
    connect(_mpAccelDevice, &AccelDevice::destroyed, this, &ThreadManager::onAccelDeviceDestroyed);
    // Ensure thread quits when AccelDevice is deleted (e.g., if deleted by itself or an error)
    connect(_mpAccelDevice, &AccelDevice::destroyed, _mpAccelDeviceThread, &QThread::quit);

    // When thread finishes, delete AccelDevice and the thread itself
    connect(_mpAccelDeviceThread, &QThread::finished, _mpAccelDevice, &QObject::deleteLater);
    connect(_mpAccelDeviceThread, &QThread::finished, _mpAccelDeviceThread, &QObject::deleteLater);

    _mpAccelDeviceThread->start();

    // Start the AccelDevice's internal timer via a queued connection
    QMetaObject::invokeMethod(
        _mpAccelDevice,
        [this]() { 
            if (_mpAccelDevice) { // Check if _mpAccelDevice is still valid
                _mpAccelDevice->startTimer(0); 
            }
        },
        Qt::QueuedConnection
    );

    qDebug() << "AccelDevice created and moved to thread:" << _mpAccelDeviceThread;
    // If AccelDevice has signals for data (e.g., AccelUpdated), connect them here if ThreadManager needs to act on them
    // connect(_mpAccelDevice, &AccelDevice::AccelUpdated, this, &ThreadManager::onAccelDataReceived);
}

void ThreadManager::onAccelDeviceDestroyed()
{
    qDebug() << "AccelDevice was destroyed, cleaning up pointers in ThreadManager";
    _mpAccelDevice = nullptr;       // Nullify pointer as the object is gone
    // _mpAccelDeviceThread will be deleted by its finished signal connection
    // or if ThreadManager is destroyed and _mpAccelDeviceThread is its child.
    // If thread is still running, it should be quit by other means (e.g. AccelDevice::destroyed connection)
}

void ThreadManager::handleClosePort()
{
    if (_mpAccelDeviceThread && _mpAccelDeviceThread->isRunning()) {
        qDebug() << "Stopping AccelDevice thread";
        // Stop the timer in AccelDevice first, if possible, via queued connection
        if (_mpAccelDevice) {
             QMetaObject::invokeMethod(_mpAccelDevice, "stopTimer", Qt::QueuedConnection);
        }
        _mpAccelDeviceThread->quit();
        if (!_mpAccelDeviceThread->wait(3000)) { // Wait for max 3 seconds
            qDebug() << "AccelDevice thread did not quit gracefully, terminating...";
            _mpAccelDeviceThread->terminate();
            _mpAccelDeviceThread->wait(); // Wait for termination
        }
    } else if (_mpAccelDeviceThread) {
        // If thread is not running but exists, it might just need deletion
        _mpAccelDeviceThread->deleteLater();
    }

    // _mpAccelDevice will be deleted when _mpAccelDeviceThread finishes due to connections.
    // If _mpAccelDevice was not moved to thread or thread didn't start, delete manually.
    if (_mpAccelDevice && !_mpAccelDeviceThread) { // Or if thread didn't own it
        delete _mpAccelDevice;
    }
   
    _mpAccelDevice = nullptr;
    _mpAccelDeviceThread = nullptr; // Pointer is now invalid
    qDebug() << "Port closed and resources cleaned up.";
}

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

    // Invoke writeBuffer on AccelDevice's thread
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