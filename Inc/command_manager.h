#ifndef _MANAGER_
#define _MANAGER_
#include "SQL_engine.h"
#include <string>
#include <map>
using namespace std;
using namespace SQL;

class Manager
{
private:
    string user;
    string psword;
    Engine *eng;
    SQL::Mode mode = SQL::Init;

public:
    Manager(const char *Usr, const char *password, string &buf);
    ~Manager();

    string command(const char com[]);

    string CreateSpace(int parameterNum, char **parmeter);
};








#endif