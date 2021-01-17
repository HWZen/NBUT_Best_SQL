#include "sock5_client.h"
#include "string.h"
#include <string>
using namespace std;

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"

Client::Client(const char server_address[], int port, int msgsize)
{

    Real_Addr = server_address;
#ifdef I_OS_WIN
    // Initialize Windows socket library
    WSAStartup(0x0202, &wsaData);
    WORD wVersionRequested = 0;
    int err = 0;

    wVersionRequested = MAKEWORD(2, 2);

    err = WSAStartup(wVersionRequested, &wsaData);

    if (err != 0)
        exit(0);

    if (LOBYTE(wsaData.wVersion) != 2 ||
        HIBYTE(wsaData.wVersion) != 2)
    {

        WSACleanup();
        exit(0);
    }
    char **pptr = NULL;
    HOSTENT *pHostEntry = gethostbyname(server_address);
    char szIpBuff[32] = {0};
    if (NULL != pHostEntry && server_address[0] != '\0')
    {

        //将主机地址列表输出，可含多个
        int i = 0;
        for (i = 1, pptr = pHostEntry->h_addr_list; *pptr != NULL; ++pptr)
        {
            memset(szIpBuff, 0, sizeof(szIpBuff));
            //inet_ntop的返回值为NULL，则表示失败，否则返回相应的IP地址（此时szIpRet指向的是szIpBuff）
            const char *szIpRet = inet_ntop(pHostEntry->h_addrtype, *pptr, szIpBuff, sizeof(szIpBuff));
            if(szIpRet!=NULL)
                Real_Addr = szIpBuff;
        }
        
    }
    else
    {
        cerr << "域名解析失败！" << endl;
        exit(0);
    }

#endif

#ifdef I_OS_LINUX

    char **pptr;
    struct hostent *hptr;
    char str[32];
    if ((hptr = gethostbyname(server_address)) == NULL)
    {
        printf("  gethostbyname error for host:%s\n ", server_address);
        exit(0);
    }
    switch (hptr->h_addrtype)
    {
    case AF_INET:
    case AF_INET6:
        pptr = hptr->h_addr_list;
        Real_Addr = inet_ntop(hptr->h_addrtype, hptr->h_addr, str, sizeof(str));
        break;
    default:
        printf(" unknown address type\n ");
        exit(0);
        break;
    }

#endif

        // 创建客户端套节字
    sClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); //AF_INET指明使用TCP/IP协议族；
                                                             //SOCK_STREAM, IPPROTO_TCP具体指明使用TCP协议
    // 指明远程服务器的地址信息(端口号、IP地址等)
    memset(&server, 0, sizeof(SOCKADDR_IN));            //先将保存地址的server置为全0
    server.sin_family = PF_INET;                        //声明地址格式是TCP/IP地址格式
    server.sin_port = htons(port);                      //指明连接服务器的端口号，htons()用于 converts values between the host and network byte order
    server.sin_addr.s_addr = inet_addr(Real_Addr.c_str());       //指明连接服务器的IP地址
                                                        //结构SOCKADDR_IN的sin_addr字段用于保存IP地址，sin_addr字段也是一个结构体，sin_addr.s_addr用于最终保存IP地址
                                                        //inet_addr()用于将 形如的"127.0.0.1"字符串转换为IP地址格式
}

Client::~Client()
{
    // 解除连接
    closesocket(sClient);

#ifdef I_OS_WIN
    WSACleanup();
#endif

}

void Client::connect2server()
{
    if(connect(sClient, (struct sockaddr *)&server, sizeof(SOCKADDR_IN))<0)
    {
        cerr << "connect fail."<< endl;
        exit(1);
    }
}

void Client::sendSTR(const char str[],int len)
{
    if(len > 0)
        send(sClient, str, len, 0);
}

string Client::receive(bool *Is_pw_end)
{
    int ret;
    string buf;
    do
    {
        ret = recv(sClient, szMessage, MSGSIZE, 0);
        if(ret==-1)
        {
            strncpy(szMessage, "lose connect", 13);
            ret = 13;
        }
        szMessage[ret] = '\0';

        if (strcmp(szMessage, "over") && strcmp(szMessage, "pw mode"))
            buf += szMessage;

    } while (strcmp(szMessage, "over") && strcmp(szMessage, "pw mode"));

    *Is_pw_end = strcmp(szMessage, "pw mode") ? false : true;

    return buf;
}

string Client::IP_Addr()
{
    return Real_Addr;
}

#pragma clang diagnostic pop