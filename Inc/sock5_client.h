#include <stdio.h>
#include <iostream>
#include <vector>
#include "osplatformutil.h"

#ifdef I_OS_WIN

#include <WINSOCK2.H>
#pragma comment(lib, "ws2_32.lib")

#endif

#ifdef I_OS_LINUX

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/shm.h>
typedef int SOCKET;
typedef struct sockaddr_in SOCKADDR_IN;
#define closesocket(x) close(x)

#endif

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
using namespace std;

//���������ʹ�õĳ���
#define SERVER_ADDRESS "127.0.0.1" //��������IP��ַ
#define DEFAULT_PORT 6800                  //�������Ķ˿ں�
#define MSGSIZE 1024               //�շ��������Ĵ�С

class Client
{
private:

#ifdef I_OS_WIN
    WSADATA wsaData;
#endif
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
