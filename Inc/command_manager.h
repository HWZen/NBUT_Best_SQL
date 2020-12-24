#ifndef _MANAGER_
#define _MANAGER_
#include "SQL_engine.h"
#include <string>
#include <map>
using namespace std;

namespace SQL
{
    enum Command_Enum
    {
        USE,use,
        SELECT,Select,
        FROM,from,
        WHERE,where,
        AND,And,
        ORDER,BY,order,by,
        INSERT,INTO,insert,into,
        ADelete,Delete,
        CREATE,create,
        DATABASE,database,
        TABLE,table,
    };

    enum DataType_Enum
    {
        Int,
        CHAR,
        LONG_CHAR,
        FLOAT
    };
}

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