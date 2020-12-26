#pragma once
#ifndef _SQL_ENGINE
#define _SQL_ENGINE
#include <stdlib.h>
#include <stdio.h>
#include <string>
using namespace std;

#define ENGINE_VERSION "0.0.1"
#define VERSION_SIZE 8

#define MAX_DB_NUM 16
#define MAX_TABLE_NUM 32
#define MAX_INDEX_NUM 128
#define PATH_SIZE 256

extern char *PATH;

//error and waring type define
#define NO_SPACE_FILE "e 0 "
#define NO_DB_NAME "e 1 "
#define FILE_CREATE_FAIL "e 2 "
#define NO_DB_SPECIFIED "e 3"
#define NO_TARGET "e 4"
#define TARGET_EXIST "e 5"

#define SPACE_PATH_WARN "w 0 "
#define SPACE_VERSION_WARN "w 1 "

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

    enum Mode
    {
        Init,
        Normal,
        Create
    };
} // namespace SQL

#define CHAR_SIZE 16
#define LONG_CHAR_SIZE 128

struct SQL_SPACE
{
    char PATH[PATH_SIZE] = {0};
    int DB_Num;
    char DB[MAX_DB_NUM][32];
};

struct DB_SPACE
{
    char PATH[PATH_SIZE] = {0};
    char Group[32];
    char Owner[32];
    int Table_Num;
    int Index_Num;
    char Table[MAX_TABLE_NUM][32];
    char Index[MAX_INDEX_NUM][64];
};

struct Tab_SPACE
{
    char PATH[PATH_SIZE] = {0};
    int Col_num;
    char Col[16][32];
    SQL::DataType_Enum Col_type[16];
    int page_size;
    int Rol_num = 0;
    int Rol_void_buf[32] = {0};
};

class Engine
{
private:
    friend class Manager;
    
    string path;

    FILE *space_fptr;

    SQL_SPACE space_Header;

    string DB_name;

    DB_SPACE DB_Header;

public:
    Engine(const char *use, string &return_buf);

    string CreatSpace();

    string CreatDB(const char *name, const char *group, const char *owner);

    string CreatTable(const char *name, int Col_Num, const char Col[][32], const SQL::DataType_Enum *Col_type);

    string use(const char *name);

    string insertRol(const char *Tab_name, void **argv);
    

};



#endif