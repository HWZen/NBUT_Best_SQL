#ifndef _MANAGER_
#define _MANAGER_
#include "SQL_engine.h"
#include <string>
using namespace std;

class Manager
{
private:
    string user;
    string psword;

public:
    Manager(const char Usr[],const char password[]);
    ~Manager();

    string command(const char com[]);

};








#endif