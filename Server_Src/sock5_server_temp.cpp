#include <iostream>
#include <WinSock2.h>
#include <stdio.h>
#include <thread>

#include <string>
#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
using namespace std;

#define DEFAULT_PORT 6800
#define MSGSIZE 1024
#define MAX_THREAD 8

#pragma comment(lib, "ws2_32.lib")

WSADATA wsaData;
SOCKET sListen;
SOCKADDR_IN local;
SOCKADDR_IN client;

void Accept(SOCKET &sClient);
void Recevie(SOCKET &sClient);
int pth();

int main()
{
    
    
    
    WSAStartup(0x0202, &wsaData);

    sListen = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    local.sin_family = AF_INET;
    local.sin_port = htons(DEFAULT_PORT);
    local.sin_addr.s_addr = htonl(INADDR_ANY);
    bind(sListen, (struct sockaddr *)&local, sizeof(SOCKADDR_IN));

    printf("listening. port:%d \n", DEFAULT_PORT);
    listen(sListen, 10);
    thread *acceptThr[MAX_THREAD];
    thread *recThr[MAX_THREAD];
    SOCKET sClient[8];

    int cnt = 0;
    acceptThr[cnt] = new thread(Accept,ref(sClient[cnt]));
    acceptThr[cnt]->join();
    recThr[cnt] = new thread(Recevie, ref(sClient[cnt]));
    recThr[cnt]->detach();
    while(cnt++<8)
    {
        acceptThr[cnt] = new thread(Accept, ref(sClient[cnt]));
        acceptThr[cnt]->join();
        recThr[cnt] = new thread(Recevie, ref(sClient[cnt]));
        recThr[cnt]->detach();
    }

    return 0;
}

void Accept(SOCKET &sClient)
{

    char szMessage[MSGSIZE];

    int iaddrSize = sizeof(SOCKADDR_IN);

    sClient = accept(sListen, (struct sockaddr *)&client, &iaddrSize);

    printf("Accepted client:%s:%d\n", inet_ntoa(client.sin_addr),
           ntohs(client.sin_port));

    sprintf(szMessage, "SQL running.");
    send(sClient, szMessage, strlen(szMessage), 0);

    
}

void Recevie(SOCKET &sClient)
{
    char szMessage[MSGSIZE];
    int ret;
    while (TRUE)
    {
        ret = recv(sClient, szMessage, MSGSIZE, 0);
        if(ret<0)
        {
            cout<<"lose connect."<<endl;
            break;
        }
        szMessage[ret] = '\0';
        printf("Received [%d bytes]: '%s'\n", ret, szMessage);
        send(sClient, "Received.", 10, 0);
    }
    // 释放连接和进行结束工作
    closesocket(sClient);
    return;
}

#pragma clang diagnostic pop