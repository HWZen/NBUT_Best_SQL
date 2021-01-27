//
// Created by HWZ on 2021/1/19.
//

#include "socketor.h"
#include <string.h>

#ifdef I_OS_LINUX

#include <time.h>

void Sleep(int ms)
{
    struct timeval delay;
    delay.tv_sec = 0;
    delay.tv_usec = ms * 1000; // 20 ms
    select(0, NULL, NULL, NULL, &delay);
}

#endif


socketor::socketor(SOCKET target_socket, SOCKADDR_IN socket_info)
{
    socket = target_socket;
    Address = inet_ntoa(socket_info.sin_addr);
    Port = ntohs(socket_info.sin_port);
}

void socketor::Send(const string &str)
{
    send(socket, str.c_str(), str.length(), 0);
    Sleep(100);
}

string socketor::receive()
{
    char buf[MAX_SOCKET_SIZE];
    int ret = recv(socket, buf, MAX_SOCKET_SIZE, 0);
    if (ret < 0)
        return "lose connect";
    buf[ret] = '\0';
    return buf;
}

void socketor::close_connect()
{
    closesocket(socket);
}