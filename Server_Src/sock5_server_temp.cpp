#include "osplatformutil.h"

#ifdef I_OS_WIN
#include <iostream>
#include <stdio.h>
#include <thread>

#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")


#include <string>
#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
using namespace std;

#define DEFAULT_PORT 6800
#define MSGSIZE 1024
#define MAX_THREAD 8

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

#endif

#ifdef I_OS_LINUX
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <iostream>
#include <thread>
#include <list>

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"

#define PORT 6800
#define IP "127.0.0.1"

int s;
struct sockaddr_in servaddr;
socklen_t len;
std::list<int> li; //用list来存放套接字，没有限制套接字的容量就可以实现一个server跟若干个client通信

void getConn()
{
    while (1)
    {
        int conn = accept(s, (struct sockaddr *)&servaddr, &len);
        li.push_back(conn);
        printf("%d\n", conn);
    }
}

void getData()
{
    struct timeval tv;
    tv.tv_sec = 100; //设置倒计时时间
    tv.tv_usec = 0;
    while (1)
    {
        std::list<int>::iterator it;
        for (it = li.begin(); it != li.end(); ++it)
        {
            fd_set rfds;
            FD_ZERO(&rfds);
            int maxfd = 0;
            int retval = 0;
            FD_SET(*it, &rfds);
            if (maxfd < *it)
            {
                maxfd = *it;
            }
            retval = select(maxfd + 1, &rfds, NULL, NULL, &tv);
            if (retval == -1)
            {
                printf("select error\n");
            }
            else if (retval == 0)
            {
                //printf("not message\n");
            }
            else
            {
                char buf[1024];
                memset(buf, 0, sizeof(buf));
                int len = recv(*it, buf, sizeof(buf), 0);
                printf("%s", buf);
            }
        }
        sleep(1);
    }
}

void sendMess()
{
    while (1)
    {
        char buf[1024];
        fgets(buf, sizeof(buf), stdin);
        //printf("you are send %s", buf);
        std::list<int>::iterator it;
        for (it = li.begin(); it != li.end(); ++it)
        {
            send(*it, buf, sizeof(buf), 0);
        }
    }
}

int main()
{
    //new socket
    s = socket(AF_INET, SOCK_STREAM, 0);
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = inet_addr(IP);
    if (bind(s, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1)
    {
        perror("bind");
        exit(1);
    }
    if (listen(s, 20) == -1)
    {
        perror("listen");
        exit(1);
    }
    len = sizeof(servaddr);

    //thread : while ==>> accpet
    std::thread t(getConn);
    t.detach(); //detach的话后面的线程不同等前面的进程完成后才能进行，如果这里改为join则前面的线程无法判断结束，就会
    //一直等待，导致后面的线程无法进行就无法实现操作
    //printf("done\n");
    //thread : input ==>> send
    std::thread t1(sendMess);
    t1.detach();
    //thread : recv ==>> show
    std::thread t2(getData);
    t2.detach();
    while (1) //做一个死循环使得主线程不会提前退出
    {
    }
    return 0;
}

#pragma clang diagnostic pop
/*这个跟前面的不一样的地方是，把获得连接套接字getConn和发送信息sendMess和接收信息getData放在三个函数中，创建
的三个线程分别对应处理三个函数，就可以使得server能跟若干个client通信*/
#endif