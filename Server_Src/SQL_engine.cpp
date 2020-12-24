#include "SQL_engine.h"
#include "osplatformutil.h"
#include <cstring>

#ifdef I_OS_WIN
#include <direct.h>
#include <io.h>
#define CreatDir(x) mkdir(x)
#endif

#ifdef I_OS_LINUX
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#define CreatDir(x) mkdir(x, S_IRWXU)
#endif

using namespace std;

Engine::Engine(const char *use_name, string &return_buf)
{
    return_buf.clear();
    path = PATH;
    space_fptr = fopen("SQL.space", "rb");
    if (feof(space_fptr))
    {
        //error: load SQL.space fail
        return_buf += NO_SPACE_FILE;
        DB_name[0] = 0;
        return;
    }
    else
    {
        char space_path[PATH_SIZE];
        fread(space_path, sizeof(space_path), 1, space_fptr);

        if(strcmp(space_path,path.c_str())!=0)
            //waring: space may has been moved.
            return_buf += SPACE_PATH_WARN;

        char space_version[VERSION_SIZE];
        fread(space_version, sizeof(space_version), 1, space_fptr);

        if(strcmp(space_version,ENGINE_VERSION)!= 0)
            //waring: space engine version is different from now
            return_buf += SPACE_VERSION_WARN;

        fread(&space_Header, sizeof(SQL_SPACE), 1, space_fptr);

        for (int i = 0; i < space_Header.DB_NUm;i++)
        {
            if(strcmp(space_Header.DB[i],use_name)==0)
            {
                DB_name = use_name;
                fclose(space_fptr);
                return;
            }
        }
        //error: can't find DB
        return_buf += NO_DB_NAME;
        fclose(space_fptr);
        return;
    }
}

string Engine::use(const char *name)
{
    string return_buf = "";
    if(space_Header.PATH[0]=0)
    {
        return_buf += "e 0";
        return return_buf;
    }
    return return_buf;
}

string Engine::CreatSpace()
{
    string return_buf = "";

    if(space_fptr!=NULL)
        fclose(space_fptr);

    space_fptr = fopen("SQL.space", "wb+");
    if(space_fptr=NULL)
    {
        return_buf += FILE_CREATE_FAIL;
        return return_buf;
    }

    fwrite(PATH, sizeof(char[PATH_SIZE]), 1, space_fptr);
    fwrite(ENGINE_VERSION, sizeof(ENGINE_VERSION), 1, space_fptr);

    memcpy(space_Header.PATH, PATH, sizeof(char[PATH_SIZE]));
    space_Header.DB_NUm = 0;
    fwrite(&space_Header, sizeof(SQL_SPACE), 1, space_fptr);

    fclose(space_fptr);

    return return_buf;
}

string Engine::CreatDB(const char *name, const char *group, const char *owner)
{
    string return_buf = "";

    // 数据库绝对路径
    string Abs_Path = path;
    if(Abs_Path[Abs_Path.length()-1]!='\\')
        Abs_Path += '\\';
    Abs_Path += name;
    // 生成完毕

    // 检查并创建数据库
    if(access(Abs_Path.c_str(),0)==-1)
    {
        if(!CreatDir(Abs_Path.c_str()))
        {
            return_buf += FILE_CREATE_FAIL;
            return return_buf;
        }       
    }
    else
    {
        return_buf += FILE_CREATE_FAIL;
        return return_buf;
    }
    // 创建完毕

    // 创建数据库配置文件
    FILE *DB_fp = fopen((Abs_Path + "\\DB.space").c_str(), "wb+");
    if(DB_fp==NULL)
    {
        return_buf += "e 2";
        return return_buf;
    }

    fwrite(Abs_Path.c_str(), sizeof(char[PATH_SIZE]), 1, DB_fp);
    fwrite(ENGINE_VERSION, sizeof(char[VERSION_SIZE]), 1, DB_fp);

    memcpy(DB_Header.PATH, Abs_Path.c_str(), sizeof(char[PATH_SIZE]));
    memcpy(DB_Header.Group, group, sizeof(char[32]));
    memcpy(DB_Header.Owner, owner, sizeof(char[32]));
    DB_Header.Index_Num = 0;
    DB_Header.Table_Num = 0;
    fwrite(&DB_Header, sizeof(DB_SPACE), 1, DB_fp);
    // 创建完毕


    // 更新工作区配置文件
    space_fptr = fopen("SQL.space", "rb+");
    if(space_fptr==NULL)
    {
        return_buf += NO_SPACE_FILE;
        return return_buf;
    }
    fseek(space_fptr, sizeof(char[PATH_SIZE]) + sizeof(char[VERSION_SIZE]), SEEK_SET);
    memcpy(space_Header.DB[++space_Header.DB_NUm], name, 32);
    // 更新完毕
    return return_buf;
}