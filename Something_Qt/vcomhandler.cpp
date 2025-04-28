/**
 **********************************************
 * @file: vcomhandler.cpp
 * @brief: This file provides Implementations of VComHanlder
 * VCom handler is a wrapperLike function provides management
 * a SerialPort providing secure thread-safe system buffer
 * interaction functions and easy interface
 *
 * Each Buffer should be have its own mutex
 * These can be set by constructor or setX methods
 *
 **********************************************
 */

/* Includes */
#include "vcomhandler.h"

/* Defines */
#define _//qDebugHex(__X) QString("%1").arg(__X,0,16)

/* Class Method implemnentations */

/**
 * @brief: Constructor of VComHandler
 * @note: this constructor should be used with other set functions
 * @param[in,put] parent: parent of class
 * @retval: None
 */
VComHandler::VComHandler(QObject * parent): QSerialPort(parent)
{
  //qDebug () << "[I] VComHanlde: Initialized at: " << this;
}

/**
 * @brief: Constructor of VComHandler
 * @param[in,out] BufferRx: Receive Buffer Reference
 * @param[in,out] BufferTx: Transmit Buffer Reference
 * @param[in,out] MutexRx: Mutex of Receive Buffer Reference
#include <QMutex>
 * @param[in,out] MutexTx: Mutex of Transmit Buffer Reference
 * @param[in,out] parent: Parent of this class
 * @retval: None
 */
VComHandler::VComHandler(QByteArray & BufferRx, QByteArray & BufferTx,
    QMutex & MutexRx, QMutex & MutexTx,
    QObject * parent) : QSerialPort(parent)
{
  this->m_pBufferRx = & BufferRx;
  this->m_pBufferTx = & BufferTx;

  this->m_pBufferMutexRx = & MutexRx;
  this->m_pBufferMutexTx = & MutexTx;

  //qDebug () << "[I] VComHanlde: Initialized at: " << this;
}

/**
 * @brief: Destructor of VComHandler
 * @retval: None
 */
VComHandler::~VComHandler(void)
{
  // wait For Mutexes
  // Mutexes does not belong to this class
  if (this->isOpen())
    this->releasePort();

  //qDebug () << "[I] VcomHandle: Vanished at: " << this;
}

/**
 * @brief: This Function sets Port Config
 * @note: This function should be called first before opening the port
 * @param[in] PortName: Port Name, Ex: COM1, tty5
 * @note This may vary respect to operating system, aka linux:tty,tts
 * @param[in] BaudRate: BaudRate of USART Ex: 9600, 115200 ...
 * @note This might not very important for virtual coms, but it is
 * a thing for phsyical com ports
 * @param[in] DataBits: DataBİts Ex: 7,8,9
 * @param[in] Parity: Parity Ex: None, 1 Odd, Even
 * @param[in] Stop Bits: Stop Bits Ex: None, 1, 2, 1.5
 * @param[in] Flow Control: Flow Control Ex: RTS DTS, Hw Control, Software Control
 * @note: All these paremeters either can be set as quint32 or related Enum of QSerialPort
 * 	either way, they should obey the QSerialPort enums. QSerialPort enums are suggested
 * @retval: None
 */
void VComHandler::setPort (QString const & PortName, quint32 const & BaudRate,
    quint32 const & DataBits, quint32 const & Parity,
    quint32 const & StopBits, quint32 const & FlowControl)
{
  this->setPortName (PortName);

  this->setBaudRate ( static_cast<QSerialPort::BaudRate>(BaudRate) );
  this->setDataBits ( static_cast<QSerialPort::DataBits>(DataBits) );
  this->setParity ( static_cast<QSerialPort::Parity>(Parity) );
  this->setStopBits ( static_cast<QSerialPort::StopBits>(StopBits) );
  this->setFlowControl ( static_cast<QSerialPort::FlowControl>(FlowControl) );

  //qDebug () << "[I] VComHanlde: setPort at: " << this;
}

/**
 * @brief: This Function sets Port Config
 * @note: This function should be called first before opening the port
 * @param[in] PortName: Port Name, Ex: COM1, tty5
 * @note This may vary respect to operating system, aka linux:tty,tts
 * @param[in] BaudRate: BaudRate of USART Ex: 9600, 115200 ...
 * @note This might not very important for virtual coms, but it is
 * a thing for phsyical com ports
 * @param[in] DataBits: DataBİts Ex: 7,8,9
 * @param[in] Parity: Parity Ex: None, 1 Odd, Even
 * @param[in] Stop Bits: Stop Bits Ex: None, 1, 2, 1.5
 * @param[in] Flow Control: Flow Control Ex: RTS DTS, Hw Control, Software Control
 * @note: All these paremeters either can be set as quint32 or related Enum of QSerialPort
 * 	either way, they should obey the QSerialPort enums. QSerialPort enums are suggested
 * @retval: None
 */
void VComHandler::setPort (QString const & PortName, QSerialPort::BaudRate const & BaudRate,
    QSerialPort::DataBits const & DataBits, QSerialPort::Parity const & Parity,
    QSerialPort::StopBits const & StopBits, QSerialPort::FlowControl const & FlowControl)
{
  this->setPortName (PortName);

  this->setBaudRate (BaudRate);
  this->setDataBits (DataBits);
  this->setParity (Parity);
  this->setStopBits (StopBits);
  this->setFlowControl (FlowControl);

  //qDebug () << "[I] VComHanlde: setPort at: " << this;
}

/**
 * @brief: This function opens Port with Mode
 * This Function should be called after seting Port
 * If Something happened, this function sets a custom error string
 * @param[in] Mode: QIODeviceBase::OpenMode, or int value in the enum aka: Direction
 * @retval: None
 */
void VComHandler::openPort (quint32 const & Mode)
{
  if (this->isOpen ()) // if already open
  {
    this->setErrorString ("[!] VComHandler: Port Already Open\r\n");
    //qDebug () << "[!] VComHandler: Port Already Open\r\n";
  }
  else if (! this->open (static_cast<QIODeviceBase::OpenMode>(Mode))) // if failed to open
  {
    this->setErrorString ("[!] VComHandler: Port Could not Opened\r\n");
    //qDebug () << "[!] VComHandler: Port Could not Opened\r\n";
  }
  else // Successful to open the port
  {
    //qDebug () << "[I] VComHanlde: openPort: " << this->portName() << " At "<< this;
  }
}

/**
 * @brief: This function opens Port with Mode
 * This Function should be called after seting Port
 * If Something happened, this function sets a custom error string
 * @param[im] Mode: QIODeviceBase::OpenMode, or int value in the enum aka: Direction
 * @retval: None
 */
void VComHandler::openPort (QIODeviceBase::OpenMode const & Mode)
{
  if (this->isOpen ()) // if already open
  {
    this->setErrorString ("[!] VComHandler: Port Already Open\r\n");
    //qDebug () << "[!] VComHandler: Port Already Open\r\n";
  }
  else if (! this->open (Mode)) // if failed to open
  {
    this->setErrorString ("[!] VComHandler: Port Could not Opened\r\n");
    //qDebug () << "[!] VComHandler: Port Could not Opened\r\n";
  }
  else // Successful to open the port
  {
    //qDebug () << "[I] VComHanlde: openPort: " << this->portName() << " At "<< this;
  }
}

/**
 * @brief: This Function releases the port
 * @retval: None
 */
void VComHandler::releasePort (void)
{
  // cannot close a port that has not opened
  if (! this->isOpen())
    return;

  /* If set, Lock mutexes and release port */
  // This May cause long pooling !!
  // Wait for mutex
  //qDebug () << "[I] VComHanlder: releasePort, TryLock Mutex: " << this;

  // Locking mutexes
  // even when Mutexes are not set, releasing buffer should be available
  if (nullptr != this->m_pBufferRx)
    this->m_pBufferMutexRx->lock();

  if (nullptr != this->m_pBufferTx)
    this->m_pBufferMutexTx->lock();

  //qDebug () << "[I] VComHanlder: releasePort, Locked Mutex: " << this;

  // Close Port
  this->close();

  if (nullptr != this->m_pBufferRx)
    this->m_pBufferMutexTx->unlock();

  if (nullptr != this->m_pBufferTx)
    this->m_pBufferMutexRx->unlock();

  //qDebug () << "[I] VComHanlder: releasePort, UNLocked Mutex: " << this;
}

/**
 * @brief: This Function sets Mutex as Buffer & Port
 * Buffer and Port mutexes are same
 * This Function may need to be called after ReleaseMutex
 * @param[in,out] Src: QMutex
 * @param[in] Direction: Direction Rx:1 or Tx:2 Aka: VcomHandlerDirection
 * @retval: None
 */
void VComHandler::setMutex(QMutex & Src, VComHandler::Direction const & Direction)
{
  assert (xVCOMHANDLER_IsDirectionValid(Direction));
  // Seting Buffers and mutexes should be done before opening the port
  assert (! this->isOpen());

  if (Direction == VComHandler::Direction::Rx)
  {
    this->m_pBufferMutexRx = &Src;
    //qDebug () << "[I] VComHanlder: setMutex: NewMutexRx " << this->m_pBufferMutexRx<< " " << this;
  }
  else
  {
    this->m_pBufferMutexTx = &Src;
    //qDebug () << "[I] VComHanlder: setMutex: NewMutexTx " << this->m_pBufferMutexTx<< " " << this;
  }
}

/**
 * @brief: This function releases mutex, TOOD check insanity of this function in usage
 * @param[in] Direction: Direction Rx:1 or Tx:2 Aka: VcomHandlerDirection
 * @retval: None
 */
void VComHandler::releaseMutex(VComHandler::Direction const & Direction)
{
  assert (xVCOMHANDLER_IsDirectionValid(Direction));
  // Seting Buffers and mutexes should be done before opening the port
  assert (! this->isOpen());

  QMutex * pMutex = (Direction == VComHandler::Direction::Rx) ?	\
                    (this->m_pBufferMutexRx) : (this->m_pBufferMutexTx);

  // Cannot release a mutex if it is not set
  if (nullptr == pMutex)
    return;

  pMutex->lock();
  if (VComHandler::Direction::Rx == Direction)
    this->m_pBufferMutexRx = nullptr;
  else
    this->m_pBufferMutexTx = nullptr;

  pMutex->unlock();
  delete pMutex;
}

/**
 * @brief: This returns Size of current Buffer
 * @param[in] Direction: Direction Rx:1 or Tx:2 Aka: VcomHandlerDirection
 * @retval: size of Current Buffer
 */
qsizetype VComHandler::getBufferSize (VComHandler::Direction const & Direction) const
{
  assert (xVCOMHANDLER_IsDirectionValid(Direction));

  QByteArray const * const pBuffer = (Direction == VComHandler::Direction::Rx) ?	\
                                     (this->m_pBufferRx) : (this->m_pBufferTx);

  if (nullptr != pBuffer)
    return pBuffer->size();

  return 0;
}

/**
 * @brief: This function returns pointer of buffer
 * @note: User cannot write this buffer via this function
 * @param[in] Direction: Direction Rx:1 or Tx:2 Aka: VcomHandlerDirection
 * @retval: Pointer of QByteArray
 */
QByteArray const * VComHandler::getBuffer (VComHandler::Direction const & Direction) const
{
  assert (xVCOMHANDLER_IsDirectionValid(Direction));

  QByteArray const * const pBuffer = (Direction == VComHandler::Direction::Rx) ?	\
                                     (this->m_pBufferRx) : (this->m_pBufferTx);

  return (QByteArray const *)(pBuffer);
}

/**
 * @brief: This function writes to Buffer
 * @note: This function implemented due to further simulator needs
 * @note: When Direction is Tx, Device transmits new data
 * @param[in,out] Src: Data to Write
 * @param[in] Direction: Direction Rx:1 or Tx:2 Aka: VcomHandlerDirection
 * @retval None
 */
void VComHandler::writeBuffer (QByteArray const & Src, VComHandler::Direction const & Direction)
{
  assert (xVCOMHANDLER_IsDirectionValid(Direction));

  QByteArray * const pBuffer = (Direction == VComHandler::Direction::Rx) ?	\
                               (this->m_pBufferRx) : (this->m_pBufferTx);

  QMutex * const pMutex = (Direction == VComHandler::Direction::Rx) ?	\
                          (this->m_pBufferMutexRx) : (this->m_pBufferMutexTx);

  // Do not accept any call if mutex is not set
  assert (nullptr != pMutex);
  // Cannot write with empty buffer, User should be sure what he is using
  assert (this->isBufferSet(Direction));
  // cannot write if port is not opened
  assert (this->isOpen());

  pMutex->lock();
  if (nullptr != pBuffer)
    *(pBuffer) += Src;

  pMutex->unlock();
  if (VComHandler::Direction::Tx == Direction)
    this->writeSystemBuffer();
}

/**
 * @brief: This Function Clears Seleced Buffer
 * @param[in] Direction: Direction Rx:1 or Tx:2 Aka: VcomHandlerDirection
 * @retval: None
 */
void VComHandler::clearBuffer (VComHandler::Direction const & Direction)
{
  QByteArray * const pBuffer = (Direction == VComHandler::Direction::Rx) ?	\
                               (this->m_pBufferRx) : (this->m_pBufferTx);

  QMutex * const pMutex = (Direction == VComHandler::Direction::Rx) ?	\
                          (this->m_pBufferMutexRx) : (this->m_pBufferMutexTx);

  // Do not accept any call if mutex is not set
  assert (nullptr != pMutex);
  // Every Buffer should have a mutex
  assert (nullptr != pBuffer);

  pMutex->lock();
  pBuffer->clear();
  pMutex->unlock();
}

/**
 * @brief This Reads From User Buffer and Writes into Dst respect to Length
 * @note: While Reading, it also pops from UserBuffer
 * @param[in,out] Dst: Destination wher read data kept
 * @param[in] Length: Length to read from
 * @param[in] Direction: Direction Rx:1 or Tx:2 Aka: VcomHandlerDirection
 * @retval: None
 */
void VComHandler::readBuffer (QByteArray & Dst, size_t const & Length, VComHandler::Direction const & Direction)
{
  Q_UNUSED(Length);
  assert (xVCOMHANDLER_IsDirectionValid(Direction));

  QByteArray * const pBuffer = (VComHandler::Direction::Rx == Direction) ?	\
                               (this->m_pBufferRx) : (this->m_pBufferTx);

  QMutex * const pMutex = (VComHandler::Direction::Rx == Direction) ?	\
                          (this->m_pBufferMutexRx) : (this->m_pBufferMutexTx);

  // Do not accept any call if mutex is not set
  assert (nullptr != pMutex);
  //if (nullptr == pMutex)
  //    return;

  if ((! this->isBufferSet(Direction)) || (! this->isOpen()) )
    return;

  pMutex->lock();
  // Dirty Trick
  //if (VComHandler::Direction::Rx == Direction)
  //{
  //    Dst += pBuffer->remove (0, Length);
  //    pBuffer->squeeze();
  //}
  // else // Debug Stuff
  //memcpy(Dst.data(), pBuffer->data(), Length);
  Dst = * pBuffer;

  pMutex->unlock();
}

/**
 * @brief: This Sets User Buffer,
 * @param[in,out] Src: Buffer to set Respect to Direction
 * @note This set method provides easy manipulation of buffers
 * @param[in] Direction: Rx or Tx Buffer to set
 * @retval: None
 */
void VComHandler::setBuffer(QByteArray & Src, VComHandler::Direction const & Direction)
{
  assert (xVCOMHANDLER_IsDirectionValid(Direction));
  // Seting Buffers and mutexes should be done before opening the port
  assert (! this->isOpen());

  QMutex * const pMutex = (VComHandler::Direction::Rx == Direction) ?	\
                          (this->m_pBufferMutexRx) : (this->m_pBufferMutexTx);

  // Do not accept any call if mutex is not set
  assert (nullptr != pMutex);

  pMutex->lock();

  if (VComHandler::Direction::Rx == Direction)
    this->m_pBufferRx = &Src;
  else
    this->m_pBufferTx = &Src;

  pMutex->unlock();
}

/**
 * @brief: This Function Releases user Buffer
 * @param[im] Direction: Direction to release
 * @retval: None
 */
void VComHandler::releaseBuffer(VComHandler::Direction const & Direction)
{
  assert (xVCOMHANDLER_IsDirectionValid(Direction));
  // Seting Buffers and mutexes should be done before opening the port
  assert (! this->isOpen());

  QMutex * const pMutex = (VComHandler::Direction::Rx == Direction) ?	\
                          (this->m_pBufferMutexRx) : (this->m_pBufferMutexTx);

  // Do not accept any call if mutex is not set
  if (nullptr == pMutex)
    return;

  if (! this->isBufferSet(Direction))
    return;

  // Lock mutex in case of any writing activity
  pMutex->lock();
  if (VComHandler::Direction::Rx == Direction)
    this->m_pBufferRx = nullptr;
  else
    this->m_pBufferTx = nullptr;

  pMutex->unlock();
}

/**
 * @brief: Returns Buffer state
 * @param[in] Direction: Direction to release
 * @retval: True if Buffer is set, False otherwise
 */
quint32 VComHandler::isBufferSet (VComHandler::Direction const & Direction)
{
  QByteArray const * pBuffer = (VComHandler::Direction::Rx == Direction) ?	\
                               (this->m_pBufferRx) : (this->m_pBufferTx);

  return (nullptr != pBuffer);
}

/**
 * @brief: This reads from sytem Buffer & writes into User's Buffer
 * @note: This function should be connected with readyRead or with a timer
 * @retval: None
 */
void VComHandler::readSystemBuffer (void)
{
  // To Read system Buffer, port should be opened Note: Buffer is not needed
  // Due to clear system buffer
  assert (this->isOpen ());

  // If timer triggers when system buffer is empty
  if (! this->bytesAvailable())
    return;

  // if buffer set
  if (nullptr != this->m_pBufferMutexRx)
    this->m_pBufferMutexRx->lock();
  QByteArray const dataRead = this->readAll();

  // When Device has no Rx Buffer, read, but not write
  // this helps with sytem Buffer's size
  if (nullptr != this->m_pBufferRx)
    *(this->m_pBufferRx) += dataRead;

  // if buffer set
  if (nullptr != this->m_pBufferMutexRx)
    this->m_pBufferMutexRx->unlock();

  //qDebug () << "System Buffer Read: " << dataRead;
  //qDebug () << "User Buffer Rx is now " << this->m_pBufferRx->toHex();

  // Emit Signal
  emit this->systemBufferRead();
}

/**
 * @brief: This reads from User's Buffer & writes to system Buffer
 * @note: This Function should be connected to a timer or signal
 * also this method can be called manually
 * @retval: None
 */
void VComHandler::writeSystemBuffer (void)
{
  // Cannot write to a system buffer if it is not open
  assert (this->isOpen());

  // If timer triggers with empty Tx
  if (this->m_pBufferTx->isEmpty())
    return;

  // if buffer set
  if (this->m_pBufferMutexTx)
    this->m_pBufferMutexTx->lock();

  // There is no further check whether bufferTx is null or not
  // since bufer pointer cannot be set if mutex is missing
  quint32 err = this->write (* (this->m_pBufferTx));

  // if buffer set
  if (this->m_pBufferMutexTx)
    this->m_pBufferMutexTx->unlock();

  //qDebug () << "System Buffer Write Tx: " << * (this->m_pBufferTx);
  //qDebug () << "User Buffer is now Tx: " << this->m_pBufferTx->toHex();
  //qDebug () << "Write Err " << err;

  // clear tx buffer so that bytes sent wont be sended again
  this->m_pBufferTx->clear();
  // Emit Signal
  emit this->systemBufferWritten();
}

/**
 * @brief: This Is Not implemented TODO
 * @retval: None
 */
void VComHandler::ErrorHandler (void)
{
  Q_UNIMPLEMENTED();
}
