#include "acceldevice.h"

#include "comm.h"
#include <QMutexLocker>
#include <cassert>

/**
 * @brief AccelDevice::AccelDevice
 * @param parent
 */
AccelDevice::AccelDevice(QObject *parent)
    : VComHandler (_mBufferRx, _mBufferTx, _mMutexRx, _mMutexTx, parent)
{
    this->_mTimerReadOs = new QTimer (this);

    // connect signals to slots
    QObject::connect(this->_mTimerReadOs, SIGNAL( timeout() ), this, SLOT( readSystemBuffer() ));
    QObject::connect(this, SIGNAL( systemBufferRead() ), this, SLOT( stateMachine() ));


    // starts threads and timers
    // this->_mTimerReadOs.start();
}

int32_t AccelDevice::startTimer (uint32_t MSec)
{
    QObject::connect(this->_mTimerReadOs, SIGNAL( timeout() ), this, SLOT( readSystemBuffer() ));
    this->_mTimerReadOs->start(MSec);

    qDebug () << " Accel Device set Timer MSec " << MSec << " " << this;

    return 0;
}
int32_t AccelDevice::stopTimer ()
{
    this->_mTimerReadOs->stop();

    QObject::disconnect(this->_mTimerReadOs, SIGNAL( timeout() ), this, SLOT( readSystemBuffer() ));
    qDebug () << " Accel Device set Timer Stop " << this;

    return 0;
}



/**
 * @brief AccelDevice::stateMachine
 * @return
 */
int32_t AccelDevice::stateMachine ()
{
    /* static vars */
    static bool isSynced = false;

    /* thread safety */
    QMutexLocker rxLock (& this->_mMutexRx);

    while (1)
    {


    qDebug () << "############ HED RECEIVE BUFFER ###################";
    qDebug () << this->_mBufferRx;
    qDebug () << "############ END RECEIVE BUFFER ###################";

    // check buffer size
    if ( 0 >= this->_mBufferRx.size() )
    {
        return -1;
    }

    // this may looks like it should break the code, but it wraps the iterators soooo
    void const * pRxHead = this->_mBufferRx.data();

    // get synced
    if (! isSynced)
    {
        // check for common message structure
        while ( ( 0 < this->_mBufferRx.size() ) && (0xAA != ComDef_xu8GetHeading(pRxHead)) ) /// @todo make message structure more generic
        {
            this->_mBufferRx.erase (this->_mBufferRx.cbegin(), this->_mBufferRx.cbegin() +1);
            pRxHead = this->_mBufferRx.data();
        }

        isSynced = true;
    }

    /* assume synced */
    // sync is lost
    if (0xAA != ComDef_xu8GetHeading(pRxHead))
    {
        isSynced = false;
        qDebug () << "This Should Not Have happened in this project, srrry\r\n";
        continue;
        assert (0);
    }

    // message is Okay
    // check if there is enough data in buffer
    // not enough
    if (ComDef_xpu32CalculateLength(pRxHead) >= this->_mBufferRx.size())
    {
        return -1;
    }


    switch (ComDef_xu8GetCommand(pRxHead) & ComDefCommandModeMask)
    {
    case (ComDefCommandMode):
        switch (ComDef_xu8GetCommand(pRxHead) & ComDefCommandModeMask)
        {
        case (ComDefCommandMaskGet):
            break;
        case (ComDefCommandMaskSet):
            break;
        case (ComDefCommandMaskRet):
            break;
        }

        break;

    case (ComDefCommandImu):
        switch (ComDef_xu8GetCommand(pRxHead) & ComDefCommandModeMask)
        {
        case (ComDefCommandMaskGet):
            break;
        case (ComDefCommandMaskSet):
            break;
        case (ComDefCommandMaskRet):
            ComDefImu_TypeDef * pPayload = (ComDefImu_TypeDef*) & ComDef_xpu8GetPayload(pRxHead);
            qDebug () << "\r\n PAYLOAD FX " << pPayload->fX
                     << " \r\n PAYLOAD FY " << pPayload->fY
                     << " \r\n PAYLOAD FZ " << pPayload->fZ;

            emit this->AccelUpdated (pPayload->fX, pPayload->fY, pPayload->fZ);
            break;
        }
        break;

    case (ComDefCommandSamplingRate):
        switch (ComDef_xu8GetCommand(pRxHead) & ComDefCommandModeMask)
        {
        case (ComDefCommandMaskGet):
            break;
        case (ComDefCommandMaskSet):
            break;
        case (ComDefCommandMaskRet):
            break;
        }
        break;

    case (ComDefCommandAngle):
        switch (ComDef_xu8GetCommand(pRxHead) & ComDefCommandModeMask)
        {
        case (ComDefCommandMaskGet):
            break;
        case (ComDefCommandMaskSet):
            break;
        case (ComDefCommandMaskRet):
            qDebug () << " Angle Set ";
            break;
        }
        break;


    }

    this->_mBufferRx.erase (this->_mBufferRx.cbegin(), this->_mBufferRx.cbegin() +ComDef_xpu32CalculateLength(pRxHead) );
    }
    /// @todo
    return 0;
}
