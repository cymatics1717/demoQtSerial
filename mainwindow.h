#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QKeyEvent>
#include <QMainWindow>
#include <QQueue>
#include <QTimer>
#include <QtSerialPort/QSerialPort>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void handleError(QSerialPort::SerialPortError);
private:
    void openPort();
    void writeData();
    void enumSerials();
    void handleReadyRead();
    QList<QSerialPortInfo> serial_lst;
    QSerialPort serial;

    Ui::MainWindow *ui;

};

#endif // MAINWINDOW_H
