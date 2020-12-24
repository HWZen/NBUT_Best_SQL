#include "function.h"
#include <string.h>

void String2argv_argc(char *str, int &argc, char **argv)
{
    argc = 0;
    argv = new char*[16];
    for (int i = 0; i < strlen(str);i++)
    {
        if(str[i]==' ')
            str[i] = '\0';    
        else if(str[i] >= 0x21&&str[i]<=0x7e)
        {
            if(i!=0)
            {
                if(str[i-1]='\0')
                {
                    argc++;
                    argv[argc - 1] = str + i;
                }

            }
            else
            {
                argc++;
                argv[argc - 1] = str + i;
            }
        }

        if(argc>=8)
            break;
    }
}