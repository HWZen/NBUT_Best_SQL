#include "osplatformutil.h"
#include "sock5_server.h"
#include <iostream>
#include <cstring>
#include <cstdlib>

#ifdef I_OS_LINUX
#include <limits.h>
#endif

using namespace std;

char *PATH;

void getPath(const char *argv);

int main(int argc, char *argv[])
{
    getPath(argv[0]);
    cout << "Path: " << PATH << endl;
    cout << "NBUT Best SQL is running." << endl;
    if (argc ==3)
    {
        if (strcmp(argv[1], "-p") == 0 || strcmp(argv[1], "--port") == 0)
        {
            Server server(atoi(argv[2]));
            server.Listen();
        }
        else
        {
            cout << "inviable parameter: " << argv[1] << endl;
            exit(0);
        }
    }
    else
    {
        cout << "no parameter or inviable parameter." << endl;
        Server server;
        server.Listen();
    }

    return 0;
}

void getPath(const char *argv)
{
    PATH = new char[256];
    memcpy(PATH, argv, strlen(argv));

    for (int i = strlen(PATH); i > 0; i--)
    {
#ifdef I_OS_WIN
        if(PATH[i-1]=='\\')
#endif
#ifdef I_OS_LINUX
        if(PATH[i-1]=='/')
#endif
        {
            PATH[i] = '\0';
            break;
        }
    }

//#ifdef I_OS_LINUX
//    if(!realpath("server",PATH))
//    {
//        cerr<<"can't load path"<<endl;
//        exit(0);
//    }
//
//#endif

}



