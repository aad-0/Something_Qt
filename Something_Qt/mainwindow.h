#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>

#include "acceldevice.h"
#include "uiaddport.h"
#include "roadvisualizer.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    AccelDevice * _mpAccelDevice;
    QThread * _mpAccelDeviceThread;
    UiAddPort * _mpUiAddPort; // render all the time for now

    RoadVisualizer * _mpAccelView;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // void portChange (QString PortName, QSerialPort::BaudRate BaudRate,
    //     QSerialPort::DataBits DataBits, QSerialPort::Parity Parity,
    //     QSerialPort::StopBits StopBits, QSerialPort::FlowControl FlowControl);

    void portChange (QString const PortName, QSerialPort::BaudRate const BaudRate,
        QSerialPort::DataBits const DataBits, QSerialPort::Parity const Parity,
        QSerialPort::StopBits const StopBits,QSerialPort::FlowControl const FlowControl);

protected slots:
public slots:



private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
