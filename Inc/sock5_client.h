#include <stdio.h>
#include <iostream>
#include "osplatformutil.h"

#ifdef I_OS_WIN

#include <WINSOCK2.H>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#endif

#ifdef I_OS_LINUX

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
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

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
using namespace std;

//定义程序中使用的常量
#define SERVER_ADDRESS "127.0.0.1" //服务器端IP地址
#define DEFAULT_PORT 6800          //服务器的端口号
#define MSGSIZE 1024               //收发缓冲区的大小

class Client
{
private:
#ifdef I_OS_WIN
    WSADATA wsaData;
#endif
    //连接所用套节字
    SOCKET sClient;
    //保存远程服务器的地址信息
    SOCKADDR_IN server;
    //收发缓冲区

    //vector<char[MSGSIZE]> RecVector;

    char szMessage[MSGSIZE];
    //成功接收字节的个数

    int port;

    /* data */
public:
    Client(const char server_address[] = SERVER_ADDRESS, int port = DEFAULT_PORT, int msgsize = MSGSIZE);
    ~Client();

    // 连接至服务器
    void connect2server();
    // 发送字符串
    void sendSTR(const char str[], int len);
    // 接收消息（阻塞式）
    char *receive();
};

#pragma clang diagnostic pop
