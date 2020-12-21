#include "osplatformutil.h"
#include "sock5_server.h"
#ifdef I_OS_WIN
using namespace std;

void Server::Listen()
{
    bind(sListen, (struct sockaddr *)&local, sizeof(SOCKADDR_IN));

    printf("listening. port:%d \n", DEFAULT_PORT);
    listen(sListen, 10);

    int cnt = 0;
    acceptThr[cnt] = new thread(&Server::Accept, this, ref(sClient[cnt]));
    acceptThr[cnt]->join();
    recThr[cnt] = new thread(&Server::Receice, this, ref(sClient[cnt]));
    recThr[cnt]->detach();
    while (cnt++ < 8)
    {
        acceptThr[cnt] = new thread(&Server::Accept, this, ref(sClient[cnt]));
        acceptThr[cnt]->join();
        recThr[cnt] = new thread(&Server::Receice, this, ref(sClient[cnt]));
        recThr[cnt]->detach();
    }
}

void Server::Accept(SOCKET &sClient)
{

}

char *Server::Receice(SOCKET &sClient)
{
    char *t = new char[10];
    return t;
}
#endif

#ifdef I_OS_LINUX


#endif