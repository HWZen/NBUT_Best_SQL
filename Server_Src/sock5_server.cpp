#include "osplatformutil.h"
#include "sock5_server.h"
#include "command_manager.h"
#include "socketor.h"
#include <string>
#include <string.h>

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
    if (bind(sListen, (struct sockaddr *)&local, sizeof(SOCKADDR_IN)) == -1)
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
    int iaddrSize = sizeof(SOCKADDR_IN);
    thread *RecThr;

    while (TRUE)
    {
        // 开始接受握手请求，accept()是阻断函数，等成功接受后才会继续进行程序
#ifdef I_OS_WIN
        SOCKET temp = accept(sListen, (struct sockaddr *)&client, &iaddrSize);
        socketor Client(temp, client);
#endif

#ifdef I_OS_LINUX
        socklen_t length = sizeof(client);
        socketor Client(accept(sListen, (struct sockaddr *)&client, &length), client);
#endif
        // 接受成功后：
        cout << "Accepted client:" << Client.address() << ":" << Client.port() << endl;
        Client.Send("SQL Connected\n");
        // 新建线程，在新线程中进行后续的接发通信操作
        RecThr = new thread(&Server::New_Connect, this, Client);
        RecThr->detach();
        // 建完线程后继续循环，继续接受新的握手握手请求
    }
}

// 成功握手后建立通信后，进行后续的接发操作
void Server::New_Connect(socketor sClient)
{

    // 接受从管理层返回的消息
    string result;
    int ret = 0;

    // 初始化管理层
    Manager manager(sClient);

    /**********************************************/

    // 丢失连接，结束线程
    cout << "lose connect: " << sClient.address() << ":" << sClient.port() << endl;
}

Server::~Server()
{
}