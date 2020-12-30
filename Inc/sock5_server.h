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

// Windows��
#ifdef I_OS_WIN

#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")

#endif

// Linux��
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

    // ����SOCKET
    SOCKET sListen;
    // ��¼������ַ��Ϣ
    SOCKADDR_IN local;
    // ��¼�˿�
    int port;

public:
    Server(int Port = DEFAULT_PORT);
    ~Server();

    // ������������
    void Listen();

    // �������Ӳ������߳̽��պ���
    [[noreturn]] void Accept();

    // ���պ����������ӹ���ϵͳ
    void Receice(SOCKET sClient, SOCKADDR_IN Cli_Info);

    // ���ͺ���
    void Send(SOCKET &sClient, const char *str);
};

#endif