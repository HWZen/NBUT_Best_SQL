#pragma once
#ifndef _SQL_ENGINE
#define _SQL_ENGINE
#include <cstdlib>
#include <cstdio>
#include <string>
using namespace std;

/*******************/
/**�����������궨��**/

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

/**��������**/
/***********/



extern char *PATH;

/*****************/
/**����;��涨��**/
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
/**��������**/
/***********/

// ���˸�SQL�����ռ䣬��Ϊ��Щö�����ƺ�C/C++�ؼ��ֳ�ͻ
namespace SQL
{
    // ָ��ö��
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

    // ��������ö��
    enum DataType_Enum
    {
        Int,
        CHAR,
        LONG_CHAR,
        FLOAT,
        BOOL
    };

    // �����ģʽö��
    enum Mode
    {
        Normal,
        Init,
    };
} // namespace SQL


// ���������ýṹ��
struct SQL_SPACE
{
    char PATH[PATH_SIZE] = {0};
    int DB_Num;
    char DB[MAX_DB_NUM][CHAR_SIZE];
};

// ���ݿ����ýṹ��
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

// �����ýṹ��
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

// ���ݿ�������
class Engine
{
private:
    friend class Manager;
    
    // ����·��
    string path;

    // �����������ļ�ָ��
    FILE *space_fptr;

    // ����������
    SQL_SPACE space_Header;

    // Ŀǰʹ�õ����ݿ�����
    string DB_name;

    // Ŀǰʹ�õ����ݿ�����
    DB_SPACE DB_Header;

    // �ж����ݿ��Ƿ����
    bool findDB(const char *name);

    // �����Ƿ����
    bool findTab(const char *name);

public:
    Engine(const char *use, string &return_buf);

    // ���ù�����
    string InitSpace();

    // �������ݿ�
    string CreatDB(const char *name, const char *group, const char *owner);

    // ������
    string CreatTable(const char *name, int Col_Num, const char **Col, const SQL::DataType_Enum *Col_type);

    // ѡ�����ݿ�
    string use(const char *name);

    // ����
    string insertRol(const char *Tab_name, const void *argv);

    // ��ȡ�����ýṹ��
    string Get_Tab_Header(const char *name, Tab_SPACE &buf);

    // ����
    string serach(const char *Tab_name, const char *Col_name, const void *target, void *buf);

};



#endif