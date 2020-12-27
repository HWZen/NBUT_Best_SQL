#ifndef _MANAGER_
#define _MANAGER_
#include "SQL_engine.h"
#include <string>
using namespace std;
using namespace SQL;

class Manager
{
private:
    string user;
    string psword;
    Engine *eng;
    SQL::Mode mode = SQL::NoLogin;

public:
    Manager(const char *Usr, const char *password, string &buf);
    ~Manager();

    string command(const char com[]);

    string CreateSpace(int parameterNum, char **parmeter);
    string NormalCom(int argc, char **argv);
    string CreatCom(int argc, char **argv);
    string insertCom(int argc, char **argv);
    string selectCom(int argc, char **argv);
};








#endif