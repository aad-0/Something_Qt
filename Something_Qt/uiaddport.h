/**
 * @file uiaddport.h
 * @brief ui for adding port via signal
 */
#ifndef UIADDPORT_H
#define UIADDPORT_H

/* Includes */
#include <QWidget>
#include <QSerialPortInfo>
#include <QSerialPort>
#include <QMetaObject>
#include <QMetaEnum>

// class UiTerminal;


namespace Ui {
  class UiAddPort;
}

/* Class */
class UiAddPort : public QWidget
{
  Q_OBJECT

  public:
    explicit UiAddPort(QWidget *parent = nullptr);
    virtual ~UiAddPort();

  private:
    Ui::UiAddPort *ui;

    QList<QSerialPortInfo> m_AvailablePorts;
    // Enum Stuff

  protected:
  public:
    QList<QSerialPortInfo> const & getAvailablePorts (void) const;

    private slots:
      void setComboBoxes (void);

    // Button Slots
    void buttonAddPortClicked (void);
    // warning Window if port is already opened

    protected slots:

      public slots:
      void updateComs (void);
    void portAlreadyOpened (QString const & PortName);


  public:
signals:
    /**
     * @brief addPort This signal sends information about comport that user has set
     * @param pPortName Name Of Port
     * @note This may vary due to OS
     * @param pBaudRate BaudRate
     * @param pDataBits DataBits
     * @param pParity Parity
     * @param pStopBits StopBits
     * @param pFlowControl FlowControl
     */
    void addPort (QString const PortName,
                  QSerialPort::BaudRate const BaudRate,
                  QSerialPort::DataBits const DataBits,
                  QSerialPort::Parity const Parity,
                  QSerialPort::StopBits const StopBits,
                 QSerialPort::FlowControl const FlowControl);

    /**
     * @brief releaseRequest This is emited when close button is clicked
     *   this signal request for release, owner of this Ui is expected to delete the instance
     *   when this signal is emited
     */
    void releaseRequest (void);

    // friend class UiTerminal;
};

#endif // UIADDPORT_H
