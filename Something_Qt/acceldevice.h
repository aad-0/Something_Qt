#ifndef ACCELDEVICE_H
#define ACCELDEVICE_H

#include <QObject>
#include <QThread>
#include <QTimer>
#include <QByteArray>
#include <QMutex>
#include "vcomhandler.h"

class AccelDevice : public VComHandler
{
private:
    // QThread _mThread;

    QTimer _mTimerReadOs;
    int32_t stateMachine ();

    QByteArray  _mBufferRx;
    QByteArray  _mBufferTx;

    QMutex _mMutexRx;
    QMutex _mMutexTx;

  public:
    explicit AccelDevice(QObject *parent = nullptr);

};

#endif // ACCELDEVICE_H
