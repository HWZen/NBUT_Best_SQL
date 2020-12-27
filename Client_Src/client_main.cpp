#include "osplatformutil.h"
#include <iostream>
#include <string>
#include <stdio.h>
#ifdef I_OS_WIN
#include <conio.h>
#endif
#ifdef I_OS_LINUX
#include <string.h>
#include <termio.h>
#endif
#include "sock5_client.h"
#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
using namespace std;

//���������ʹ�õĳ���
string user;
string password;
int port;
string serverAddress;

#ifdef I_OS_LINUX
int getch(void);
#endif

// ��ȡ����������
void ReadOptions(int argc, char *argv[]);
// ������Ϣ
void Help();
// �û���Ϣ
void userConfig();

int main(int argc, char *argv[])
{
    // ��ȡ�����������û���������
    ReadOptions(argc, argv);
    char str[MSGSIZE];
    if (user.length() == 0)
        userConfig();

    // ���ʼ��
    Client cl1(serverAddress.c_str(), port);

    // ���ӷ�����
    cout << "connecting to " << cl1.IP_Addr() << ":" << port << " ..." << endl;
    cl1.connect2server();
    cout << "connection sucessful." << endl;
    cout << cl1.receive() << endl;

    // �����û���������
    cl1.sendSTR(user.c_str(),user.length());
    cl1.receive();
    cl1.sendSTR(password.c_str(), password.length());
    cout << cl1.receive();

#ifdef I_OS_WIN
    cin.getline(str, MSGSIZE);
#endif


    while(1)
    {
        //�Ӽ�������ָ��
        cin.getline(str, MSGSIZE); //The function reads characters from stdin and loads them into szMessage

        // ����ָ��
        if(strlen(str) == 0)
            continue;
        cl1.sendSTR(str, strlen(str));

        // ���շ��ؽ�������
        char *t=cl1.receive();

        cout << t;

        if (strcmp(t, "bye.\n") == 0 || strcmp(t, "lose connect")==0)
            break;
    }
    return 0;
}

void ReadOptions(int argc, char *argv[])
{
    port = DEFAULT_PORT;
    serverAddress = "127.0.0.1";
    user = "";
    password = "";

    for (int i = 0; i < argc;i++)
    {
        if (strcmp(argv[i], "-u") == 0 || strcmp(argv[i], "--user") == 0)
        {
            if(i!=argc-1)
                user = argv[++i];
            else
            {
                cout << "Missing parameters: user" << endl;
                Help();
                exit(0);
            }
        }
        else if (strcmp(argv[i], "-p") == 0 || strcmp(argv[i], "--port") == 0)
        {
            if (i != argc - 1)
                port = atoi(argv[++i]);
            else
            {
                cout << "Missing parameters: port" << endl;
                Help();
                exit(0);
            }
        }
        else if (strcmp(argv[i], "-pw") == 0 || strcmp(argv[i], "--password") == 0)
        {
            if (i != argc - 1)
                password = argv[++i];
            else
            {
                cout << "Missing parameters: password" << endl;
                Help();
                exit(0);
            }
        }
        else if (strcmp(argv[i], "-a") == 0 || strcmp(argv[i], "--address") == 0)
        {
            if (i != argc - 1)
                serverAddress = argv[++i];
            else
            {
                cout << "Missing parameters: address" << endl;
                Help();
                exit(0);
            }
        }
        else if(strcmp(argv[i],"--help")==0 || strcmp(argv[i],"-h")==0)
        {
            Help();
            exit(0);
        }
        else
        {
            if(i!=0)
            {
                cout << "unknow command: " << argv[i] << endl;
                Help();
                exit(0);
            }
        }
    }
}

void userConfig()
{
    cout << "user:";
    cin >> user;
    cout << "password:";

    char c;
    int count = 0;
    while ((c = getch()) != '\r')
    {

        if (c == 8)
        { // �˸�
            if (count == 0)
            {
                continue;
            }
#ifdef I_OS_WIN
            putchar('\b'); // ����һ��
            putchar(' ');  // ���һ���ո�ԭ����*����
            putchar('\b'); // �ٻ���һ��ȴ�����
#endif
            count--;
            password.pop_back();
        }
        if (count == 128 - 1)
        { // ��󳤶�Ϊsize-1
            continue;
        }
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9'))
        {                 // ����ֻ�ɰ������ֺ���ĸ
#ifdef I_OS_WIN
            putchar('*'); // ���յ�һ���ַ���, ��ӡһ��*
#endif
            password += c;
            count++;
        }
    }
    password[count] = '\0';
    cout<<endl;
}

void Help()
{
    cout << "commant:" << endl;
    cout << "-u --uesr: user" << endl
         << "-p --port: port" << endl
         << "-pw --password: password" << endl
         << "-a --address: address" << endl;
}

#ifdef I_OS_LINUX

int getch(void)
{
    struct termios tm, tm_old;
    int fd = 0, ch;

    if (tcgetattr(fd, &tm) < 0) {//�������ڵ��ն�����
        return -1;
    }

    tm_old = tm;
    cfmakeraw(&tm);//�����ն�����Ϊԭʼģʽ����ģʽ�����е������������ֽ�Ϊ��λ������
    if (tcsetattr(fd, TCSANOW, &tm) < 0) {//�����ϸ���֮�������
        return -1;
    }

    ch = getchar();
    if (tcsetattr(fd, TCSANOW, &tm_old) < 0) {//��������Ϊ���������
        return -1;
    }

    return ch;
}

#endif

#pragma clang diagnostic pop