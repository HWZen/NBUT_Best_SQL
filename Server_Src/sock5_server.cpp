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

// ���캯������Ҫ�Ǹ���port��ʼ��������ַ��Ϣ
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

// ������ʼ��
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

    // �����ɹ���ʼ��������
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
        // ��ʼ������������accept()����Ϻ������ȳɹ����ܺ�Ż�������г���
#ifdef I_OS_WIN
        Client = accept(sListen, (struct sockaddr *)&client, &iaddrSize);
#endif

#ifdef I_OS_LINUX
        socklen_t length = sizeof(client);
        Client = accept(sListen, (struct sockaddr *) &client, &length);
#endif
        // ���ܳɹ���
        printf("Accepted client:%s:%d\n", inet_ntoa(client.sin_addr),
               ntohs(client.sin_port));
        Send(Client, "SQL Connected\n");
        // �½��̣߳������߳��н��к����Ľӷ�ͨ�Ų���
        RecThr = new thread(&Server::Receice, this, Client, client);
        RecThr->detach();
        // �����̺߳����ѭ�������������µ�������������
    }
}

// �ɹ����ֺ���ͨ�ź󣬽��к����Ľӷ�����
void Server::Receice(SOCKET sClient, SOCKADDR_IN Cli_Info)
{
    // ������Ϣ������
    char szMessage[MSGSIZE];

    // ���ܴӹ���㷵�ص���Ϣ
    string result;
    int ret = 0;

    // ���ڴ����û�������˻����루��̬�����ڴ棬����������ͷţ����ⱻ�ڴ���Ӳ��������ȡ���룩
    char *user = new char[16];
    char *password = new char[32];

/**************************/
/**  ��ʼ�����˻�������Ϣ  **/
    // ������Ϣ������C���ṩ��
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
/**  �������  **/
/***************/

    // ��ʼ�������
    Manager manager(user, password, result);
    delete[] user;
    delete[] password;

    // ������Ϣ���ͻ���
    Send(sClient, result.c_str());

    // ������Ϣ
    ret = recv(sClient, szMessage, MSGSIZE, 0);
    while (ret > 0) // ret==0 �������Ӷ�ʧ������ѭ���������������
    {
        szMessage[ret] = '\0';

        // ����Ϣ���������
        result = manager.command(szMessage);
        // �����ص����ݷ��͸��ͻ���
        Send(sClient, result.c_str());

        // ��ʱ���������Ժ��ع���ʱ�����겹ͨ���߼���
        Send(sClient, "over");
        if (result == "bye.")
            break;
        // ������Ϣ
        ret = recv(sClient, szMessage, MSGSIZE, 0);
        if (ret == 0)
            continue;
    }
    /*  �����while ���Ը�Ϊdo...while���ṹ������  */
    /**********************************************/

    // ��ʧ���ӣ������߳�
    cout << "lose connect: " << inet_ntoa(Cli_Info.sin_addr) << ":" << ntohs(Cli_Info.sin_port) << endl;
    closesocket(sClient);
    return;
}

// ���ͺ���
void Server::Send(SOCKET &sClient, const char *str)
{
    send(sClient, str, strlen(str), 0);
}

Server::~Server()
{
}