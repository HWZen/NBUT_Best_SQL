#include "command_manager.h"
#include "function.h"
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <map>

using namespace std;
using namespace SQL;

// 指令转枚举map
typedef map<string, Command_Enum> com2enum;
com2enum c2e =
        {
                {"USE",      USE},
                {"use",      use},
                {"SELECT",   SELECT},
                {"select",   Select},
                {"FROM",     FROM},
                {"from",     from},
                {"WHERE",    WHERE},
                {"where",    where},
                {"AND",      AND},
                {"and",      And},
                {"ORDER",    ORDER},
                {"BY",       BY},
                {"order",    order},
                {"by",       by},
                {"INSERT",   INSERT},
                {"INTO",     INTO},
                {"into",     into},
                {"insert",   insert},
                {"DELETE",   ADelete},
                {"delect",   Delete},
                {"CREATE",   CREATE},
                {"create",   create},
                {"DATABASE", DATABASE},
                {"database", database},
                {"TABLE",    TABLE},
                {"table",    table}
        };

// 数据类型转枚举map
typedef map<string, DataType_Enum> data2enum;
data2enum d2e =
        {
                {"INT",       Int},
                {"CHAR",      CHAR},
                {"LONG_CHAR", LONG_CHAR},
                {"FLOAT",     FLOAT}
        };


Manager::Manager(const char *Usr, const char *password, string &buf)
{
    string rec_buf;
    user = Usr;
    psword = password;
    eng = new Engine("mysql", rec_buf);
    if (rec_buf == NO_SPACE_FILE)
    {
        mode = SQL::Init;
        buf = "no space file,maybe you need to create one. \nDo you want to init space? [Y/N]\n";
        return;
    }
    if (rec_buf.find("e") != rec_buf.npos)
    {
        buf = "sql init error.\n";
        buf += rec_buf;
        return;
    }

    rec_buf = eng->use("mysql");
    if (rec_buf.find("e") != rec_buf.npos)
    {
        buf = "can't found mysql data\n";
        buf += rec_buf;
        return;
    }

    void *databuf;
    databuf = new char[4 * 3 + CHAR_SIZE * 6];
    rec_buf = eng->serach("user", "name", Usr, databuf);
    if (rec_buf.find("e") != rec_buf.npos)
    {
        buf = "can't found user data: ";
        buf += Usr;
        buf += "\n";
        buf += rec_buf;
        return;
    }
    if (databuf != NULL)
    {
        if (!strcmp((char *) databuf + 4 + CHAR_SIZE, password))
        {
            mode = SQL::Normal;
            buf = "Login as ";
            buf += Usr;
            buf += "\n";
            buf += Usr;
            buf += "~SQL:";
            return;
        }
        else
        {
            mode = SQL::NoLogin;
            buf = "Error user or password.\nbey.\n";
        }
    }
}

Manager::~Manager()
{
}

string Manager::command(const char com[])
{
    char *ComBuf = new char[strlen(com) + 1];
    int parameterNum;
    char **parameter;
    parameter = new char *[16];
    memcpy(ComBuf, com, strlen(com) + 1);

    String2argv_argc(ComBuf, parameterNum, parameter);

    if (parameterNum == 0)
        return string("\n");

    if (strcmp(parameter[0], "exit") == 0)
        return string("bye.\n");
    else if (strcmp(parameter[0], "server") == 0)
        if (parameterNum >= 2)
            if (strcmp(parameter[1], "stop") == 0)
                exit(0);


    switch (mode)
    {
        case SQL::Normal:
            return NormalCom(parameterNum, parameter);
            break;
        case SQL::NoLogin:
            break;
        case SQL::Init:
            return InitSpace(parameterNum, parameter);
            break;

        default:
            break;
    }


    return string("received");
}

string Manager::InitSpace(int parameterNum, char **parmeter)
{
    string re_buf;
    if (parameterNum == 1 && (!strcmp(parmeter[0], "N") || !strcmp(parmeter[0], "n")))
        return "bye.";

    if (parameterNum == 1 && (!strcmp(parmeter[0], "Y") || !strcmp(parmeter[0], "y")))
        return "Please Enter root name and password.\n";

    if (parameterNum == 2)
    {
        string rec;
        rec = eng->InitSpace();
        if (rec.length() != 0)
            if (rec.find("e") != rec.npos)
                return "Creat Space Index file error. \n" + rec;


        rec = eng->CreatDB("mysql", "sys", "Admin");
        if (rec.length() != 0)
            if (rec.find("e") != rec.npos)
                return "Creat Datebase error. \n" + rec;

        rec = eng->use("mysql");
        if (rec.length() != 0)
            if (rec.find("e") != rec.npos)
                return "Use database error. \n" + rec;


        char Col[3][CHAR_SIZE] = {"id", "name", "Password"};
        SQL::DataType_Enum type_list[3] = {SQL::Int, SQL::CHAR, SQL::CHAR};
        rec = eng->CreatTable("user", 3, (const char **) Col, type_list);
        if (rec.length() != 0)
            if (rec.find("e") != rec.npos)
                return "Creat table error. \n" + rec;
        void *buf;
        buf = new char[4 + CHAR_SIZE * 2];
        *(int *) buf = 0;
        memcpy((char *) buf + 4, parmeter[0], CHAR_SIZE);
        memcpy((char *) buf + 4 + CHAR_SIZE, parmeter[1], CHAR_SIZE);
        rec = eng->insertRol("user", (const void *) buf);
        if (rec.length() != 0)
            if (rec.find("e") != rec.npos)
                return "Insert data error. \n" + rec;

        mode = SQL::Normal;
        return "Creat space success!\n" + user + "~SQL:";

    }

    if (parameterNum == 0)
        return "";
    return "";
}

string Manager::NormalCom(int argc, char **argv)
{
    if (argc <= 0)
        return "";
    com2enum::iterator it = c2e.find(argv[0]);
    if (it == c2e.end())
    {
        string re_buf = "error command: ";
        re_buf += argv[0];
        re_buf += "\n" + user + "~SQL:";
        return re_buf;
    }
    switch (it->second)
    {
        case CREATE:
        case create:
            return CreatCom(argc, argv);
            break;
        case insert:
        case INSERT:
            return insertCom(argc, argv);
            break;
        case SELECT:
        case Select:
            return selectCom(argc, argv);
            break;
        case USE:
        case use:
        {
            if (argc < 2)
                return "Please assign database name.\n" + user + "~SQL:";

            string rec = eng->use(argv[1]);
            if (rec.find("e") != rec.npos)
                return "Use database error. \n" + rec + "\n" + user + "~SQL:";
            return "Use database " + string(argv[1]) + "\n" + user + "~SQL:";
            break;
        }
        default:
            return "功能建设中i(～￣▽￣)～\n" + user + "~SQL:";
            break;
    }
    return "";
}

string Manager::CreatCom(int argc, char **argv)
{
    if (argc < 2)
        return "use CREATE TABLE or CREATE DATABASE\n" + user + "~SQL:";

    com2enum::iterator it = c2e.find(argv[1]);
    if (it == c2e.end())
    {
        string re_buf = "error command: ";
        re_buf += argv[1];
        re_buf += "\n" + user + "~SQL:";
        return re_buf;
    }

    switch (it->second)
    {
        case TABLE:
        case table:
        {
            int Col_Num = (argc - 3) / 2;
            char Col[MAX_COL][CHAR_SIZE];
            DataType_Enum *Col_types;
            Col_types = new DataType_Enum[Col_Num];
            data2enum::iterator it;
            for (int i = 0; i < Col_Num; i++)
            {
                memcpy(Col[i], argv[2 * i + 3], CHAR_SIZE);
                it = d2e.find(argv[2 * i + 4]);
                if (it == d2e.end())
                {
                    string re_buf = "error datatype: ";
                    re_buf += argv[2 * i + 4];
                    re_buf += "\n" + user + "~SQL:";
                    return re_buf;
                }
                Col_types[i] = it->second;
            }
            string rec = eng->CreatTable(argv[2], Col_Num, (const char **) Col, Col_types);
            if (rec.length() != 0)
                if (rec.find("e") != rec.npos)
                    return "Creat table error. \n" + rec + "\n" + user + "~SQL:";

            return "Creat table " + (string) argv[2] + "\n" + user + "~SQL:";
            break;
        }
        case DATABASE:
        case database:
        {
            if (argc < 3)
                return "please assign database name.\n" + user + "~SQL";
            string rec = eng->CreatDB(argv[2], "User", user.c_str());
            if (rec.length() != 0)
                if (rec.find("e") != rec.npos)
                    return "Creat Datebase error. \n" + rec + "\n" + user + "~SQL:";

            return "Creat database " + (string) argv[2] + "\n" + user + "~SQL:";
            break;
            break;
        }
        default:
            return "use CREATE TABLE or CREATE DATABASE\n" + user + "~SQL:";
    }
    return "";
}

string Manager::insertCom(int argc, char **argv)
{
    if (argc < 4)
        return "error command.\nPlease use INSERT INTO 'table_name' 'value1' 'value2' ...\n" + user + "~SQL:";
    com2enum::iterator it = c2e.find(argv[1]);

    if (it != c2e.end())
    {
        if (it->second != INTO && it->second != into)
            return "error command.\nPlease use INSERT INTO 'table_name' 'value1' 'value2' ...\n" + user + "~SQL:";
    }
    else
        return "error command.\nPlease use INSERT INTO 'table_name' 'value1' 'value2' ...\n" + user + "~SQL:";

    Tab_SPACE tab_header;
    string rec = eng->Get_Tab_Header(argv[2], tab_header);
    if (rec.find("e") != rec.npos)
        return "Get table Header error. \n" + rec + "\n" + user + "~SQL:";

    if (argc - 3 < tab_header.Col_num)
        return "too few Col num.\n" + user + "~SQL:";
    void *Rol;
    Rol = new char[tab_header.page_size];
    int dataptr = 0;
    for (int i = 0; i < tab_header.Col_num; i++)
    {
        switch (tab_header.Col_type[i])
        {
            case Int:
            {
                *(int *) ((char *) Rol + dataptr) = atoi(argv[i + 3]);
                dataptr += 4;
                break;
            }
            case CHAR:
            {
                memcpy(((char *) Rol + dataptr), argv[i + 3], CHAR_SIZE);
                dataptr += CHAR_SIZE;
                break;
            }
            case LONG_CHAR:
            {
                memcpy(((char *) Rol + dataptr), argv[i + 3], LONG_CHAR_SIZE);
                dataptr += LONG_CHAR_SIZE;
                break;
            }
            case FLOAT:
                *(double *) ((char *) Rol + dataptr) = atof(argv[i + 3]);
                dataptr += sizeof(double);
                break;
            default:
                return "error: read an unknown datatype\n" + user + "~SQL:";
        }
    }

    rec = eng->insertRol(argv[2], (const void **) Rol);
    if (rec.find("e") != rec.npos)
        return "Insert data error. \n" + rec + "\n" + user + "~SQL:";
    return "Insert data into " + string(argv[2]) + "\n" + user + "~SQL:";
}

string Manager::selectCom(int argc, char **argv)
{
    if(argc<4)
        return "error command.\nUse SELECT 'Col_name or *' FROM 'Tab_name'\n" + user + "~SQL:";
    
    if(argc>=5)
        if (c2e.find(string(argv[4]))->second == WHERE || c2e.find(string(argv[4]))->second == where)
            return "功能尚未开发i(～￣▽￣)～\n" + user + "~SQL:";

    Tab_SPACE tab_header;
    string rec = eng->Get_Tab_Header(argv[3], tab_header);
    if (rec.find("e") != rec.npos)
        return "Get table Header error. \n" + rec + "\n" + user + "~SQL:";

    void *p = new char[tab_header.page_size * tab_header.Rol_num];
    rec = eng->serach(argv[3], argv[1], NULL, p);
    if (rec.find("e") != rec.npos)
        return "Serach data error. \n" + rec + "\n" + user + "~SQL:";

    int Rol_num = atoi(rec.c_str());
    int dataptr = 0;
    string re_data = "";
    for (int i = 0; i < tab_header.Col_num; i++)
    {
        re_data += tab_header.Col[i];
        re_data += "  ";
    }
    re_data += "\n";
    for (int i = 0; i < Rol_num; i++)
    {
        for (int j = 0; j < tab_header.Col_num; j++)
        {
            switch(tab_header.Col_type[j])
            {
                case Int:
                    re_data += to_string(*(int *)((char *)p +dataptr));
                    dataptr += 4;
                    break;
                case CHAR:
                    re_data += ((char *)p + dataptr);
                    dataptr += CHAR_SIZE;
                    break;
                case LONG_CHAR:
                    re_data += ((char *)p + dataptr);
                    dataptr += LONG_CHAR_SIZE;
                    break;
                case FLOAT:
                    re_data += to_string(*(double *)((char *)p + dataptr));
                    dataptr += sizeof(double);
                default:
                    break;
            }
            re_data += " ";
        }
        re_data += "\n";
    }
    return re_data + "\n" + user + "~SQL:";
}

