#include "command_manager.h"

Manager::Manager(const char Usr[],const char password[])
{
    user = Usr;
    psword = password;
}

Manager::~Manager()
{

}

string Manager::command(const char com[])
{
    if(stricmp(com,"exit")==0)
        return string("bye.");
    else
        return string("received");
}