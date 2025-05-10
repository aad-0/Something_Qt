#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "uiaddport.h"
#include "comm.h"

#define qDebugHex(__X) QString("%1").arg(__X,0,16)

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , _mpAccelDevice (nullptr)
    , _mpUiAddPort (nullptr)

    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->updateUiElements ();


    this->_mpUiAddPort = nullptr;
    this->_mpAccelDevice = nullptr;
    // connect signals
    QObject::connect (this->ui->PushButton_OpenPort, SIGNAL(clicked(bool)), this, SLOT(openPortOnClick()));
    QObject::connect (this->ui->PushButton_ClosePort, SIGNAL(clicked(bool)), this, SLOT(closePortOnClick()));

    // Connect Control Signals
    QObject::connect (this->ui->pushButton_SetMode, SIGNAL(clicked(bool)), this, SLOT(setModeOnClick()));
    QObject::connect (this->ui->pushButton_SetAngle, SIGNAL(clicked(bool)), this, SLOT(setAngleOnClick()));
    QObject::connect (this->ui->pushButton_SetSamplingRate, SIGNAL(clicked(bool)), this, SLOT(setSamplingRateOnClick()));


    // QObject::connect(
    //     this->_mpUiAddPort,
    //     SIGNAL(addPort(QString const &, QSerialPort::BaudRate const &, QSerialPort::DataBits const &,
    //                    QSerialPort::Parity const &, QSerialPort::StopBits const &, QSerialPort::FlowControl const &)),
    //     this,
    //     SLOT(portChange(QString const &, QSerialPort::BaudRate const &, QSerialPort::DataBits const &,
    //                     QSerialPort::Parity const &, QSerialPort::StopBits const &, QSerialPort::FlowControl const &))
    //     );


    // QObject::connect (
    //     this->_mpUiAddPort,
    //     &UiAddPort::destroyed,
    //     this,
    //     [this] (QObject * parent)
    //     {
    //         Q_UNUSED(parent);
    //         this->_mpAccelDevice = nullptr;

    //     }


    //     );






}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::portChange (QString const PortName, QSerialPort::BaudRate const BaudRate,
    QSerialPort::DataBits const DataBits, QSerialPort::Parity const Parity,
    QSerialPort::StopBits const StopBits,QSerialPort::FlowControl const FlowControl)
{
    // create new device
    if (nullptr != this->_mpAccelDevice)
    {
        this->_mpAccelDeviceThread->quit();
        this->_mpAccelDeviceThread->wait();
    }


    this->_mpAccelDeviceThread = new QThread (this);
    this->_mpAccelDevice = new AccelDevice (nullptr);


    this->_mpAccelDevice->setPort(PortName, BaudRate, DataBits, Parity, StopBits, FlowControl);
    this->_mpAccelDevice->openPort(QIODeviceBase::ReadWrite);

    this->_mpAccelDevice->moveToThread(this->_mpAccelDeviceThread);

    QObject::connect (_mpAccelDeviceThread, SIGNAL(finished()), _mpAccelDevice, SLOT(deleteLater()));
    QObject::connect (_mpAccelDeviceThread, SIGNAL(finished()), _mpAccelDeviceThread, SLOT(deleteLater()));



    this->_mpAccelDeviceThread->start ();

    QMetaObject::invokeMethod(
        this->_mpAccelDevice,
        [this]() { this->_mpAccelDevice->startTimer(10); },
        Qt::QueuedConnection
        );

}


    /// Port
void MainWindow::openPortOnClick (void)
{
    if (nullptr != this->_mpUiAddPort)
        return;

    this->_mpUiAddPort = new UiAddPort ();

    // connect signals

    QObject::connect(
        this->_mpUiAddPort,
        SIGNAL(addPort(QString const &, QSerialPort::BaudRate const &, QSerialPort::DataBits const &,
                       QSerialPort::Parity const &, QSerialPort::StopBits const &, QSerialPort::FlowControl const &)),
        this,
        SLOT(portChange(QString const &, QSerialPort::BaudRate const &, QSerialPort::DataBits const &,
                        QSerialPort::Parity const &, QSerialPort::StopBits const &, QSerialPort::FlowControl const &))
        );

    // Odenmesi gereken kucuk bir bedel
    QObject::connect(
        this->_mpUiAddPort,
        & QPushButton::destroyed,
        this,
        [this] ()
            {
            this->_mpUiAddPort = nullptr;
            }
        );


    this->_mpUiAddPort->show();


}

void MainWindow::closePortOnClick (void)
{
    if (nullptr == this->_mpUiAddPort)
        return;

    // QObject::disconnect(
    //     this->_mpUiAddPort,
    //     SIGNAL(addPort(QString const &, QSerialPort::BaudRate const &, QSerialPort::DataBits const &,
    //                    QSerialPort::Parity const &, QSerialPort::StopBits const &, QSerialPort::FlowControl const &)),
    //     this,
    //     SLOT(portChange(QString const &, QSerialPort::BaudRate const &, QSerialPort::DataBits const &,
    //                     QSerialPort::Parity const &, QSerialPort::StopBits const &, QSerialPort::FlowControl const &))
    //     );

    this->_mpUiAddPort->hide();

    this->_mpUiAddPort->deleteLater();
    delete this->_mpUiAddPort;
    this->_mpUiAddPort = nullptr;
}

/// Controls
void MainWindow::setModeOnClick (void)
{
    if (nullptr == this->_mpAccelDevice)
        return;

    QByteArray toAccelDeviceTxBuffer;
    uint8_t SetModeChar = static_cast<uint8_t> (this->ui->comboBox_SetModeSelection->currentData().toChar().unicode());
    ComDef_xpu8DeclareBuffer(toAccelDeviceRaw, ComDefMode_TypeDef);

    ComDef_xu8GetHeading(&toAccelDeviceRaw) = 0xAA;
    ComDef_xu8GetCommand(&toAccelDeviceRaw) = ComDef_xu8CommandMask(ComDefCommandMode, ComDefCommandMaskSet); //ComDef_xu8CommandModeGet(ComDefCommandMode);
    ComDef_xu16GetPayloadLength(&toAccelDeviceRaw) = (uint16_t) sizeof(ComDefMode_TypeDef);
    ComDefMode_TypeDef * payload = (ComDefMode_TypeDef *)& ComDef_xpu8GetPayload(&toAccelDeviceRaw);
    ComDef_xu8GetCrc(&toAccelDeviceRaw) = 0xFA;
    ComDef_xu8GetEnd(&toAccelDeviceRaw) = 0xBB;

    payload->u8Mode = SetModeChar;

    uint32_t Length = ComDef_xpu32CalculateLength(&toAccelDeviceRaw);

    for (uint32_t i = 0; i < Length; ++ i)
    {
        toAccelDeviceTxBuffer += toAccelDeviceRaw [i];
    }

    this->_mpAccelDevice->writeBuffer(toAccelDeviceTxBuffer, VComHandler::Direction::Tx);

}
void MainWindow::setAngleOnClick (void)
{
    if (nullptr == this->_mpAccelDevice)
        return;

    QByteArray toAccelDeviceTxBuffer;
    uint16_t setAngle = static_cast<uint16_t> ( this->ui->lineEdit_SetAngleInput->text().toInt() );

    ComDef_xpu8DeclareBuffer(toAccelDeviceRaw, ComDefAngle_TypeDef);

    ComDef_xu8GetHeading(&toAccelDeviceRaw) = 0xAA;
    ComDef_xu8GetCommand(&toAccelDeviceRaw) = ComDef_xu8CommandMask(ComDefCommandAngle, ComDefCommandMaskSet); //ComDef_xu8CommandModeGet(ComDefCommandMode);
    ComDef_xu16GetPayloadLength(&toAccelDeviceRaw) = (uint16_t) sizeof(ComDefAngle_TypeDef);
    ComDefAngle_TypeDef * payload = (ComDefAngle_TypeDef *)& ComDef_xpu8GetPayload(&toAccelDeviceRaw);
    ComDef_xu8GetCrc(&toAccelDeviceRaw) = 0xFA;
    ComDef_xu8GetEnd(&toAccelDeviceRaw) = 0xBB;

    payload->u16Angle = setAngle;

    uint32_t Length = ComDef_xpu32CalculateLength(&toAccelDeviceRaw);

    for (uint32_t i = 0; i < Length; ++ i)
    {
        toAccelDeviceTxBuffer += toAccelDeviceRaw [i];
    }

    this->_mpAccelDevice->writeBuffer(toAccelDeviceTxBuffer, VComHandler::Direction::Tx);
}
void MainWindow::setSamplingRateOnClick (void)
{
    if (nullptr == this->_mpAccelDevice)
        return;

    QByteArray toAccelDeviceTxBuffer;
    uint16_t setSamplingRate = static_cast<uint16_t> ( this->ui->lineEdit_SetSamplingRateInput->text().toInt() );

    ComDef_xpu8DeclareBuffer(toAccelDeviceRaw, ComDefAngle_TypeDef);

    ComDef_xu8GetHeading(&toAccelDeviceRaw) = 0xAA;
    ComDef_xu8GetCommand(&toAccelDeviceRaw) = ComDef_xu8CommandMask(ComDefCommandSamplingRate, ComDefCommandMaskSet); //ComDef_xu8CommandModeGet(ComDefCommandMode);
    ComDef_xu16GetPayloadLength(&toAccelDeviceRaw) = (uint16_t) sizeof(ComDefSamplingRate_TypeDef);
    ComDefSamplingRate_TypeDef * payload = (ComDefSamplingRate_TypeDef *)& ComDef_xpu8GetPayload(&toAccelDeviceRaw);
    ComDef_xu8GetCrc(&toAccelDeviceRaw) = 0xFA;
    ComDef_xu8GetEnd(&toAccelDeviceRaw) = 0xBB;

    payload->u16SamplingRate = setSamplingRate;

    uint32_t Length = ComDef_xpu32CalculateLength(&toAccelDeviceRaw);

    for (uint32_t i = 0; i < Length; ++ i)
    {
        toAccelDeviceTxBuffer += toAccelDeviceRaw [i];
    }

    this->_mpAccelDevice->writeBuffer(toAccelDeviceTxBuffer, VComHandler::Direction::Tx);

}

void MainWindow::updateUiElements (void)
{
    // ModeSelection Options
    this->ui->comboBox_SetModeSelection->addItem("ModeStream", 0x01);
    this->ui->comboBox_SetModeSelection->addItem("ModeOnRequest", 0x02);
    this->ui->comboBox_SetModeSelection->addItem("ModeStop", 0x04);


}
