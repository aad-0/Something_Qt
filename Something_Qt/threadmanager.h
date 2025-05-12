#ifndef THREADMANAGER_H
#define THREADMANAGER_H

#include <QObject>
#include <QThread>
#include <QSerialPort> // For QSerialPort enums
#include <QString>
#include <QByteArray> // Include QByteArray

// Forward declarations
class AccelDevice;

class ThreadManager : public QObject
{
    Q_OBJECT

private:
    AccelDevice* _mpAccelDevice;
    QThread* _mpAccelDeviceThread;

public:
    explicit ThreadManager(QObject *parent = nullptr);
    ~ThreadManager();

signals:
    // Signals to potentially update GUI (if needed later)
    // void statusMessage(const QString& message);
    // void accelDataUpdated(float x, float y, float z);

public slots:
    void handlePortChange(const QString& portName, QSerialPort::BaudRate baudRate,
                          QSerialPort::DataBits dataBits, QSerialPort::Parity parity,
                          QSerialPort::StopBits stopBits, QSerialPort::FlowControl flowControl);
    void handleClosePort();
    void handleSetMode(uint8_t mode);
    void handleSetAngle(uint16_t angle);
    void handleSetSamplingRate(uint16_t rate);

private slots:
    void onAccelDeviceDestroyed(); // Slot to handle cleanup when device is destroyed
    // void onAccelDataReceived(float x, float y, float z); // Example if data needs relaying


};

#endif // THREADMANAGER_H 