#ifndef SOCK5_SERVER
#define SOCK5_SERVER
#include "osplatformutil.h"
#include <iostream>
#include <stdio.h>
#include <thread>
#include <vector>
#include <string>
using namespace std;

#define DEFAULT_PORT 5150
#define MSGSIZE 1024

// Windows库
#ifdef I_OS_WIN

#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")

#endif

// Linux库
#ifdef I_OS_LINUX

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/shm.h>
typedef int SOCKET;
typedef struct sockaddr_in SOCKADDR_IN;
#define closesocket(x) close(x)

#define TRUE 1

#endif

class Server
{
private:
#ifdef I_OS_WIN
    WSADATA wsaData;
#endif

    // 监听SOCKET
    SOCKET sListen;
    // 记录本机地址信息
    SOCKADDR_IN local;
    // 记录端口
    int port;

public:
    Server(int Port = DEFAULT_PORT);
    ~Server();

    // 开启监听函数
    void Listen();

    // 受理连接并开多线程接收函数
    [[noreturn]] void Accept();

    // 接收函数，并连接管理系统
    void Receice(SOCKET sClient, SOCKADDR_IN Cli_Info);

    // 发送函数
    void Send(SOCKET &sClient, const char *str);
};

#endif