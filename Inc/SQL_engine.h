#pragma once
#ifndef _SQL_ENGINE
#define _SQL_ENGINE
#include <cstdlib>
#include <cstdio>
#include <string>
using namespace std;

/*******************/
/**服务器参数宏定义**/

#define ENGINE_VERSION "0.0.1"
#define VERSION_SIZE 8

#define MAX_DB_NUM 16
#define MAX_TABLE_NUM 32
#define MAX_INDEX_NUM 128
#define PATH_SIZE 256

#define CHAR_SIZE 16
#define LONG_CHAR_SIZE 128

#define MAX_COL 16
#define MAX_VOID_BUF 128

/**结束定义**/
/***********/



extern char *PATH;

/*****************/
/**错误和警告定义**/
//error and waring type define
#define NO_SPACE_FILE "e 0 "
#define NO_DB_NAME "e 1 "
#define FILE_CREATE_FAIL "e 2 "
#define FILE_OPEN_FAIL "e 3"
#define NO_DB_SPACE_FILE "e 4"
#define NO_TARGET "e 5"
#define TARGET_EXIST "e 6"

#define SPACE_PATH_WARN "w 0 "
#define SPACE_VERSION_WARN "w 1 "
/**结束定义**/
/***********/

// 整了个SQL命名空间，因为有些枚举名称和C/C++关键字冲突
namespace SQL
{
    // 指令枚举
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

    // 数据类型枚举
    enum DataType_Enum
    {
        Int,
        CHAR,
        LONG_CHAR,
        FLOAT,
        BOOL
    };

    // 管理层模式枚举
    enum Mode
    {
        Normal,
        Init,
    };
} // namespace SQL


// 工作区配置结构体
struct SQL_SPACE
{
    char PATH[PATH_SIZE] = {0};
    int DB_Num;
    char DB[MAX_DB_NUM][CHAR_SIZE];
};

// 数据库配置结构体
struct DB_SPACE
{
    char PATH[PATH_SIZE] = {0};
    char Group[CHAR_SIZE];
    char Owner[CHAR_SIZE];
    int Table_Num;
    int Index_Num;
    char Table[MAX_TABLE_NUM][CHAR_SIZE];
    char Index[MAX_INDEX_NUM][2 * CHAR_SIZE];
};

// 表配置结构体
struct Tab_SPACE
{
    char PATH[PATH_SIZE] = {0};
    int Col_num{};
    char Col[MAX_COL][CHAR_SIZE]{};
    SQL::DataType_Enum Col_type[MAX_COL]{};
    int page_size{};
    int Rol_num = 0;
    long Rol_void_buf[MAX_VOID_BUF] = {0};
};

// 数据库引擎类
class Engine
{
private:
    friend class Manager;
    
    // 工作路径
    string path;

    // 工作区配置文件指针
    FILE *space_fptr;

    // 工作区配置
    SQL_SPACE space_Header;

    // 目前使用的数据库名字
    string DB_name;

    // 目前使用的数据库配置
    DB_SPACE DB_Header;

    // 判断数据库是否存在
    bool findDB(const char *name);

    // 检测表是否存在
    bool findTab(const char *name);

public:
    Engine(const char *use, string &return_buf);

    // 配置工作区
    string InitSpace();

    // 创建数据库
    string CreatDB(const char *name, const char *group, const char *owner);

    // 创建表
    string CreatTable(const char *name, int Col_Num, const char **Col, const SQL::DataType_Enum *Col_type);

    // 选择数据库
    string use(const char *name);

    // 插入
    string insertRol(const char *Tab_name, const void *argv);

    // 获取表配置结构体
    string Get_Tab_Header(const char *name, Tab_SPACE &buf);

    // 查找
    string serach(const char *Tab_name, const char *Col_name, const void *target, void *buf);

};



#endif