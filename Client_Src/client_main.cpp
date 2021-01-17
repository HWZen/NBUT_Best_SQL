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
// ��������ģʽ
string password_mode();

int main(int argc, char *argv[])
{
    // ��ȡ�����������û���������
    ReadOptions(argc, argv);

    char str[MSGSIZE];
    // if (user.length() == 0)
    //     userConfig();

    // ���ʼ��
    Client cl1(serverAddress.c_str(), port);

    // ���ӷ�����
    cout << "connecting to " << cl1.IP_Addr() << ":" << port << " ..." << endl;
    cl1.connect2server();
    cout << "connection sucessful." << endl;
    


    // �����û���
    if (user.length() != 0)
    {
        cout << cl1.receive(NULL) << endl;
        cl1.sendSTR(user.c_str(),user.length());
    }

    // ��¼����ģʽ����ͨģʽ������ģʽ�� 
    bool typing_mode;
    while(1)
    {
        
        // ���շ��ؽ�������
        string buf = cl1.receive(&typing_mode);

        if (buf == "bye." || buf == "lose connect")
            break;

        //�Ӽ�������ָ��
        if(typing_mode)// �ж�����ģʽ
            strcpy(str, password_mode().c_str());
        else
            cin.getline(str, MSGSIZE); 

        // ����ָ��
        if(strlen(str) == 0)
        {
            str[0] = ' ';
            str[1] = '\0';
        }
        cl1.sendSTR(str, strlen(str));

    }
    return 0;
}

void ReadOptions(int argc, char *argv[])
{
    port = DEFAULT_PORT;
    serverAddress = "127.0.0.1";
    user = "";

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
                exit(1);
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
                exit(1);
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
                exit(1);
            }
        }
        else if(strcmp(argv[i],"--help")==0 || strcmp(argv[i],"-h")==0)
        {
            Help();
            exit(1);
        }
        else
        {
            if(i!=0)
            {
                cout << "unknow command: " << argv[i] << endl;
                Help();
                exit(1);
            }
        }
    }
}

// void userConfig()
// {
//     cout << "user:";
//     cin >> user;
//     cout << "password:";
//
//     char c;
//     int count = 0;
//     while ((c = getch()) != '\r')
//     {
//
//         if (c == 8)
//         { // �˸�
//             if (count == 0)
//             {
//                 continue;
//             }
// #ifdef I_OS_WIN
//             putchar('\b'); // ����һ��
//             putchar(' ');  // ���һ���ո�ԭ����*����
//             putchar('\b'); // �ٻ���һ��ȴ�����
// #endif
//             count--;
//             password.pop_back();
//         }
//         if (count == 128 - 1)
//         { // ��󳤶�Ϊsize-1
//             continue;
//         }
//         if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9'))
//         {                 // ����ֻ�ɰ������ֺ���ĸ
// #ifdef I_OS_WIN
//             putchar('*'); // ���յ�һ���ַ���, ��ӡһ��*
// #endif
//             password += c;
//             count++;
//         }
//     }
//     password[count] = '\0';
//     cout<<endl;
// }

void Help()
{
    cout << "commant:" << endl;
    cout << "-u --uesr: user" << endl
         << "-p --port: port" << endl
         << "-a --address: address" << endl;
}

string password_mode()
{
    char c;
    int count = 0;
    string re_buf;
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
            re_buf.pop_back();
        }
        if (count == 128 - 1)
        { // ��󳤶�Ϊsize-1
            continue;
        }
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9'))
        { // ����ֻ�ɰ������ֺ���ĸ
#ifdef I_OS_WIN
            putchar('*'); // ���յ�һ���ַ���, ��ӡһ��*
#endif
            re_buf += c;
            count++;
        }
    }
#ifdef I_OS_WIN
    char temp[10];
    cin.getline(temp, MSGSIZE);
#endif
    return re_buf;
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