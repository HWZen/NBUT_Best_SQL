#include "SQL_engine.h"
#include "osplatformutil.h"
#include "function.h"
#include <cstring>

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

    // �򿪱�
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
        // ��ȡ�����������ļ���������
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

    // ������ݿ��Ƿ����
    if (space_Header.PATH[0] = 0)
    {
        re_buf += NO_SPACE_FILE;
        return re_buf;
    }

    for (int i = 0; i <= space_Header.DB_Num; i++)
    {
        if (i == space_Header.DB_Num)
            return NO_DB_NAME;
        if (!strcmp(space_Header.DB[i], name))
            break;
    }
    // ������

    // ���ݿ����·��
    string Abs_Path = path;

    if (Abs_Path[Abs_Path.length() - 1] != Slash)
        Abs_Path += Slash;
    Abs_Path += name;

    // �����ݿ������ļ�
    FILE *DB_fp = fopen(string(Abs_Path + Slash + "DB.space").c_str(), "rb");
    if (DB_fp == NULL)
        return NO_DB_SPECIFIED;

    DB_name = name;

    // ��������ļ�
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

string Engine::CreatSpace()
{
    // ��ʼ��������
    // ���ã������⹤�����Ƿ��Ѵ���
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

    // ���ݿ����·��
    string Abs_Path = path;

    if (Abs_Path[Abs_Path.length() - 1] != Slash)
        Abs_Path += Slash;
    Abs_Path += name;
    // �������

    // ��鲢�������ݿ�

    // ������ݿ��Ƿ����
    for (int i = 0; i < space_Header.DB_Num; i++)
    {
        if (!strcmp(name, space_Header.DB[i]))
            return TARGET_EXIST;
    }
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
    // �������

    // �������ݿ������ļ�
    FILE *DB_fp = fopen((Abs_Path + Slash + "DB.space").c_str(), "wb+");
    if (DB_fp == NULL)
    {
        re_buf += FILE_CREATE_FAIL;
        return re_buf;
    }

    fwrite(Abs_Path.c_str(), sizeof(char[PATH_SIZE]), 1, DB_fp);
    fwrite(ENGINE_VERSION, sizeof(char[VERSION_SIZE]), 1, DB_fp);

    memcpy(DB_Header.PATH, Abs_Path.c_str(), sizeof(char[PATH_SIZE]));
    memcpy(DB_Header.Group, group, sizeof(char[32]));
    memcpy(DB_Header.Owner, owner, sizeof(char[32]));
    DB_Header.Index_Num = 0;
    DB_Header.Table_Num = 0;
    fwrite(&DB_Header, sizeof(DB_SPACE), 1, DB_fp);
    fclose(DB_fp);
    // �������


    // ���¹����������ļ�
    space_fptr = fopen("SQL.space", "rb+");
    if (space_fptr == NULL)
    {
        re_buf += NO_SPACE_FILE;
        return re_buf;
    }
    fseek(space_fptr, sizeof(char[PATH_SIZE]) + sizeof(char[VERSION_SIZE]), SEEK_SET);
    memcpy(space_Header.DB[space_Header.DB_Num++], name, 32);
    fwrite(&space_Header, sizeof(SQL_SPACE), 1, space_fptr);
    fclose(space_fptr);
    // �������
    return re_buf;
}

string Engine::CreatTable(const char *name, int Col_Num, const char Col[][32], const SQL::DataType_Enum *Col_type)
{
    string re_buf = "";

    // ������ݿ�
    if (DB_name[0] == 0)
        return NO_DB_SPECIFIED;

    // �����Ƿ����
    for (int i = 0; i < DB_Header.Table_Num; i++)
    {
        if (!strcmp(name, DB_Header.Table[i]))
            return TARGET_EXIST;
    }
    // ������

    // ���ݿ����·��
    string Abs_Path = path;
    if (Abs_Path[Abs_Path.length() - 1] != Slash)
        Abs_Path += Slash;
    Abs_Path += DB_name + Slash + name;

    // �������ļ�ͷ
    FILE *Tab_fptr = fopen(Abs_Path.c_str(), "wb+");
    if (Tab_fptr == NULL)
        return FILE_CREATE_FAIL;

    // �����ݿ������ļ�ͷ
    FILE *DB_fp = fopen((DB_name + Slash + "DB.space").c_str(), "rb+");
    if (DB_fp == NULL)
        return NO_DB_SPECIFIED;

    // ��ʼ������
    fwrite(Abs_Path.c_str(), sizeof(char[PATH_SIZE]), 1, Tab_fptr);
    fwrite(ENGINE_VERSION, sizeof(char[VERSION_SIZE]), 1, Tab_fptr);

    Tab_SPACE Header;
    strcpy(Header.PATH, Abs_Path.c_str());
    Header.Col_num = Col_Num;
    memcpy(Header.Col, Col, Col_Num * sizeof(char[32]));
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
    // �������

    // �������ݿ������ļ�
    fseek(DB_fp, sizeof(char[PATH_SIZE]) + sizeof(char[VERSION_SIZE]), SEEK_SET);
    memcpy(DB_Header.Table[DB_Header.Table_Num++], name, sizeof(char[32]));
    fwrite(&DB_Header, sizeof(DB_SPACE), 1, DB_fp);
    fclose(DB_fp);
    // �������

    return re_buf;
}

string Engine::insertRol(const char *Tab_name, void **argv)
{
    string re_buf;
    // ������ݿ�
    if (DB_name[0] == 0)
        return NO_DB_SPECIFIED;

    // �����Ƿ����
    for (int i = 0; i <= DB_Header.Table_Num; i++)
    {
        if (i == DB_Header.Table_Num)
            return NO_TARGET;
        if (!strcmp(Tab_name, DB_Header.Table[i]))
            break;
    }
    // ������

    // ���ݿ����·��
    string Abs_Path = path;
    if (Abs_Path[Abs_Path.length() - 1] != Slash)
        Abs_Path += Slash;
    Abs_Path += DB_name + Slash + Tab_name;

    // �򿪱��ļ�
    FILE *Tab_fptr = fopen(Abs_Path.c_str(), "rb+");
    if (Tab_fptr == NULL)
        return FILE_CREATE_FAIL;

    // �������
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

    // ��λ����λ��
    if (tab_header.Rol_void_buf[0] != 0)
    {
        /*
            ��ɾ���Ŀ�϶����
        */
    }
    else
        fseek(Tab_fptr, 0, SEEK_END);

    if (DB_Header.Index[0])
    {
        /*
            ���������
        */
    }
    fwrite(argv, tab_header.page_size, 1, Tab_fptr);

    tab_header.Rol_num++;
    fwrite(&tab_header, sizeof(Tab_SPACE), 1, Tab_fptr);

    fclose(Tab_fptr);

    return re_buf;
}
