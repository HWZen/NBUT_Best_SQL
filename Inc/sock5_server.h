#pragma once
#include <iostream>
#include <WinSock2.h>
#include <stdio.h>
#include <thread>

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
    char szMessage[MSGSIZE];
    int port;
    int threadNum;

    thread *acceptThr[MAX_THREAD];
    thread *recThr[MAX_THREAD];
    SOCKET sClient[8];

public:
    Server(int port, int MaxThreadNum);
    ~Server();

    void Listen();
    void Accept(SOCKET &sClient);
    char *Receice(SOCKET &sClient);
};

Server::Server(int port = DEFAULT_PORT, int MaxThreadNum = MAX_THREAD)
{
    WSAStartup(0x0202, &wsaData);

    sListen = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    local.sin_family = AF_INET;
    local.sin_port = htons(DEFAULT_PORT);
    local.sin_addr.s_addr = htonl(INADDR_ANY);
    
}