#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    tcpSocket = new QTcpSocket(this);

    isStart = true;

    setWindowTitle("客户端 端口：6666");

    connect(tcpSocket, &QTcpSocket::readyRead,
    [=]()
    {
        //取出接收的内容
        QByteArray buf = tcpSocket->readAll();

        if( isStart ==true)
        {
            //接收头
            isStart = false;

            //初始化
            //文件名
            fileName = QString(buf).section("##", 0, 0);
            //文件大小
            fileSize = QString(buf).section("##", 1, 1).toInt();
            recvSize = 0;   //已经接收文件大小
            qDebug()<<" 客户端接受得文件大小:"<<fileSize<<endl;

            //关联文件名字
            file.setFileName(fileName);

            //只写方式，打开文件
            bool isOk = file.open(QIODevice::WriteOnly);
            if(false == isOk)
            {
                qDebug() << "WriteOnly error 49";

                tcpSocket->disconnectFromHost(); //断开连接
                tcpSocket->close(); //关闭套接字

                return; //如果打开文件失败，中断函数
            }

            //弹出对话框，显示接收文件的信息
            QString str1 = QString("接收的文件: [%1: %2kb]").arg(fileName).arg(fileSize/1024);
            QMessageBox::information(this, "文件信息", str1);

        }
        else //文件信息
        {

            qint64 len = file.write(buf);
     //       recvSize += len; //累计接收大小
            if(len>0)
            {
                recvSize+=len;
                qDebug() <<"recvSize接受文件的大小" <<recvSize;
            }


            if(recvSize == fileSize) //文件接收完毕
            {

                //先给服务发送(接收文件完成的信息)
                tcpSocket->write("file done");
                 file.close(); //关闭文件
                QMessageBox::information(this, "完成", "文件接收完成");

            }
       }

        }

    );
}

MainWindow::~MainWindow()
{
    delete ui;
}

//主动连接
void MainWindow::on_buttonConnect_clicked()
{
    //获取服务器的ip和端口
    QString ip = ui->lineEditIP->text();
    quint16 port = ui->lineEditPort->text().toInt();
    
    //主动和服务器连接
    tcpSocket->connectToHost(QHostAddress(ip), port);

    if (tcpSocket->waitForConnected(500)) {
        //连接成功
        qDebug() << "连接成功";
    }

    tcpSocket->setProxy(QNetworkProxy::NoProxy);

    isStart = true;
}
