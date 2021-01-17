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

        //��������ַ�б�������ɺ����
        int i = 0;
        for (i = 1, pptr = pHostEntry->h_addr_list; *pptr != NULL; ++pptr)
        {
            memset(szIpBuff, 0, sizeof(szIpBuff));
            //inet_ntop�ķ���ֵΪNULL�����ʾʧ�ܣ����򷵻���Ӧ��IP��ַ����ʱszIpRetָ�����szIpBuff��
            const char *szIpRet = inet_ntop(pHostEntry->h_addrtype, *pptr, szIpBuff, sizeof(szIpBuff));
            if(szIpRet!=NULL)
                Real_Addr = szIpBuff;
        }
        
    }
    else
    {
        cerr << "��������ʧ�ܣ�" << endl;
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

        // �����ͻ����׽���
    sClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); //AF_INETָ��ʹ��TCP/IPЭ���壻
                                                             //SOCK_STREAM, IPPROTO_TCP����ָ��ʹ��TCPЭ��
    // ָ��Զ�̷������ĵ�ַ��Ϣ(�˿ںš�IP��ַ��)
    memset(&server, 0, sizeof(SOCKADDR_IN));            //�Ƚ������ַ��server��Ϊȫ0
    server.sin_family = PF_INET;                        //������ַ��ʽ��TCP/IP��ַ��ʽ
    server.sin_port = htons(port);                      //ָ�����ӷ������Ķ˿ںţ�htons()���� converts values between the host and network byte order
    server.sin_addr.s_addr = inet_addr(Real_Addr.c_str());       //ָ�����ӷ�������IP��ַ
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