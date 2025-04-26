/**
 * @file uiaddport.cpp
 * @brief This class provides add port ui
 */

#include "uiaddport.h"
#include "ui_uiaddport.h"

//    {
//    "BaudRate",
//    "DataBits",
//    "Parity",
//    "StopBits",
//    "FlowControl"
//};

/**
 * @brief UiAddPort::UiAddPort constructor of UiAddPort
 * @note User is expected to add ports via this view
 * @param parent parent of object
 */
UiAddPort::UiAddPort(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::UiAddPort)
{
  this->setAttribute(Qt::WA_DeleteOnClose);
  // set ui before editing elements of it
  ui->setupUi(this);
  // connections
  QObject::connect (this->ui->pushButton_Add, SIGNAL(clicked(bool)), this, SLOT(buttonAddPortClicked()));
  QObject::connect (this->ui->pushButtonCancel, SIGNAL(clicked(bool)), this, SLOT(deleteLater()));
  /* Set Ports */
  this->updateComs();
  /* Set Enums */
  this->setComboBoxes();

  qDebug () << "[I] UIAddPort Constructed at " << this;
}

/**
 * @brief UiAddPort::~UiAddPort Destructor Of UiAddPort
 * @retval None
 */
UiAddPort::~UiAddPort()
{
  delete ui;

  qDebug () << "[I] UIAddPort Vanished at " << this;
}


/**
 * @brief UiAddPort::updateComs This Updates available ports
 * @retval: None
 */
void UiAddPort::updateComs (void)
{
  // Clear old one
  this->m_AvailablePorts.clear();
  // Get new ones
  this->m_AvailablePorts = QSerialPortInfo::availablePorts();

  // Clear ComboList
  this->ui->comboBox_Port->clear();
  for (auto const & port : this->m_AvailablePorts)
    this->ui->comboBox_Port->addItem(port.portName());
}

/**
 * @brief UiAddPort::setComboBoxes This slot builds all ComboBoxes
 * @retval None
 */
void UiAddPort::setComboBoxes (void)
{
  int count;
  int offset;
  int i;
  QMetaEnum metaEnum;

  //  BaudRate
  this->ui->comboBox_BaudRate->clear();

  offset = QSerialPort::staticMetaObject.indexOfEnumerator("BaudRate");
  assert (-1 != offset);

  metaEnum = QSerialPort::staticMetaObject.enumerator(offset);
  count = metaEnum.keyCount();
  for (i = 0; i < count; ++i)
    this->ui->comboBox_BaudRate->addItem( metaEnum.key(i));

  // DataBits
  this->ui->comboBox_DataBits->clear();

  offset = QSerialPort::staticMetaObject.indexOfEnumerator("DataBits");
  assert (-1 != offset);

  metaEnum = QSerialPort::staticMetaObject.enumerator(offset);
  count = metaEnum.keyCount();
  for (i = 0; i < count; ++i)
    this->ui->comboBox_DataBits->addItem( metaEnum.key(i));

  // Parity
  this->ui->comboBox_Parity->clear();

  offset = QSerialPort::staticMetaObject.indexOfEnumerator("Parity");
  assert (-1 != offset);

  metaEnum = QSerialPort::staticMetaObject.enumerator(offset);
  count = metaEnum.keyCount();
  for (i = 0; i < count; ++i)
    this->ui->comboBox_Parity->addItem( metaEnum.key(i));

  // StopBits
  this->ui->comboBox_StopBits->clear();

  offset = QSerialPort::staticMetaObject.indexOfEnumerator("StopBits");
  assert (-1 != offset);

  metaEnum = QSerialPort::staticMetaObject.enumerator(offset);
  count = metaEnum.keyCount();
  for (i = 0; i < count; ++i)
    this->ui->comboBox_StopBits->addItem( metaEnum.key(i));

  // FlowControl
  this->ui->comboBox_FlowControl->clear();

  offset = QSerialPort::staticMetaObject.indexOfEnumerator("FlowControl");
  assert (-1 != offset);

  metaEnum = QSerialPort::staticMetaObject.enumerator(offset);
  count = metaEnum.keyCount();
  for (i = 0; i < count; ++i)
    this->ui->comboBox_FlowControl->addItem( metaEnum.key(i));

  // Role
  // offset = ModBusDevice::staticMetaObject.indexOfEnumerator("DeviceRole");
  // assert (-1 != offset);

  // metaEnum = ModBusDevice::staticMetaObject.enumerator(offset);
  // count = metaEnum.keyCount();
  // for (i = 0; i < count; ++i)
  //   this->ui->comboBox_Role->addItem( metaEnum.key(i));
}

/**
 * @brief UiAddPort::getAvailablePorts This function returns last/current ports listed
 *   in PortSelect
 * @retval List of ports in combolist
 */
QList<QSerialPortInfo> const & UiAddPort::getAvailablePorts (void) const
{
  return this->m_AvailablePorts;
}

/**
 * @brief UiAddPort::addPortClicked This Slot triggered when "AddPort" button is clicked
 *   this slot also emits another signal called addport
 *   essentailly it provides user selection
 *   @note Slot uses references instead of pointers
 *   @retval None
 */
void UiAddPort::buttonAddPortClicked (void)
{
  /**
    void addPort (QString const & PortName, QSerialPort::BaudRate const & BaudRate,
    QSerialPort::DataBits const & DataBits, QSerialPort::Parity const & Parity,
    QSerialPort::StopBits const & StopBits, QSerialPort::FlowControl const & FlowControl,
    UiAddPort::Role const & Role);
    */


  emit this->addPort(this->ui->comboBox_Port->currentText(),
      *(QSerialPort::BaudRate *)(this->ui->comboBox_BaudRate->currentData().data()),
      *(QSerialPort::DataBits *)(this->ui->comboBox_DataBits->currentData().data()),
      *(QSerialPort::Parity *)(this->ui->comboBox_Parity->currentData().data()),
      *(QSerialPort::StopBits *)(this->ui->comboBox_StopBits->currentData().data()),
      *(QSerialPort::FlowControl *)(this->ui->comboBox_FlowControl->currentData().data())
      );

}

/**
 * @brief UiAddPort::portAlreadyOpened This should be triggered by owner of the object
 *   with this slot, it can be shown the error to user via popup
 * @note Unimplemented
 * @param PortName
 * @retval None
 */
void UiAddPort::portAlreadyOpened (QString const & PortName)
{
  Q_UNUSED(PortName);
  Q_UNIMPLEMENTED();
}
