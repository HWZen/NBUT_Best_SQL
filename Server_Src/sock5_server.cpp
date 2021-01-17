#include "osplatformutil.h"
#include "sock5_server.h"
#include "command_manager.h"
#include <string>
#include <string.h>

#ifdef I_OS_WIN

#include <windows.h>

#endif

#ifdef I_OS_LINUX

#include <time.h>
void Sleep(int ms)
{
    struct timeval delay;
    delay.tv_sec = 0;
    delay.tv_usec = ms * 1000; // 20 ms
    select(0, NULL, NULL, NULL, &delay);
}

#endif

using namespace std;
using namespace SQL;

// 构造函数，主要是根据port初始化本机地址信息
Server::Server(int Port)
{
    port = Port;
#ifdef I_OS_WIN
    WSAStartup(0x0202, &wsaData);
#endif
    sListen = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    local.sin_family = AF_INET;
    local.sin_port = htons(port);
    local.sin_addr.s_addr = htonl(INADDR_ANY);
}

// 监听初始化
void Server::Listen()
{
    if (bind(sListen, (struct sockaddr *) &local, sizeof(SOCKADDR_IN)) == -1)
    {
        cerr << "bind fail: port " << port << endl;
        exit(0);
    }

    if (listen(sListen, 20) == -1)
    {
        cerr << "listen fail: port " << port << endl;
        exit(0);
    }

    cout << "listrn sucess port: " << port << endl;

    // 监听成功后开始接受握手
    Accept();
}

[[noreturn]] void Server::Accept()
{
    SOCKADDR_IN client;
    SOCKET Client;
    int iaddrSize = sizeof(SOCKADDR_IN);
    thread *RecThr;

    while (TRUE)
    {
        // 开始接受握手请求，accept()是阻断函数，等成功接受后才会继续进行程序
#ifdef I_OS_WIN
        Client = accept(sListen, (struct sockaddr *)&client, &iaddrSize);
#endif

#ifdef I_OS_LINUX
        socklen_t length = sizeof(client);
        Client = accept(sListen, (struct sockaddr *) &client, &length);
#endif
        // 接受成功后：
        printf("Accepted client:%s:%d\n", inet_ntoa(client.sin_addr),
               ntohs(client.sin_port));
        Send(Client, "SQL Connected\n");
        // 新建线程，在新线程中进行后续的接发通信操作
        RecThr = new thread(&Server::Receice, this, Client, client);
        RecThr->detach();
        // 建完线程后继续循环，继续接受新的握手握手请求
    }
}

// 成功握手后建立通信后，进行后续的接发操作
void Server::Receice(SOCKET sClient, SOCKADDR_IN Cli_Info)
{
    // 接受消息缓冲区
    char szMessage[MSGSIZE];

    // 接受从管理层返回的消息
    string result;
    int ret = 0;

    // 用于储存用户输入的账户密码（动态分配内存，用完后立马释放，避免被内存监视病毒恶意读取密码）
    char *user = new char[16];
    char *password = new char[32];

/**************************/
/**  开始接收账户密码信息  **/
    // 接收消息函数（C库提供）
    Send(sClient, "User: ");
    Sleep(300);
    Send(sClient, "over");
    ret = recv(sClient, user, MSGSIZE, 0);
    if (ret >= 16)
        ret = 15;
    user[ret] = '\0';
    Send(sClient, "Password: ");
    Sleep(300);
    Send(sClient, "pw mode");

    ret = recv(sClient, password, MSGSIZE, 0);
    if (ret >= 32)
        ret = 31;
    password[ret] = '\0';
/**  接收完毕  **/
/***************/

    // 初始化管理层
    Manager manager(user, password, result);
    delete[] user;
    delete[] password;

    // 发送消息给客户端
    Send(sClient, result.c_str());

    // 接收消息
    ret = recv(sClient, szMessage, MSGSIZE, 0);
    while (ret > 0) // ret==0 代表连接丢失，跳出循环，否则继续接收
    {
        szMessage[ret] = '\0';

        // 将消息传给管理层
        result = manager.command(szMessage);
        // 将返回的内容发送给客户端
        Send(sClient, result.c_str());

        // 暂时的做法，以后重构的时候再详补通信逻辑。
        Send(sClient, "over");
        if (result == "bye.")
            break;
        // 接收消息
        ret = recv(sClient, szMessage, MSGSIZE, 0);
        if (ret == 0)
            continue;
    }
    /*  上面的while 可以改为do...while，结构更清晰  */
    /**********************************************/

    // 丢失连接，结束线程
    cout << "lose connect: " << inet_ntoa(Cli_Info.sin_addr) << ":" << ntohs(Cli_Info.sin_port) << endl;
    closesocket(sClient);
    return;
}

// 发送函数
void Server::Send(SOCKET &sClient, const char *str)
{
    send(sClient, str, strlen(str), 0);
}

Server::~Server()
{
}