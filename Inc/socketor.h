//
// Created by HWZ on 2021/1/19.
//

#ifndef NBUT_BEST_SQL_SOCKETOR_H
#define NBUT_BEST_SQL_SOCKETOR_H

#include <string>
#include "osplatformutil.h"

using namespace std;

#ifdef I_OS_WIN

#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")

#endif

#define MAX_SOCKET_SIZE 4096

#ifdef I_OS_LINUX

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/shm.h>

typedef int SOCKET;
typedef struct sockaddr_in SOCKADDR_IN;
#define closesocket(x) close(x)

#define TRUE 1

#endif

#ifdef I_OS_WIN



#endif

// SOCKET的封装
class socketor
{
private:
    SOCKET socket;
    string Address;
    int Port;

public:
    socketor() = default;

    socketor(SOCKET target_socket, SOCKADDR_IN socket_info);

    // 发送函数
    void Send(const string &str);

    // 接收函数
    string receive();

    void close_connect();

    // socket地址
    inline string address()
    { return Address; };

    // socket端口
    inline int port()
    { return Port; };
};


#endif //NBUT_BEST_SQL_SOCKETOR_H
