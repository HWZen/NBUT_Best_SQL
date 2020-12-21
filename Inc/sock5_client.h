#include <stdio.h>
#include <iostream>
#include <vector>
#include <thread>
#include "osplatformutil.h"

#ifdef I_OS_WIN
#include <WINSOCK2.H>
#endif

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
using namespace std;

//���������ʹ�õĳ���
#define SERVER_ADDRESS "127.0.0.1" //��������IP��ַ
#define DEFAULT_PORT 6800                  //�������Ķ˿ں�
#define MSGSIZE 1024               //�շ��������Ĵ�С
#pragma comment(lib, "ws2_32.lib")

class Client
{
private:
    WSADATA wsaData;
    //���������׽���
    SOCKET sClient;
    //����Զ�̷������ĵ�ַ��Ϣ
    SOCKADDR_IN server;
    //�շ�������

    //vector<char[MSGSIZE]> RecVector;

    char szMessage[MSGSIZE];
    //�ɹ������ֽڵĸ���
    
    int port;

    /* data */
public:
    Client(const char server_address[] = SERVER_ADDRESS, int port = DEFAULT_PORT, int msgsize = MSGSIZE);
    ~Client();

    // ������������
    void connect2server();
    // �����ַ���
    void sendSTR(const char str[], int len);
    // ������Ϣ������ʽ��
    char *receive();

};

#pragma clang diagnostic pop
