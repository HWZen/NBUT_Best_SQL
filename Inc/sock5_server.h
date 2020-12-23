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
#define MAX_THREAD 8

#ifdef I_OS_WIN

#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")

#endif

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

    SOCKET sListen;
    SOCKADDR_IN local;
    int port;
    int threadNum;

    thread *acceptThr[MAX_THREAD];
    thread *recThr[MAX_THREAD];

    vector<SOCKET> sClient_vec;
    SOCKET sClient[MAX_THREAD];
    bool Thr_connected[MAX_THREAD] = {false};
    int next_Thr();

public:
    Server(int Port = DEFAULT_PORT);
    ~Server();

    void Listen();

    [[noreturn]] void Accept();
    void Receice(SOCKET sClient, SOCKADDR_IN Cli_Info);
    void Send(SOCKET &sClient, const char *str);
};

#endif