#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "uiaddport.h"
#include "comm.h"
#include "threadmanager.h"

#define qDebugHex(__X) QString("%1").arg(__X,0,16)

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , _mpUiAddPort (nullptr)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    _mpUiAddPort = nullptr;
    _mpThreadManager = new ThreadManager(this);

    this->updateUiElements ();

    QObject::connect (this->ui->PushButton_OpenPort, &QPushButton::clicked, this, &MainWindow::openPortOnClick);
    QObject::connect (this->ui->PushButton_ClosePort, &QPushButton::clicked, this, [this](){
        if (this->_mpUiAddPort) {
            this->_mpUiAddPort->close();
            this->_mpUiAddPort = nullptr;
        }
        emit closePortRequested();
    });

    QObject::connect (this->ui->pushButton_SetMode, &QPushButton::clicked, this, [this](){
        uint8_t mode = static_cast<uint8_t>(this->ui->comboBox_SetModeSelection->currentData().toChar().unicode());
        emit setModeRequested(mode);
    });
    QObject::connect (this->ui->pushButton_SetAngle, &QPushButton::clicked, this, [this](){
        bool ok;
        uint16_t angle = static_cast<uint16_t>(this->ui->lineEdit_SetAngleInput->text().toUShort(&ok));
        if(ok) {
            emit setAngleRequested(angle);
        }
    });
    QObject::connect (this->ui->pushButton_SetSamplingRate, &QPushButton::clicked, this, [this](){
        bool ok;
        uint16_t rate = static_cast<uint16_t>(this->ui->lineEdit_SetSamplingRateInput->text().toUShort(&ok));
        if(ok) {
             emit setSamplingRateRequested(rate);
        }
    });

    connect(this, &MainWindow::portSettingsChanged, _mpThreadManager, &ThreadManager::handlePortChange);
    connect(this, &MainWindow::closePortRequested, _mpThreadManager, &ThreadManager::handleClosePort);
    connect(this, &MainWindow::setModeRequested, _mpThreadManager, &ThreadManager::handleSetMode);
    connect(this, &MainWindow::setAngleRequested, _mpThreadManager, &ThreadManager::handleSetAngle);
    connect(this, &MainWindow::setSamplingRateRequested, _mpThreadManager, &ThreadManager::handleSetSamplingRate);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::portChange (QString const PortName, QSerialPort::BaudRate const BaudRate,
    QSerialPort::DataBits const DataBits, QSerialPort::Parity const Parity,
    QSerialPort::StopBits const StopBits,QSerialPort::FlowControl const FlowControl)
{
    emit portSettingsChanged(PortName, BaudRate, DataBits, Parity, StopBits, FlowControl);
}

void MainWindow::openPortOnClick (void)
{
    if (nullptr != this->_mpUiAddPort) {
        this->_mpUiAddPort->raise();
        this->_mpUiAddPort->activateWindow();
        return;
    }

    this->_mpUiAddPort = new UiAddPort ();

    QObject::connect(
        this->_mpUiAddPort,
        &UiAddPort::addPort,
        this,
        &MainWindow::portChange
    );

    QObject::connect(
        this->_mpUiAddPort,
        &QObject::destroyed,
        this,
        [this]() { this->_mpUiAddPort = nullptr; }
    );

    this->_mpUiAddPort->show();
}

void MainWindow::closePortOnClick (void)
{
    if (this->_mpUiAddPort) {
         this->_mpUiAddPort->close();
         this->_mpUiAddPort = nullptr;
    }
    emit closePortRequested();
}

void MainWindow::updateUiElements (void)
{
    this->ui->comboBox_SetModeSelection->clear();
    this->ui->comboBox_SetModeSelection->addItem("Stream", QChar(ComDefModeStream));
    this->ui->comboBox_SetModeSelection->addItem("On Request", QChar(ComDefModeOnRequest));
    this->ui->comboBox_SetModeSelection->addItem("Stop", QChar(ComDefModeStop));

    // bool portIsOpen = (_mpThreadManager != nullptr);
    // this->ui->pushButton_SetMode->setEnabled(portIsOpen);
    // this->ui->pushButton_SetAngle->setEnabled(portIsOpen);
    // this->ui->pushButton_SetSamplingRate->setEnabled(portIsOpen);
    // this->ui->comboBox_SetModeSelection->setEnabled(portIsOpen);
    // this->ui->lineEdit_SetAngleInput->setEnabled(portIsOpen);
    // this->ui->lineEdit_SetSamplingRateInput->setEnabled(portIsOpen);

    // this->ui->PushButton_ClosePort->setEnabled(portIsOpen);
    // this->ui->PushButton_OpenPort->setEnabled(!portIsOpen);
}
