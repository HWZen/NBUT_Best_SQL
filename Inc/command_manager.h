#ifndef _MANAGER_
#define _MANAGER_
#include "SQL_engine.h"
#include "socketor.h"
#include <string>
#include <vector>

using namespace std;
using namespace SQL;

class Manager
{
private:
    // 连接上的套接字
    socketor client;

    // user和password(password要AES-256加密)
    string user;
    string psword;

    // 服务器引擎指针
    Engine *eng;

    // 当前连接状态
    SQL::Mode mode;

    // 管理层主函数
    void MAIN();
public:
    // 构造
    explicit Manager(socketor socket);
    ~Manager();

    
    // 指令处理入口
    string command(const string &com);

    // 初始化空间指令
    string InitSpace(int parameterNum, vector<string> parmeter);
    
    // 普通指令
    string NormalCom(int argc, std::vector<string> argv);

    // 创建指令（新建数据库、建表）
    string CreatCom(int argc, vector<string> argv);

    // 插入指令
    string insertCom(int argc, vector<string> argv);

    // select指令函数
    string selectCom(int argc, vector<string> argv);

};








#endif