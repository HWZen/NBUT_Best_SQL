#ifndef _MANAGER_
#define _MANAGER_
#include "SQL_engine.h"
#include <string>
using namespace std;
using namespace SQL;

class Manager
{
private:
    // user��password(passwordҪAES-256����)
    string user;
    string psword;

    // ����������ָ��
    Engine *eng;

    // ��ǰ����״̬
    SQL::Mode mode = SQL::NoLogin;

public:
    // ����
    Manager(const char *Usr, const char *password, string &buf);
    ~Manager();
    
    // ָ������
    string command(const char com[]);

    // ��ʼ���ռ�ָ��
    string InitSpace(int parameterNum, char **parmeter);
    
    // ��ָͨ��
    string NormalCom(int argc, char **argv);

    // ����ָ��½����ݿ⡢����
    string CreatCom(int argc, char **argv);

    // ����ָ��
    string insertCom(int argc, char **argv);

    // selectָ���
    string selectCom(int argc, char **argv);
};








#endif