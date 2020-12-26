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


Manager::Manager(const char *Usr, const char *password, string &buf)
{
    string rec_buf;
    user = Usr;
    psword = password;
    eng = new Engine("mysql", rec_buf);
    if(rec_buf==NO_SPACE_FILE)
    {
        mode = Create;
        buf = "no space file,maybe you need to create one. \nDo you want to init space? [Y/N]\n";
    }
        
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


    switch (mode)
    {
    case SQL::Normal:
        break;
    case SQL::Init:
        break;
    case SQL::Create:
        return CreateSpace(parameterNum, parameter);
        break;

    default:
        break;
    }


    return string("received");
}

string Manager::CreateSpace(int parameterNum, char **parmeter)
{
    string re_buf;
    if(parameterNum==1 && (!strcmp(parmeter[0],"N") || !strcmp(parmeter[0],"n")))
        return "bye.";

    if (parameterNum == 1 && (!strcmp(parmeter[0], "Y") || !strcmp(parmeter[0], "y")))
        return "Please Enter root name and password.\n";

    if(parameterNum==2)
    {
        string rec;
        rec = eng->CreatSpace();
        if(rec.length()!=0)
            if(rec.find("e")!=rec.npos)
                return "Creat Space Index file error. \n" + rec;


        rec =  eng->CreatDB("mysql", "sys", "Admin");
        if (rec.length() != 0)
            if (rec.find("e") != rec.npos)
                return "Creat Datebase error. \n" + rec;

        rec =  eng->use("mysql");
        if (rec.length() != 0)
            if (rec.find("e") != rec.npos)
                return "Use database error. \n" + rec;


        char Col[3][32] = {"id", "name","Group"};
        SQL::DataType_Enum type_list[3] = {SQL::Int, SQL::CHAR, SQL::CHAR};
        rec = eng->CreatTable("user", 3, Col, type_list);
        if (rec.length() != 0)
            if (rec.find("e") != rec.npos)
                return "Creat table error. \n" + rec;

        rec = eng->insertRol("user", (void **)parmeter);
        if (rec.length() != 0)
            if (rec.find("e") != rec.npos)
                return "Insert data error. \n" + rec;

        return "Creat space success!\n" + user + "~SQL:";

        mode=SQL::Normal;
    }

    if(parameterNum==0)
        return "";
    return "";
}

