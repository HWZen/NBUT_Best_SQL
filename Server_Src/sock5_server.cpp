#include "osplatformutil.h"
#include "sock5_server.h"
#include "command_manager.h"
#include "socketor.h"
#include <string>
#include <string.h>

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

    // �����ɹ���ʼ��������
    Accept();
}

[[noreturn]] void Server::Accept()
{
    SOCKADDR_IN client;
    int iaddrSize = sizeof(SOCKADDR_IN);
    thread *RecThr;

    while (TRUE)
    {
        // ��ʼ������������accept()����Ϻ������ȳɹ����ܺ�Ż�������г���
#ifdef I_OS_WIN
        SOCKET temp = accept(sListen, (struct sockaddr *)&client, &iaddrSize);
        socketor Client(temp, client);
#endif

#ifdef I_OS_LINUX
        socklen_t length = sizeof(client);
        socketor Client(accept(sListen, (struct sockaddr *)&client, &length), client);
#endif
        // ���ܳɹ���
        cout << "Accepted client:" << Client.address() << ":" << Client.port() << endl;
        Client.Send("SQL Connected\n");
        // �½��̣߳������߳��н��к����Ľӷ�ͨ�Ų���
        RecThr = new thread(&Server::New_Connect, this, Client);
        RecThr->detach();
        // �����̺߳����ѭ�������������µ�������������
    }
}

// �ɹ����ֺ���ͨ�ź󣬽��к����Ľӷ�����
void Server::New_Connect(socketor sClient)
{

    // ���ܴӹ���㷵�ص���Ϣ
    string result;
    int ret = 0;

    // ��ʼ�������
    Manager manager(sClient);

    /**********************************************/

    // ��ʧ���ӣ������߳�
    cout << "lose connect: " << sClient.address() << ":" << sClient.port() << endl;
}

Server::~Server()
{
}