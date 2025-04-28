#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "uiaddport.h"
#include "roadvisualizer.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , _mpAccelDevice (nullptr)
    , _mpUiAddPort (nullptr)

    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    this->_mpUiAddPort = new UiAddPort (this->ui->addDevice);
    //this->_mpUiAddPort = new UiAddPort (nullptr);

    // this->_mpAccelView = new RoadVisualizer (this->ui->accelView);


    this->_mpUiAddPort->show();
    // this->_mpAccelView->show();
    // this->_mpAccelView->initSurface();


    // connect signals
    QObject::connect(
        this->_mpUiAddPort,
        SIGNAL(addPort(QString const &, QSerialPort::BaudRate const &, QSerialPort::DataBits const &,
                       QSerialPort::Parity const &, QSerialPort::StopBits const &, QSerialPort::FlowControl const &)),
        this,
        SLOT(portChange(QString const &, QSerialPort::BaudRate const &, QSerialPort::DataBits const &,
                        QSerialPort::Parity const &, QSerialPort::StopBits const &, QSerialPort::FlowControl const &))
        );


    QObject::connect (
        this->_mpUiAddPort,
        &UiAddPort::destroyed,
        this,
        [this] (QObject * parent)
        {
            Q_UNUSED(parent);
            this->_mpAccelDevice = nullptr;

        }


        );






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

    QObject::disconnect (_mpAccelDevice, SIGNAL(AccelUpdated(float,float,float)), _mpAccelView, SLOT(updateSurface(float,float,float)));

        delete this->_mpAccelDeviceThread;
        delete this->_mpAccelDevice;
    }

    this->_mpAccelDeviceThread = new QThread (this);
    this->_mpAccelDevice = new AccelDevice (nullptr);


    this->_mpAccelDevice->setPort(PortName, BaudRate, DataBits, Parity, StopBits, FlowControl);
    this->_mpAccelDevice->openPort(QIODeviceBase::ReadWrite);

    this->_mpAccelDevice->moveToThread(this->_mpAccelDeviceThread);

    QObject::connect (_mpAccelDeviceThread, SIGNAL(finished()), _mpAccelDevice, SLOT(deleteLater()));
    QObject::connect (_mpAccelDeviceThread, SIGNAL(finished()), _mpAccelDeviceThread, SLOT(deleteLater()));

    QObject::connect (_mpAccelDevice, SIGNAL(AccelUpdated(float,float,float)), this->ui->accelView, SLOT(updateSurface(float,float,float)));


    this->_mpAccelDeviceThread->start ();

    QMetaObject::invokeMethod(
        this->_mpAccelDevice,
        [this]() { this->_mpAccelDevice->startTimer(0); },
        Qt::QueuedConnection
        );

}
