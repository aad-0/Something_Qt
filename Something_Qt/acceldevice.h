#ifndef ACCELDEVICE_H
#define ACCELDEVICE_H

#include <QObject>
#include <QThread>
#include <QTimer>
#include <QByteArray>
#include <QMutex>
#include "vcomhandler.h"
#include <cstdint>

class AccelDevice : public VComHandler
{
  Q_OBJECT
private:
    // QThread _mThread;

    QTimer * _mTimerReadOs;

    QByteArray  _mBufferRx;
    QByteArray  _mBufferTx;

    QMutex _mMutexRx;
    QMutex _mMutexTx;

private slots:
    int32_t stateMachine ();

  public:
    explicit AccelDevice(QObject *parent = nullptr);

  signals:
      void AccelUpdated (float const & X, float const & Y, float const & Z);
  public slots:
      int32_t startTimer (uint32_t MSec);
      int32_t stopTimer ();

};

#endif // ACCELDEVICE_H
