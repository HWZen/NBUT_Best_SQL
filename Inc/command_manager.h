#ifndef _MANAGER_
#define _MANAGER_
#include "SQL_engine.h"
#include <string>
using namespace std;

class Manager
{
private:
    char user[16];
    char password[32];

public:
    Manager(const char Usr[],const char password[]);
    ~Manager();

    string command(const char com[]);

};

Manager::Manager(const char Usr[],const char password[])
{
    
}

Manager::~Manager()
{
}






#endif