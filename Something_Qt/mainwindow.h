#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include <QSerialPort>

#include "uiaddport.h"
#include "threadmanager.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    UiAddPort * _mpUiAddPort;
    ThreadManager * _mpThreadManager;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void portChange (QString const PortName, QSerialPort::BaudRate const BaudRate,
        QSerialPort::DataBits const DataBits, QSerialPort::Parity const Parity,
        QSerialPort::StopBits const StopBits,QSerialPort::FlowControl const FlowControl);

    /// Port
    void openPortOnClick (void);
    void closePortOnClick (void);

    void updateUiElements (void);

protected slots:
public slots:

signals:
    void portSettingsChanged(const QString& portName, QSerialPort::BaudRate baudRate,
                             QSerialPort::DataBits dataBits, QSerialPort::Parity parity,
                             QSerialPort::StopBits stopBits, QSerialPort::FlowControl flowControl);
    void closePortRequested();
    void setModeRequested(uint8_t mode);
    void setAngleRequested(uint16_t angle);
    void setSamplingRateRequested(uint16_t rate);

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
