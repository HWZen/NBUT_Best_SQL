#include "osplatformutil.h"
#include "sock5_server.h"
#include "command_manager.h"
#include <string>
#include <string.h>

using namespace std;
using namespace SQL;

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

int Server::next_Thr()
{
    for (int i = 0; i < MAX_THREAD; i++)
    {
        if (Thr_connected[i])
            return i;
    }
    return -1;
}

void Server::Listen()
{
    if (bind(sListen, (struct sockaddr *)&local, sizeof(SOCKADDR_IN)) == -1)
    {
        cerr << "bind fail: port " << port << endl;
        this->~Server();
    }

    if (listen(sListen, MAX_THREAD) == -1)
    {
        cerr << "listen fail: port " << port << endl;
        this->~Server();
    }

    cout << "listrn sucess port: " << port << endl;
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
#ifdef I_OS_WIN
        Client = accept(sListen, (struct sockaddr *)&client, &iaddrSize);
#endif

#ifdef I_OS_LINUX
        socklen_t length = sizeof(client);
        Client = accept(sListen, (struct sockaddr *)&client, &length);
#endif

        printf("Accepted client:%s:%d\n", inet_ntoa(client.sin_addr),
               ntohs(client.sin_port));
        Send(Client, "SQL Connected");
        RecThr = new thread(&Server::Receice, this, Client, client);
        RecThr->detach();
    }
}

void Server::Receice(SOCKET sClient, SOCKADDR_IN Cli_Info)
{
    char szMessage[MSGSIZE];
    string result;
    int ret = 0;

    char *user = new char[16];
    char *password = new char[32];

    ret = recv(sClient, user, MSGSIZE, 0);
    if (ret >= 16)
        ret = 15;
    user[ret] = '\0';
    Send(sClient, "Received_User");

    ret = recv(sClient, password, MSGSIZE, 0);
    if (ret >= 32)
        ret = 31;
    password[ret] = '\0';

    string rec_buf;
    Manager manager(user, password, rec_buf);

    Send(sClient, rec_buf.c_str());
    /*
    判断账号密码正确
    

    */
    ret = recv(sClient, szMessage, MSGSIZE, 0);
    while (ret > 0)
    {
        szMessage[ret] = '\0';
        result = manager.command(szMessage);
        Send(sClient, result.c_str());
        if (result == "bye.")
            break;

        ret = recv(sClient, szMessage, MSGSIZE, 0);
        if (ret == 0)
            continue;
    }
    cout << "lose connect: " << inet_ntoa(Cli_Info.sin_addr) << ":" << ntohs(Cli_Info.sin_port) << endl;
    closesocket(sClient);
    return;
}

void Server::Send(SOCKET &sClient, const char *str)
{
    send(sClient, str, strlen(str), 0);
}

Server::~Server()
{
}