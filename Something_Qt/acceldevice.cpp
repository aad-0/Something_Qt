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
    this->_mTimerReadOs.setInterval(1);

    // connect signals to slots
    QObject::connect(&this->_mTimerReadOs, SIGNAL( timeout() ), this, SLOT( readSystemBuffer() ));
    QObject::connect(this, SIGNAL( systemBufferRead() ), this, SLOT( stateMachine() ));


    // starts threads and timers
    this->_mTimerReadOs.start();
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

    // check buffer size
    if ( 0 < this->_mBufferRx.size() )
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
        assert (0);
    }

    // message is Okay
    // check if there is enough data in buffer
    // not enough
    if (ComDef_xpu32CalculateLength(pRxHead) < this->_mBufferRx.size())
    {
        return -1;
    }


    switch (ComDef_xu8GetCommand(pRxHead) & ComDefCommandModeMask)
    {
    case (ComDefCommandMode):
        switch (ComDef_xu8GetCommand(pRxHead) & ComDefCommandMaskXet)
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
        switch (ComDef_xu8GetCommand(pRxHead) & ComDefCommandMaskXet)
        {
        case (ComDefCommandMaskGet):
            break;
        case (ComDefCommandMaskSet):
            break;
        case (ComDefCommandMaskRet):
            break;
        }
        break;

    case (ComDefCommandSamplingRate):
        switch (ComDef_xu8GetCommand(pRxHead) & ComDefCommandMaskXet)
        {
        case (ComDefCommandMaskGet):
            break;
        case (ComDefCommandMaskSet):
            break;
        case (ComDefCommandMaskRet):
            break;
        }
        break;

    }

    /// @todo
    return 0;
}
