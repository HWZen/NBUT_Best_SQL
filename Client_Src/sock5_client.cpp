#include "sock5_client.h"
#include "string.h"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"

Client::Client(const char server_address[], int port, int msgsize)
{

#ifdef I_OS_WIN
    // Initialize Windows socket library
    WSAStartup(0x0202, &wsaData);
#endif

    // 创建客户端套节字
    sClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); //AF_INET指明使用TCP/IP协议族；
                                                         //SOCK_STREAM, IPPROTO_TCP具体指明使用TCP协议
    // 指明远程服务器的地址信息(端口号、IP地址等)
    memset(&server, 0, sizeof(SOCKADDR_IN));            //先将保存地址的server置为全0
    server.sin_family = PF_INET;                        //声明地址格式是TCP/IP地址格式
    server.sin_port = htons(port);                      //指明连接服务器的端口号，htons()用于 converts values between the host and network byte order
    server.sin_addr.s_addr = inet_addr(server_address); //指明连接服务器的IP地址
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
        cerr << "connect fail"<<endl;
        exit(1);
    }
}

void Client::sendSTR(const char str[],int len)
{
    send(sClient, str, len, 0);
}

char *Client::receive()
{
    int ret;
    ret = recv(sClient, szMessage, MSGSIZE, 0);
    if(ret==-1)
    {
        strncpy(szMessage, "lose connect", 13);
        ret = 13;
    }
    szMessage[ret] = '\0';
    //RecVector.push_back(szMessage);
    return szMessage;

}

#pragma clang diagnostic pop