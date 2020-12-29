#include "SQL_engine.h"
#include "osplatformutil.h"
#include "function.h"
#include <cstring>
#include <iostream>

#ifdef I_OS_WIN
#include <direct.h>
#include <io.h>
#define CreatDir(x) mkdir(x)
const char Slash = '\\';
#endif

#ifdef I_OS_LINUX

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define CreatDir(x) mkdir(x, S_IRWXU)
const char Slash = '/';

#endif

using namespace std;

Engine::Engine(const char *use_name, string &re_buf)
{
    re_buf.clear();
    path = PATH;

    // 打开表
    space_fptr = fopen("SQL.space", "rb");
    if (space_fptr == NULL)
    {
        //error: load SQL.space fail
        re_buf += NO_SPACE_FILE;
        DB_name[0] = 0;
        return;
    }
    else
    {
        // 读取工作区配置文件，检测错误
        char space_path[PATH_SIZE];
        fread(space_path, sizeof(space_path), 1, space_fptr);

        if (strcmp(space_path, path.c_str()) != 0)
            //waring: space may has been moved.
            re_buf += SPACE_PATH_WARN;

        char space_version[VERSION_SIZE];
        fread(space_version, sizeof(space_version), 1, space_fptr);

        if (strcmp(space_version, ENGINE_VERSION) != 0)
            //waring: space engine version is different from now
            re_buf += SPACE_VERSION_WARN;

        fread(&space_Header, sizeof(SQL_SPACE), 1, space_fptr);
        fclose(space_fptr);
        re_buf += use(use_name);
        return;
    }
}

string Engine::use(const char *name)
{
    string re_buf = "";

    // 检查数据库是否存在
    if (space_Header.PATH[0] = 0)
    {
        re_buf += NO_SPACE_FILE;
        return re_buf;
    }

    if (!findDB(name))
        return NO_DB_NAME;
    // 检查完毕

    // 数据库绝对路径
    string Abs_Path = path;

    if (Abs_Path[Abs_Path.length() - 1] != Slash)
        Abs_Path += Slash;
    Abs_Path += name;

    // 打开数据库配置文件
    FILE *DB_fp = fopen(string(Abs_Path + Slash + "DB.space").c_str(), "rb");
    if (DB_fp == NULL)
        return NO_DB_SPECIFIED;

    DB_name = name;

    // 检查配置文件
    char space_path[PATH_SIZE];
    fread(space_path, sizeof(space_path), 1, DB_fp);
    if (strcmp(space_path, (path + Slash + name).c_str()) != 0)
        //waring: space may has been moved.
        re_buf += SPACE_PATH_WARN;

    char space_version[VERSION_SIZE];
    fread(space_version, sizeof(space_version), 1, DB_fp);
    if (strcmp(space_version, ENGINE_VERSION) != 0)
        //waring: space engine version is different from now
        re_buf += SPACE_VERSION_WARN;

    fread(&DB_Header, sizeof(DB_SPACE), 1, DB_fp);

    fclose(DB_fp);

    return re_buf;
}

string Engine::InitSpace()
{
    // 初始化工作区
    // 慎用：不会检测工作区是否已存在
    string re_buf = "";

    if (space_fptr != NULL)
        fclose(space_fptr);

    space_fptr = fopen("SQL.space", "wb+");
    if (space_fptr == NULL)
    {
        re_buf += FILE_CREATE_FAIL;
        return re_buf;
    }

    fwrite(PATH, sizeof(char[PATH_SIZE]), 1, space_fptr);
    fwrite((const char *) ENGINE_VERSION, sizeof((const char *) ENGINE_VERSION), 1, space_fptr);

    memcpy(space_Header.PATH, PATH, sizeof(char[PATH_SIZE]));
    space_Header.DB_Num = 0;
    fwrite(&space_Header, sizeof(SQL_SPACE), 1, space_fptr);

    fclose(space_fptr);

    return re_buf;
}

string Engine::CreatDB(const char *name, const char *group, const char *owner)
{
    string re_buf = "";

    // 数据库绝对路径
    string Abs_Path = path;

    if (Abs_Path[Abs_Path.length() - 1] != Slash)
        Abs_Path += Slash;
    Abs_Path += name;
    // 生成完毕

    // 检查并创建数据库

    // 检测数据库是否存在
    if (findDB(name))
        return TARGET_EXIST;
    if (access(Abs_Path.c_str(), 0) != 0)
    {
        if (CreatDir(Abs_Path.c_str()) != 0)
        {
            re_buf += FILE_CREATE_FAIL;
            return re_buf;
        }
    }
    else
    {
        re_buf += FILE_CREATE_FAIL;
        return re_buf;
    }
    // 创建完毕

    // 创建数据库配置文件
    FILE *DB_fp = fopen((Abs_Path + Slash + "DB.space").c_str(), "wb+");
    if (DB_fp == NULL)
    {
        re_buf += FILE_CREATE_FAIL;
        return re_buf;
    }

    fwrite(Abs_Path.c_str(), sizeof(char[PATH_SIZE]), 1, DB_fp);
    fwrite(ENGINE_VERSION, sizeof(char[VERSION_SIZE]), 1, DB_fp);

    memcpy(DB_Header.PATH, Abs_Path.c_str(), sizeof(char[PATH_SIZE]));
    memcpy(DB_Header.Group, group, sizeof(char[CHAR_SIZE]));
    memcpy(DB_Header.Owner, owner, sizeof(char[CHAR_SIZE]));
    DB_Header.Index_Num = 0;
    DB_Header.Table_Num = 0;
    fwrite(&DB_Header, sizeof(DB_SPACE), 1, DB_fp);
    fclose(DB_fp);
    // 创建完毕

    // 更新工作区配置文件
    space_fptr = fopen("SQL.space", "rb+");
    if (space_fptr == NULL)
    {
        re_buf += NO_SPACE_FILE;
        return re_buf;
    }
    fseek(space_fptr, sizeof(char[PATH_SIZE]) + sizeof(char[VERSION_SIZE]), SEEK_SET);
    memcpy(space_Header.DB[space_Header.DB_Num++], name, CHAR_SIZE);
    fwrite(&space_Header, sizeof(SQL_SPACE), 1, space_fptr);
    fclose(space_fptr);
    // 更新完毕
    return re_buf;
}

string Engine::CreatTable(const char *name, int Col_Num, const char **Col, const SQL::DataType_Enum *Col_type)
{
    string re_buf = "";

    // 检测数据库
    if (DB_name[0] == 0)
        return NO_DB_SPECIFIED;

    // 检测表是否存在
    if (findTab(name))
        return TARGET_EXIST;
    // 检测完毕

    // 数据库绝对路径
    string Abs_Path = path;
    if (Abs_Path[Abs_Path.length() - 1] != Slash)
        Abs_Path += Slash;
    Abs_Path += DB_name + Slash + name;

    // 创建表文件头
    FILE *Tab_fptr = fopen(Abs_Path.c_str(), "wb+");
    if (Tab_fptr == NULL)
        return FILE_CREATE_FAIL;

    // 打开数据库配置文件头
    FILE *DB_fp = fopen((DB_name + Slash + "DB.space").c_str(), "rb+");
    if (DB_fp == NULL)
        return NO_DB_SPECIFIED;

    // 开始创建表
    fwrite(Abs_Path.c_str(), sizeof(char[PATH_SIZE]), 1, Tab_fptr);
    fwrite(ENGINE_VERSION, sizeof(char[VERSION_SIZE]), 1, Tab_fptr);

    Tab_SPACE Header;
    strcpy(Header.PATH, Abs_Path.c_str());
    Header.Col_num = Col_Num;
    memcpy(Header.Col, Col, Col_Num * sizeof(char[CHAR_SIZE]));

    memcpy(Header.Col_type, Col_type, Col_Num * sizeof(SQL::DataType_Enum));
    Header.page_size = 0;
    for (int i = 0; i < Col_Num; i++)
    {
        switch (Header.Col_type[i])
        {
            case SQL::Int:
                Header.page_size += sizeof(int);
                break;
            case SQL::CHAR:
                Header.page_size += CHAR_SIZE;
                break;
            case SQL::LONG_CHAR:
                Header.page_size += LONG_CHAR_SIZE;
                break;
            case SQL::FLOAT:
                Header.page_size += sizeof(double);
            default:
                break;
        }
    }

    fwrite(&Header, sizeof(Header), 1, Tab_fptr);
    fclose(Tab_fptr);
    // 表创建完毕

    // 更新数据库配置文件
    fseek(DB_fp, sizeof(char[PATH_SIZE]) + sizeof(char[VERSION_SIZE]), SEEK_SET);
    memcpy(DB_Header.Table[DB_Header.Table_Num++], name, sizeof(char[CHAR_SIZE]));
    fwrite(&DB_Header, sizeof(DB_SPACE), 1, DB_fp);
    fclose(DB_fp);
    // 更行完毕

    return re_buf;
}

string Engine::insertRol(const char *Tab_name, const void *argv)
{
    string re_buf;
    // 检测数据库
    if (DB_name[0] == 0)
        return NO_DB_SPECIFIED;

    // 检测表是否存在
    if (!findTab(Tab_name))
        return NO_TARGET;
    // 检测完毕

    // 数据库绝对路径
    string Abs_Path = path;
    if (Abs_Path[Abs_Path.length() - 1] != Slash)
        Abs_Path += Slash;
    Abs_Path += DB_name + Slash + Tab_name;

    // 打开表文件
    FILE *Tab_fptr = fopen(Abs_Path.c_str(), "rb+");
    if (Tab_fptr == NULL)
        return FILE_CREATE_FAIL;

    // 检查配置
    char space_path[PATH_SIZE];
    fread(space_path, sizeof(space_path), 1, Tab_fptr);
    if (!strcmp(space_path, (path + Slash + DB_name + Slash + Tab_name).c_str()))
        //waring: space may has been moved.
        re_buf += SPACE_PATH_WARN;

    char space_version[VERSION_SIZE];
    fread(space_version, sizeof(space_version), 1, Tab_fptr);
    if (strcmp(space_version, ENGINE_VERSION) != 0)
        //waring: space engine version is different from now
        re_buf += SPACE_VERSION_WARN;

    Tab_SPACE tab_header;
    fread(&tab_header, sizeof(Tab_SPACE), 1, Tab_fptr);

    // 定位插入位置
    if (tab_header.Rol_void_buf[0] != 0)
    {
        /*
            从删除的空隙插入
        */
    }
    else
        fseek(Tab_fptr, PATH_SIZE + VERSION_SIZE + sizeof(Tab_SPACE) + tab_header.page_size * tab_header.Rol_num,
              SEEK_SET);

    if (DB_Header.Index[0] && 0)
    {
        /*
            如果有索引
        */
    }
    fwrite(argv, tab_header.page_size, 1, Tab_fptr);

    tab_header.Rol_num++;
    fseek(Tab_fptr, PATH_SIZE + VERSION_SIZE, SEEK_SET);
    fwrite(&tab_header, sizeof(Tab_SPACE), 1, Tab_fptr);

    fclose(Tab_fptr);

    return re_buf;
}

bool Engine::findDB(const char *name)
{
    if (space_Header.PATH[1] == 0)
        return false;
    for (int i = 0; i < space_Header.DB_Num; i++)
        if (!strcmp(space_Header.DB[i], name))
            return true;
    return false;
}

bool Engine::findTab(const char *name)
{
    if (DB_Header.PATH[1] == 0)
        return false;
    for (int i = 0; i < DB_Header.Table_Num; i++)
        if (!strcmp(DB_Header.Table[i], name))
            return true;
    return false;
}

string Engine::Get_Tab_Header(const char *name, Tab_SPACE &buf)
{
    if (!findTab(name))
        return NO_TARGET;

    FILE *Tab_fp = fopen((path + Slash + DB_name + Slash + name).c_str(), "rb");
    if (Tab_fp == NULL)
        return FILE_OPEN_FAIL;

    fseek(Tab_fp, PATH_SIZE + VERSION_SIZE, SEEK_SET);
    fread(&buf, sizeof(Tab_SPACE), 1, Tab_fp);
    fclose(Tab_fp);

    return "";
}


string Engine::serach(const char *Tab_name, const char *Col_name, const void *target, void *buf)
{
    Tab_SPACE Tab_headler;
    string re_buf = Get_Tab_Header(Tab_name, Tab_headler);
    if (re_buf.find("e") != re_buf.npos)
        return re_buf;

    FILE *Tab_fp = fopen((path + Slash + DB_name + Slash + Tab_name).c_str(), "rb");
    if (Tab_fp == NULL)
        return FILE_OPEN_FAIL;
    fseek(Tab_fp, PATH_SIZE + VERSION_SIZE + sizeof(Tab_SPACE), SEEK_SET);
    bool has_void = false;
    if (Tab_headler.Rol_void_buf[0] != 0)
        has_void = true;

    int Col_Addr = -1;
    if(Col_name[0]!='*')
    {

        for (int i = 0; i < Tab_headler.Col_num; i++)
        {
            if (!strcmp(Tab_headler.Col[i], Col_name))
            {
                Col_Addr = i;
                break;
            }
        }
        if (Col_Addr == -1)
            return NO_TARGET;

        int data_ptr = 0;
        for (int i = 0; i < Col_Addr; i++)
        {
            switch (Tab_headler.Col_type[i])
            {
                case SQL::Int:
                    data_ptr += sizeof(int);
                    break;
                case SQL::CHAR:
                    data_ptr += CHAR_SIZE;
                    break;
                case SQL::LONG_CHAR:
                    data_ptr += LONG_CHAR_SIZE;
                    break;
                case SQL::FLOAT:
                    data_ptr += sizeof(double);
                default:
                    break;
            }
        }

        if (DB_Header.Index[0])
        {
            /*
            使用索引查找
            */
        }

        int data_page = 0;
        for (int i = 0; i < Tab_headler.Rol_num; i++)
        {
            fread((char *)buf + data_page * Tab_headler.page_size, Tab_headler.page_size, 1, Tab_fp);
            switch (Tab_headler.Col_type[Col_Addr])
            {
                case SQL::Int:
                    if (*(int *)((char *)buf + data_page * Tab_headler.page_size + data_ptr) == *(int *)target)
                        data_page++;
                    break;
                case SQL::CHAR:
                case SQL::LONG_CHAR:
                    if (!strcmp((char *)((char *)buf + data_page * Tab_headler.page_size + data_ptr), (char *)target))
                        data_page++;
                    break;
                case SQL::FLOAT:
                    if (*(double *)((char *)buf + data_page * Tab_headler.page_size + data_ptr) == *(double *)target)
                        data_page++;
                        break;
                default:
                    break;
            }
        }
        fclose(Tab_fp);
        if(data_page==0)
        {
            buf = NULL;
            return NO_TARGET;
        }
        else
            return to_string(data_page);
    }
    
    if(!has_void)
        fread(buf, Tab_headler.page_size, Tab_headler.Rol_num, Tab_fp);
    else
    {
        /*
            有间隙情况
        */
    }
    fclose(Tab_fp);
    return to_string(Tab_headler.Rol_num);
}
