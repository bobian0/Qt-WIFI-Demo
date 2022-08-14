#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QTcpSocket>
#include <QFile>
#include<QUdpSocket>
#include <QNetworkProxy>
#include <QDebug>
#include <QMessageBox>
#include <QHostAddress>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_buttonConnect_clicked();

private:
    Ui::MainWindow *ui;

    QTcpSocket *tcpSocket;

    QFile file; //文件对象
    QString fileName; //文件名字
    qint64 fileSize; //文件大小
    qint64 recvSize; //已经接收文件的大小

    bool isStart;   //标志位，是否为头部信息

    //利用UDP通信来发送文字
    QUdpSocket *udpSocket; //UDP套接字
};
#endif // MAINWINDOW_H
