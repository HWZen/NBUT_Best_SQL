#include "sock5_server.h"

using namespace std;

void Server::Listen()
{
    bind(sListen, (struct sockaddr *)&local, sizeof(SOCKADDR_IN));

    printf("listening. port:%d \n", DEFAULT_PORT);
    listen(sListen, 10);

    int cnt = 0;
    acceptThr[cnt] = new thread(&Server::Accept, this, ref(sClient[cnt]));
    acceptThr[cnt]->join();
    recThr[cnt] = new thread(&Server::Recevie, this, ref(sClient[cnt]));
    recThr[cnt]->detach();
    while (cnt++ < 8)
    {
        acceptThr[cnt] = new thread(&Server::Accept, this, ref(sClient[cnt]));
        acceptThr[cnt]->join();
        recThr[cnt] = new thread(Recevie, ref(sClient[cnt]));
        recThr[cnt]->detach();
    }
}

void Server::Accept(SOCKET &sClient)
{

}

char *Server::Receice()
{
    char *t = new char[10];
    return t;
}