#include "tcpserver.h"

TcpServer::TcpServer(QObject *parent) : QObject(parent)
{
    mServer = new QTcpServer(this);
    connect(mServer, SIGNAL(newConnection()), this, SLOT(newConnection()));
}

void TcpServer::start(int port)
{
    //如果没有监听就尝试开启监听
    if (!mServer->isListening()) {
        if (mServer->listen(QHostAddress::AnyIPv4, port)) {
            qDebug() << "服务端监听成功";
        } else {
            qDebug() << "服务端监听失败：" << mServer->errorString();
        }
    }
}

void TcpServer::stop()
{
    if (mServer->isListening()) {
        mServer->close();
    }
    else
    {
        qDebug() <<"根本就没监听啊";
    }
}

void TcpServer::newConnection()
{
    //bool hasPendingConnections()const 如果服务端有一个待处理的连接,就返回真,否则返回假
    while (mServer->hasPendingConnections()) {
        // 获取新连接
        QTcpSocket *sock = mServer->nextPendingConnection();
        // 发射新连接信号让调用服务器的类知道
        emit newConnection(sock);
    }
}
