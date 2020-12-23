#include "command_manager.h"
#include <stdio.h>
#include <string.h>
using namespace std;

Manager::Manager(const char Usr[], const char password[])
{
    user = Usr;
    psword = password;
}

Manager::~Manager()
{
}

string Manager::command(const char com[])
{
    if (strcmp(com, "exit") == 0)
        return string("bye.");
    else
        return string("received");
}