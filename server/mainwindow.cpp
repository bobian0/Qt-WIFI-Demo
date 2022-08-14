#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //监听套接字
    tcpServer = new QTcpServer(this);

    //监听
    tcpServer->listen(QHostAddress::Any, 8888);
    setWindowTitle("服务器端口为：8888");

    //两个按钮都不能按
    ui->buttonFile->setEnabled(false);
    ui->buttonSend->setEnabled(false);

    //如果客户端成功和服务器连接
    //tcpServer会自动触发 newConnection()
    connect(tcpServer, &QTcpServer::newConnection,
    [=]()
    {
        //取出建立好连接的套接字
        tcpSocket = tcpServer->nextPendingConnection();
        //获取对方的ip和端口
        QString ip = tcpSocket->peerAddress().toString();
        quint16 port = tcpSocket->peerPort();

        QString str = QString("[%1:%2] 成功连接").arg(ip).arg(port);
        //ui->textEdit->setText(str); //显示到编辑区

        //成功连接后，才能按选择文件
        ui->buttonFile->setEnabled(true);

    }
    );

    connect(&timer, &QTimer::timeout,
            [=]()
            {
                //关闭定时器
                timer.stop();
                //发送文件
                sendData();
            }

            );

}

MainWindow::~MainWindow()
{
    delete ui;
}

//选择文件的按钮
void MainWindow::on_buttonFile_clicked()
{
    //QString filePath = QFileDialog::getOpenFileName(this, "open", "../");
    QString filePath = QCoreApplication::applicationFilePath();
    filePath = "../number.txt";

    if(false == filePath.isEmpty()) //如果选择文件路径有效
    {
        qDebug()<<"选择文件有效"<<endl;
        fileName.clear();
        fileSize = 0;

        //获取文件信息
        QFileInfo info(filePath);
        fileName = info.fileName(); //获取文件名字
        fileSize = info.size(); //获取文件大小

        sendSize = 0; //发送文件的大小

        //只读方式打开文件
        //指定文件的名字
        file.setFileName(filePath);

        //打开文件
        bool isOk = file.open(QIODevice::ReadOnly);
        if(false == isOk)
        {
            qDebug() << "只读方式打开文件失败 108";
        }

        qDebug()<<" fileSize 获取文件大小"<<fileSize<<endl;
        ui->buttonFile->setEnabled(false);
        ui->buttonSend->setEnabled(true);

    }
    else
    {
        qDebug() << "选择文件路径出错 118";
    }

//------------------------------
    on_buttonSend_clicked();

}
//发送头部信息文件按钮
void MainWindow::on_buttonSend_clicked()
{
    ui->buttonSend->setEnabled(false);

    //先发送文件头信息  文件名##文件大小
    QString head = QString("%1##%2").arg(fileName).arg(fileSize);
    //发送头部信息
    qint64 len = tcpSocket->write( head.toUtf8() );
    if(len > 0)//头部信息发送成功
    {
         qDebug()<<" len得大小"<<len<<endl;
         qDebug()<<" fileSize 获取文件大小"<<fileSize<<endl;
        //发送真正的文件信息
        //防止TCP黏包
        //需要通过定时器延时 2000 ms
        timer.start(2000);


    }
    else
    {
        qDebug() << "头部信息发送失败 144";
        file.close();
        ui->buttonFile->setEnabled(true);
        ui->buttonSend->setEnabled(false);
    }
}

//发送本文数据
void MainWindow::sendData()
{
    qDebug() << "is sending...." << endl;
     qint64 len = 0;
     do
     {
        //每次发送数据的大小
        char buf[4*1024] = {0};
        len = 0;

        //往文件中读数据
        len = file.read(buf, sizeof(buf));
         qDebug()<<" 在文件中读数据的大小"<<len<<endl;
        //发送数据，读多少，发多少
        len = tcpSocket->write(buf, len);

        //发送的数据需要累积
        sendSize += len;
        qDebug()<<" 总数据"<<sendSize<<endl;

     }while(len > 0);


     //是否发送文件完毕
     if(sendSize == fileSize)
     {
         qDebug()<<" 发送的总文件大小"<<sendSize<<endl;

         file.close();

         //把客户端端口
         tcpSocket->disconnectFromHost();
         tcpSocket->close();

     }
}
