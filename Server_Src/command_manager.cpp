#include "command_manager.h"
#include "function.h"
#include <stdio.h>
#include <string.h>
using namespace std;
using namespace SQL;

map<Command_Enum, const char *> c2e =
{
        {USE,      "USE"}, {use, "USE"},
        {SELECT,   "SELECT"}, {Select, "selece"},
        {FROM,     "FROM"}, {from, "from"},
        {WHERE,    "WHERE"}, {where, "where"},
        {AND,      "AND"}, {And, "and"},
        {ORDER,    "ORDER"}, {BY, "BY"}, {order, "order"}, {by, "by"},
        {INSERT,   "INSERT"}, {INTO, "INTO"}, {into, "into"}, {insert, "insert"},
        {ADelete,  "ADelete"}, {Delete, "delete"},
        {CREATE,   "CREATE"}, {create, "create"},
        {DATABASE, "DATABASE"}, {database, "database"},
        {TABLE,    "TABLE"}, {table, "table"}
};

map<DataType_Enum,const char *>d2e=
{
    {Int,"INT"},
    {CHAR,"CHAR"},
    {LONG_CHAR,"LONG_CHAR"},
    {FLOAT,"FLOAT"}
};


Manager::Manager(const char Usr[], const char password[])
{
    user = Usr;
    psword = password;
}

Manager::~Manager()
{
}

string Manager::command(const char com[])
{
    char *ComBuf = new char[strlen(com)+1];
    int parameterNum;
    char **parameter;
    parameter = new char *[16];
    memcpy(ComBuf, com, strlen(com)+1);

    String2argv_argc(ComBuf, parameterNum, parameter);

    if(parameterNum==0)
        return string("");
    
    if (strcmp(parameter[0], "exit") == 0)
        return string("bye.");
    else if (strcmp(parameter[0], "server") == 0)
        if(parameterNum>=2)
            if(strcmp(parameter[1],"stop")==0)
                exit(0);
    return string("received");
    
}