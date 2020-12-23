#include "osplatformutil.h"
#include "sock5_server.h"
#include <iostream>
#include <string.h>
using namespace std;

int main(int argc, char *argv[])
{

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
