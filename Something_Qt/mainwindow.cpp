#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "uiaddport.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    UiAddPort * addPort = new UiAddPort ();
    addPort->setParent( this->ui->addDevice);
    addPort->show ();

}

MainWindow::~MainWindow()
{
    delete ui;
}
