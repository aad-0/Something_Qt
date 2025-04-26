/**
 **********************************************
 * @file: vcomhandler.h
 * @brief: This VComHanlder Header file
 * this file provides thread-safe serial port interface
 **********************************************
 */


#ifndef VCOMHANDLER_H
#define VCOMHANDLER_H

/* Includes */
#include <QSerialPort>
#include <QByteArray>
#include <QMutex>
#include <QString>
#include <QDebug>
#include <string>

#include <QIODeviceBase>

/* Function Like Macros */
/**
 * @brief: This function checks if Direction is a valid one or not
 * @param 1: Direction
 * @retval: Bool, if it is a valid direction: True, otherwise: False
 */
#define xVCOMHANDLER_IsDirectionValid(__D)	\
  (	\
    ( __D == VComHandler::Direction::Tx ) ||	\
    ( __D == VComHandler::Direction::Rx )	\
    )

/* Classes */
/**
 * @brief The VComHandler class This class provides thread-safe serialport interaction
 *
 */
class VComHandler : public QSerialPort
{
  Q_OBJECT
  public:

    /* Enums */
    /**
     * @brief The VComHandler::Direction enum
     * this enumeration is used in parameters and specifies direction
     * for related function
     */
    enum Direction
    {
      Rx = 1, /**< Rx Direction of VComHandler */
      Tx = 2, /**< Tx Direction of VComHandler */
      RESERVED = -1 /**< This is reserved, not use */
    };
    Q_ENUM(Direction);

    /* Nested struct */
    //struct PortInfo /* Future use */
    //{
    //    QString PortName;
    //    QSerialPort::BaudRate BaudRate;
    //    QSerialPort::DataBits DataBits;
    //    QSerialPort::Parity Parity;
    //    QSerialPort::StopBits StopBits;
    //    QSerialPort::FlowControl FlowControl;
    //};

  public:
    explicit VComHandler(QObject * parent = nullptr);

    explicit VComHandler(QByteArray & BufferRx, QByteArray & BufferTx,
        QMutex & MutexRx, QMutex & MutexTx,
        QObject * parent = nullptr);

    virtual ~VComHandler(void);

  private:
    //public:
		QByteArray * m_pBufferRx = nullptr; /**< SerialPort Buffer, User should write from here */
		QByteArray * m_pBufferTx = nullptr; /**< SerialPort Buffer, User should read from here */
		QMutex * m_pBufferMutexRx = nullptr; /**< Mutex Of Rx Buffer, */
		QMutex * m_pBufferMutexTx = nullptr; /**< Mutex Of Tx Buffer */

  private:

  protected:

  public:
    /* Port Functions */
    void setPort (QString const & PortName, quint32 const & BaudRate,
        quint32 const & DataBits, quint32 const & Parity,
        quint32 const & StopBits, quint32 const & FlowControl);

    void setPort (QString const & PortName, QSerialPort::BaudRate const & BaudRate,
        QSerialPort::DataBits const & DataBits, QSerialPort::Parity const & Parity,
        QSerialPort::StopBits const & StopBits, QSerialPort::FlowControl const & FlowControl);

    void openPort (quint32 const & Mode);
    void openPort (QIODeviceBase::OpenMode const & Mode);
    void releasePort (void);

    /* User Buffer Managements */
    void setMutex(QMutex & Src, VComHandler::Direction const & Direction);
    void releaseMutex(VComHandler::Direction const & Direction);

    void setBuffer(QByteArray & Src, VComHandler::Direction const & Direction);
    void releaseBuffer(VComHandler::Direction const & Direction);
    qsizetype getBufferSize (VComHandler::Direction const & Direction) const;
    quint32 isBufferSet (VComHandler::Direction const & Direction);
    QByteArray const * getBuffer (VComHandler::Direction const & Direction) const;

  public:
signals:
    /* Signals */
    void systemBufferRead (void); /**< @brief Signal that is emited when system buffer read happens */
    void systemBufferWritten (void); /**< @brief Signal that is emited when system buffer write happens */

    private
      slots:
      void ErrorHandler (void); /**< @brief This is not implemented */

    protected
      slots:
      /* System Buffer related slots */
      void writeSystemBuffer (void); /**< @brief This slot writes Tx buffer to ComPort */
    void readSystemBuffer (void); /**< @brief This slot reads from ComPort Buffer & writes to Rx buffer */

    public
      slots:
      void clearBuffer (VComHandler::Direction const & Direction);
    void readBuffer (QByteArray & Dst, size_t const & Length, VComHandler::Direction const & Direction);
    void writeBuffer (QByteArray const & Src, VComHandler::Direction const & Direction);
};
#endif // VCOMHANDLER_H
