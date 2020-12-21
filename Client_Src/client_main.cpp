#include <iostream>
#include <string>
#include <stdio.h>
#include <conio.h>
#include "sock5_client.h"
#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
using namespace std;

//定义程序中使用的常量
string user;
string password;
int port;
string serverAddress;

// 读取处理程序参数
void ReadOptions(int argc, char *argv[]);
// 帮助信息
void Help();
// 用户信息
void userConfig();

int main(int argc, char *argv[])
{
    // 读取参数，设置用户名、密码
    ReadOptions(argc, argv);
    char str[MSGSIZE];
    if (user.length() == 0)
        userConfig();

    // 类初始化
    Client cl1(serverAddress.c_str(), port);

    // 连接服务器
    cl1.connect2server();
    cout << cl1.receive() << endl;

    // 发送用户名、密码
    cl1.sendSTR(user.c_str(),user.length());
    cout << cl1.receive() << endl;
    cl1.sendSTR(password.c_str(), password.length());
    cout << cl1.receive() << endl;
    cin.getline(str, MSGSIZE);


    while(TRUE)
    {
        printf("%s~SQL:",user.c_str());
        //从键盘输入指令
        cin.getline(str, MSGSIZE); //The function reads characters from stdin and loads them into szMessage

        // 发送指令
        cl1.sendSTR(str, strlen(str));

        // 接收返回结果并输出
        char *t=cl1.receive();
        cout << t << endl;
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
                cout << "unknow command: " << argv[i];
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
        { // 退格
            if (count == 0)
            {
                continue;
            }
            putchar('\b'); // 回退一格
            putchar(' ');  // 输出一个空格将原来的*隐藏
            putchar('\b'); // 再回退一格等待输入
            count--;
            password.pop_back();
        }
        if (count == 128 - 1)
        { // 最大长度为size-1
            continue;
        }
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9'))
        {                 // 密码只可包含数字和字母
            putchar('*'); // 接收到一个字符后, 打印一个*
            password += c;
            count++;
        }
    }
    password[count] = '\0';
    cout<<endl;
    cout<<password<<endl;
}

void Help()
{
    cout << "commant:" << endl;
    cout << "-u --uesr: user" << endl
         << "-p --port: port" << endl
         << "-pw --password: password" << endl
         << "-a --address: address" << endl;
}

#pragma clang diagnostic pop