#pragma once
#include <stdlib.h>
class Engine
{
private:
    friend class Manager;
    
    char path[128];

    void *select_SQL;

    FILE file_ptr;

    class Cache;

};