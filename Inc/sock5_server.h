#ifndef SOCK5_SERVER
#define SOCK5_SERVER
#include "osplatformutil.h"
#ifdef I_OS_WIN
#include <iostream>
#include <WinSock2.h>
#include <stdio.h>
#include <thread>
#include <vector>

#include <string>
using namespace std;
#pragma comment(lib, "ws2_32.lib")

#define DEFAULT_PORT 5150
#define MSGSIZE 1024
#define MAX_THREAD 8

class Server
{
private:
    WSADATA wsaData;
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
#ifdef I_OS_LINUX


#endif

#endif