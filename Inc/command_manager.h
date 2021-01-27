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
    // �����ϵ��׽���
    socketor client;

    // user��password(passwordҪAES-256����)
    string user;
    string psword;

    // ����������ָ��
    Engine *eng;

    // ��ǰ����״̬
    SQL::Mode mode;

    // �����������
    void MAIN();
public:
    // ����
    explicit Manager(socketor socket);
    ~Manager();

    
    // ָ������
    string command(const string &com);

    // ��ʼ���ռ�ָ��
    string InitSpace(int parameterNum, vector<string> parmeter);
    
    // ��ָͨ��
    string NormalCom(int argc, std::vector<string> argv);

    // ����ָ��½����ݿ⡢����
    string CreatCom(int argc, vector<string> argv);

    // ����ָ��
    string insertCom(int argc, vector<string> argv);

    // selectָ���
    string selectCom(int argc, vector<string> argv);

};








#endif