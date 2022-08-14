#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QTcpServer> //监听套接字
#include <QTcpSocket> //通信套接字
#include <QFile>
#include <QTimer>
#include<QUdpSocket>
#include <QFileDialog>
#include <QDebug>
#include <QFileInfo>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void sendData(); //发送文件数据

private slots:
    void on_buttonFile_clicked();

    void on_buttonSend_clicked();

private:
    Ui::MainWindow *ui;

    QTcpServer *tcpServer; //监听套接字
        QTcpSocket *tcpSocket; //通信套接字

        QFile file; //文件对象
        QString fileName; //文件名字
        qint64 fileSize; //文件大小
        qint64 sendSize; //已经发送文件的大小

        QTimer timer; //定时器

        //利用UDP通信来发送文字
        QUdpSocket *udpSocket; //UDP套接字
};

#endif // MAINWINDOW_H
