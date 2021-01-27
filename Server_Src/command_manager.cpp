#include "command_manager.h"
#include "function.h"
#include <string.h>
#include <iostream>
#include <map>
#include <vector>

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
                {"CHAR",      SQL::CHAR},
                {"LONG_CHAR", LONG_CHAR},
                {"FLOAT",     SQL::FLOAT},
                {"BOOL",      SQL::BOOL}
        };


Manager::Manager(socketor socket)
{
    client = socket;
    string rec_buf;
    eng = new Engine("mysql", rec_buf);
    if (rec_buf == NO_SPACE_FILE)
    {
        mode = SQL::Init;
        rec_buf = "no space file,maybe you need to create one. \nDo you want to init space? [Y/N]\n";
        socket.Send(rec_buf);
        socket.Send("over");
        MAIN();
    }
    if (rec_buf.find('e') != string::npos)
    {
        rec_buf = "sql init error.\n";
        rec_buf += rec_buf;
        socket.Send(rec_buf);
        socket.Send("bye.");
        return;
    }

    rec_buf = eng->use("mysql");
    if (rec_buf.find('e') != string::npos)
    {
        rec_buf = "can't found mysql data\n" + rec_buf;
        socket.Send(rec_buf);
        return;
    }


    do
    {
        socket.Send("User:");
        socket.Send("over");
        user = socket.receive();
        socket.Send("Password:");
        socket.Send("pw mode");
        psword = socket.receive();
        void *databuf;
        databuf = new char[4 * 3 + CHAR_SIZE * 6];
        rec_buf = eng->serach("user", "name", user.c_str(), databuf);
        if (rec_buf.find('e') != string::npos)
        {
            rec_buf = "can't found user data: " + user + "\n" + rec_buf;
            socket.Send(rec_buf);
            socket.Send("\n");
        }
        else
        {
            if (databuf != NULL)
            {
                if (!strcmp((char *) databuf + 4 + CHAR_SIZE, psword.c_str()))
                {
                    mode = SQL::Normal;
                    rec_buf = "Login as ";
                    rec_buf += user;
                    rec_buf += "\n";
                    rec_buf += user;
                    rec_buf += "~SQL:";
                    socket.Send(rec_buf);
                    socket.Send("over");
                    break;
                }
                else
                {
                    // mode = SQL::NoLogin;
                    rec_buf = "Error user or password.\nbey.\n";
                    socket.Send(rec_buf);
                }
            }
        }
    } while (1);

    MAIN();
}

void Manager::MAIN()
{
    string com_rec;
    while ((com_rec = client.receive()) != "lose connect")
    {
        client.Send(command(com_rec));
    }


}

Manager::~Manager()
= default;

string Manager::command(const string &com)
{
//    char *ComBuf = new char[strlen(com) + 1];
//    int parameterNum;
//    char **parameter;
//    parameter = new char *[16];
//    memcpy(ComBuf, com, strlen(com) + 1);


    vector<string> parameter = String2argv_argc(com);
    int parameterNum = parameter.size();

    if (parameterNum == 0)
        return string("\n");

    if (parameter[0] == "exit")
    {
        client.Send("bye.\n");
        client.Send("over");
        client.close_connect();
        return "";
    }
    else if (parameter[0] == "server")
        if (parameterNum >= 2)
            if (parameter[1] == "stop")
            {
                client.Send("bye.\n");
                client.Send("over");
                client.close_connect();
                exit(0);
            }


    switch (mode)
    {
        case SQL::Normal:
            client.Send(NormalCom(parameterNum, parameter));
            return "over";
        case SQL::Init:
            return InitSpace(parameterNum, parameter);
        default:
            break;
    }


    return string("received");
}

string Manager::InitSpace(int parameterNum, vector<string> parmeter)
{
    string re_buf;
    if (parameterNum == 1 && (parmeter[0] == "N" || parmeter[0] == "n"))
    {
        client.Send("bye.");
        client.close_connect();
        return "";
    }

    if (parameterNum == 1 && (parmeter[0] == "Y" || parmeter[0] == "y"))
    {
        client.Send("please enter root user name:");
        client.Send("over");
        user = client.receive();
        if (user == "lose connect")
            return "";
        client.Send("Please enter root user password:");
        client.Send("pw mode");
        psword = client.receive();
        if (psword == "lose connect")
            return "";
    }


    string rec;
    rec = eng->InitSpace();
    if (rec.length() != 0)
        if (rec.find('e') != string::npos)
        {
            client.Send("Creat Space Index file error. \n" + rec);
            client.Send("over");
            client.close_connect();
            return "";
        }


    rec = eng->CreatDB("mysql", "sys", "Admin");
    if (rec.length() != 0)
        if (rec.find('e') != string::npos)
        {
            client.Send("Creat Datebase error. \n" + rec);
            client.Send("over");
            client.close_connect();
            return "";
        }

    rec = eng->use("mysql");
    if (rec.length() != 0)
        if (rec.find('e') != string::npos)
        {
            client.Send("Use database error. \n" + rec);
            client.Send("over");
            client.close_connect();
            return "";
        }


    char Col[3][CHAR_SIZE] = {"id", "name", "Password"};
    SQL::DataType_Enum type_list[3] = {SQL::Int, SQL::CHAR, SQL::CHAR};
    rec = eng->CreatTable("user", 3, (const char **) Col, type_list);
    if (rec.length() != 0)
        if (rec.find('e') != string::npos)
        {
            client.Send("Creat table error. \n" + rec);
            client.Send("over");
            client.close_connect();
            return "Creat table error. \n" + rec;
        }

    void *buf;
    buf = new char[4 + CHAR_SIZE * 2];
    *(int *) buf = 0;
    memcpy((char *) buf + 4, user.c_str(), CHAR_SIZE);
    memcpy((char *) buf + 4 + CHAR_SIZE, psword.c_str(), CHAR_SIZE);
    rec = eng->insertRol("user", (const void *) buf);
    if (rec.length() != 0)
        if (rec.find('e') != string::npos)
        {
            client.Send("Insert data error. \n" + rec);
            client.Send("over");
            client.close_connect();
            return "Insert data error. \n" + rec;
        }

    mode = SQL::Normal;
    client.Send("Creat space success!\n" + user + "~SQL:");
    return "over";
}

string Manager::NormalCom(int argc, vector<string> argv)
{
    if (argc <= 0)
        return "";
    auto c2e_it = c2e.find(argv[0]);
    if (c2e_it == c2e.end())
    {
        string re_buf = "error command: ";
        re_buf += argv[0];
        re_buf += "\n" + user + "~SQL:";
        return re_buf;
    }
    switch (c2e_it->second)
    {
        case CREATE:
        case create:
            return CreatCom(argc, argv);
        case insert:
        case INSERT:
            return insertCom(argc, argv);
        case SELECT:
        case Select:
            return selectCom(argc, argv);
        case USE:
        case use:
        {
            if (argc < 2)
                return "Please assign database name.\n" + user + "~SQL:";

            string rec = eng->use(argv[1].c_str());
            if (rec.find('e') != string::npos)
                return "Use database error. \n" + rec + "\n" + user + "~SQL:";
            return "Use database " + argv[1] + "\n" + user + "~SQL:";
        }
        default:
            return "功能建设中i(～￣▽￣)～\n" + user + "~SQL:";
    }
}

string Manager::CreatCom(int argc, vector<string> argv)
{
    if (argc < 2)
        return "use CREATE TABLE or CREATE DATABASE\n" + user + "~SQL:";

    auto c2e_it = c2e.find(argv[1]);
    if (c2e_it == c2e.end())
    {
        string re_buf = "error command: ";
        re_buf += argv[1];
        re_buf += "\n" + user + "~SQL:";
        return re_buf;
    }

    switch (c2e_it->second)
    {
        case TABLE:
        case table:
        {
            int Col_Num = (argc - 3) / 2;
            char Col[MAX_COL][CHAR_SIZE];
            DataType_Enum *Col_types;
            Col_types = new DataType_Enum[Col_Num];
            data2enum::iterator d2e_it;
            for (int i = 0; i < Col_Num; i++)
            {
                memcpy(Col[i], argv[2 * i + 3].c_str(), CHAR_SIZE);
                d2e_it = d2e.find(argv[2 * i + 4]);
                if (d2e_it == d2e.end())
                {
                    string re_buf = "error datatype: ";
                    re_buf += argv[2 * i + 4];
                    re_buf += "\n" + user + "~SQL:";
                    return re_buf;
                }
                Col_types[i] = d2e_it->second;
            }
            string rec = eng->CreatTable(argv[2].c_str(), Col_Num, (const char **) Col, Col_types);
            if (rec.length() != 0)
                if (rec.find('e') != string::npos)
                    return "Creat table error. \n" + rec + "\n" + user + "~SQL:";

            return "Creat table " + (string) argv[2] + "\n" + user + "~SQL:";
        }
        case DATABASE:
        case database:
        {
            if (argc < 3)
                return "please assign database name.\n" + user + "~SQL";
            string rec = eng->CreatDB(argv[2].c_str(), "User", user.c_str());
            if (rec.length() != 0)
                if (rec.find('e') != string::npos)
                    return "Creat Datebase error. \n" + rec + "\n" + user + "~SQL:";

            return "Creat database " + (string) argv[2] + "\n" + user + "~SQL:";
        }
        default:
            return "use CREATE TABLE or CREATE DATABASE\n" + user + "~SQL:";
    }
}

string Manager::insertCom(int argc, vector<string> argv)
{
    if (argc < 4)
        return "error command.\nPlease use INSERT INTO 'table_name' 'value1' 'value2' ...\n" + user + "~SQL:";
    auto c2e_it = c2e.find(argv[1]);

    if (c2e_it != c2e.end())
    {
        if (c2e_it->second != INTO && c2e_it->second != into)
            return "error command.\nPlease use INSERT INTO 'table_name' 'value1' 'value2' ...\n" + user + "~SQL:";
    }
    else
        return "error command.\nPlease use INSERT INTO 'table_name' 'value1' 'value2' ...\n" + user + "~SQL:";

    Tab_SPACE tab_header;
    string rec = eng->Get_Tab_Header(argv[2].c_str(), tab_header);
    if (rec.find('e') != string::npos)
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
            case SQL::Int:
            {
                *(int *) ((char *) Rol + dataptr) = atoi(argv[i + 3].c_str());
                dataptr += 4;
                break;
            }
            case SQL::CHAR:
            {
                memcpy(((char *) Rol + dataptr), argv[i + 3].c_str(), CHAR_SIZE);
                dataptr += CHAR_SIZE;
                break;
            }
            case LONG_CHAR:
            {
                memcpy(((char *) Rol + dataptr), argv[i + 3].c_str(), LONG_CHAR_SIZE);
                dataptr += LONG_CHAR_SIZE;
                break;
            }
            case SQL::FLOAT:
                *(double *) ((char *) Rol + dataptr) = atof(argv[i + 3].c_str());
                dataptr += sizeof(double);
                break;
            case SQL::BOOL:
                *(bool *) ((char *) Rol + dataptr) = atoi(argv[i + 3].c_str());
                dataptr += sizeof(bool);
                break;
            default:
                return "error: read an unknown datatype\n" + user + "~SQL:";
        }
    }

    rec = eng->insertRol(argv[2].c_str(), (const void **) Rol);
    if (rec.find('e') != string::npos)
        return "Insert data error. \n" + rec + "\n" + user + "~SQL:";
    return "Insert data into " + argv[2] + "\n" + user + "~SQL:";
}

string Manager::selectCom(int argc, vector<string> argv)
{
    if (argc < 4)
        return "error command.\nUse SELECT 'Col_name or *' FROM 'Tab_name'\n" + user + "~SQL:";

    if (argc >= 5)
        if (c2e.find(argv[4])->second == WHERE || c2e.find(argv[4])->second == where)
            return "功能尚未开发i(～￣▽￣)～\n" + user + "~SQL:";

    Tab_SPACE tab_header;
    string rec = eng->Get_Tab_Header(argv[3].c_str(), tab_header);
    if (rec.find('e') != string::npos)
        return "Get table Header error. \n" + rec + "\n" + user + "~SQL:";

    void *p = new char[tab_header.page_size * tab_header.Rol_num];
    rec = eng->serach(argv[3].c_str(), argv[1].c_str(), NULL, p);
    if (rec.find('e') != string::npos)
        return "Serach data error. \n" + rec + "\n" + user + "~SQL:";

    int Rol_num = atoi(rec.c_str());
    int dataptr = 0;
    string re_data;
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
            switch (tab_header.Col_type[j])
            {
                case SQL::Int:
                    re_data += to_string(*(int *) ((char *) p + dataptr));
                    dataptr += 4;
                    break;
                case SQL::CHAR:
                    re_data += ((char *) p + dataptr);
                    dataptr += CHAR_SIZE;
                    break;
                case LONG_CHAR:
                    re_data += ((char *) p + dataptr);
                    dataptr += LONG_CHAR_SIZE;
                    break;
                case SQL::FLOAT:
                    re_data += to_string(*(double *) ((char *) p + dataptr));
                    dataptr += sizeof(double);
                case SQL::BOOL:
                    re_data += (*(bool *) ((char *) p + dataptr)) ? "true" : "false";
                    dataptr += sizeof(bool);
                    break;
                default:
                    break;
            }
            re_data += " ";
        }
        re_data += "\n";
    }
    return re_data + "\n" + user + "~SQL:";
}

