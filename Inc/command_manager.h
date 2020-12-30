#ifndef _MANAGER_
#define _MANAGER_
#include "SQL_engine.h"
#include <string>
using namespace std;
using namespace SQL;

class Manager
{
private:
    // user和password(password要AES-256加密)
    string user;
    string psword;

    // 服务器引擎指针
    Engine *eng;

    // 当前连接状态
    SQL::Mode mode = SQL::NoLogin;

public:
    // 构造
    Manager(const char *Usr, const char *password, string &buf);
    ~Manager();
    
    // 指令处理入口
    string command(const char com[]);

    // 初始化空间指令
    string InitSpace(int parameterNum, char **parmeter);
    
    // 普通指令
    string NormalCom(int argc, char **argv);

    // 创建指令（新建数据库、建表）
    string CreatCom(int argc, char **argv);

    // 插入指令
    string insertCom(int argc, char **argv);

    // select指令函数
    string selectCom(int argc, char **argv);
};








#endif