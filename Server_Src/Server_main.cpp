#include "osplatformutil.h"
#include "sock5_server.h"
#include <iostream>
#include <cstring>
using namespace std;

char *PATH;

void getPath(const char *argv);

int main(int argc, char *argv[])
{
    PATH = argv[0];
    cout << "NBUT Best SQL is running." << endl;
    if (argc == 2)
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
        Server server;
        server.Listen();
    }

    return 0;
}

void getPath(const char *argv)
{
    PATH = new char[256];
    memcpy(PATH, argv, sizeof(argv));

    for (int i = sizeof(PATH); i > 0; i--)
    {
        if(PATH[i-1]=='\\')
        {
            PATH[i] = '\0';
            break;
        }
    }
}



