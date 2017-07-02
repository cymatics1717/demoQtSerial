#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSerialPortInfo>
#include <QDebug>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    enumSerials();
    connect(ui->pushButton,&QPushButton::clicked,this,&MainWindow::openPort);
    connect(ui->lineEdit,&QLineEdit::returnPressed,this,&MainWindow::writeData);
    connect(ui->comboBox_3,static_cast<void(QComboBox::*)(const QString&)>(&QComboBox::currentTextChanged),
            [=](QString txt){ ui->lineEdit->setText(txt); });

    connect(&serial, &QSerialPort::readyRead,this, &MainWindow::handleReadyRead);
    connect(&serial, SIGNAL(error(QSerialPort::SerialPortError)), SLOT(handleError(QSerialPort::SerialPortError)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::handleReadyRead()
{
    QByteArray rep = serial.readAll();
    ui->textBrowser->append(QString("<font color=green>%1</font>").arg(QString(rep)));
}


void MainWindow::writeData()
{
    if(serial.isOpen()){
        QString cmd =  ui->lineEdit->text();
        if(!cmd.isEmpty()){
            ui->comboBox_3->addItem(cmd);
            ui->comboBox_3->setCurrentText(cmd);
        }
        QString req = QString("%1\n").arg(cmd);
        serial.write(req.toLocal8Bit());
        statusBar()->showMessage(QString("sended:[%1]").arg(req));
        ui->lineEdit->clear();
    }
}


void MainWindow::handleError(QSerialPort::SerialPortError err)
{
    serial_lst.clear();
    statusBar()->showMessage(qPrintable(err));
    ui->pushButton->setText("Open");
}

void MainWindow::enumSerials()
{
    ui->comboBox->clear();
    ui->comboBox_2->clear();
    serial_lst = QSerialPortInfo::availablePorts();
    QStringList com1,com2;
    for(auto x:serial_lst){
        com1<< QString("%1(%2)").arg(x.description()).arg(x.portName());
    }
    for(auto x:QSerialPortInfo::standardBaudRates()){
        com2<< QString("%1").arg(x);
    }

    ui->comboBox->addItems(com1);
    ui->comboBox_2->addItems(com2);
    ui->comboBox_2->setCurrentText("115200");
}

void MainWindow::openPort()
{
    enumSerials();

    QString status;
    if(serial_lst.size()==0){
        status = "no available serial port";
    } else if(ui->pushButton->text()=="Open"){
        if(serial.isOpen()) serial.close();
        auto port = serial_lst.at(ui->comboBox->currentIndex());
        serial.setPort(port);
        serial.setBaudRate(ui->comboBox_2->currentText().toInt());
        if(serial.open(QIODevice::ReadWrite)){
            //        serial.setBaudRate(QSerialPort::Baud115200);
            //        serial.setDataBits(QSerialPort::Data8);
            //        serial.setParity(QSerialPort::NoParity);
            //        serial.setStopBits(QSerialPort::OneStop);
            //        serial.setFlowControl(QSerialPort::NoFlowControl);
            ui->pushButton->setText("running");
            writeData();
            status = QString("current: %1:%2 %3\t\t%4 %5 [%6:%7]").arg(port.description())
                    .arg(port.portName()).arg(port.manufacturer())
                    .arg(port.serialNumber()).arg(port.systemLocation())
                    .arg(port.productIdentifier(),4,16,QChar('0'))
                    .arg(port.vendorIdentifier(),4,16,QChar('0'));
        } else {
            status = QString("open failed %1:%2").arg(serial.error()).arg(serial.errorString());
        }
    }
    statusBar()->showMessage(status);

}
