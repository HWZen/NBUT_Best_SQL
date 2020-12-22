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

    // �����ͻ����׽���
    sClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); //AF_INETָ��ʹ��TCP/IPЭ���壻
                                                         //SOCK_STREAM, IPPROTO_TCP����ָ��ʹ��TCPЭ��
    // ָ��Զ�̷������ĵ�ַ��Ϣ(�˿ںš�IP��ַ��)
    memset(&server, 0, sizeof(SOCKADDR_IN));            //�Ƚ������ַ��server��Ϊȫ0
    server.sin_family = PF_INET;                        //������ַ��ʽ��TCP/IP��ַ��ʽ
    server.sin_port = htons(port);                      //ָ�����ӷ������Ķ˿ںţ�htons()���� converts values between the host and network byte order
    server.sin_addr.s_addr = inet_addr(server_address); //ָ�����ӷ�������IP��ַ
                                                        //�ṹSOCKADDR_IN��sin_addr�ֶ����ڱ���IP��ַ��sin_addr�ֶ�Ҳ��һ���ṹ�壬sin_addr.s_addr�������ձ���IP��ַ
                                                        //inet_addr()���ڽ� �����"127.0.0.1"�ַ���ת��ΪIP��ַ��ʽ
}

Client::~Client()
{
    // �������
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