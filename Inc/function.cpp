#include "function.h"
#include <vector>
#include <string>

// C style
int def_argc;
std::vector<std::string> String2argv_argc(char *str, int &argc = def_argc, char **argv = nullptr)
{
    argc = 0;
    if (argv == nullptr)
        argv = new char *[16];
    std::vector<std::string> parameter;
    for (int i = 0; str[i] != '\0'; i++)
    {
        if (str[i] == ' ')
            str[i] = '\0';
        else
        {
            if (i != 0)
            {
                if (str[i - 1] == '\0')
                {
                    argc++;
                    argv[argc - 1] = str + i;
                }
            } else
            {
                argc++;
                argv[argc - 1] = str + i;
            }
        }

        if (argc >= 16)
            break;
    }
    parameter.reserve(argc);
    for (int i = 0; i < argc; i++)
        parameter.emplace_back(std::string(argv[i]));
    return parameter;
}

using namespace std;

// c++ style
vector<string> String2argv_argc(const string& str)
{
    char sign = ' ';
    vector<string> res;
    string temp;
    for(char ch:str)
    {
        if(ch!=sign)
        {
            if(temp.empty()&&(ch=='\''||ch=='\"'))
                sign = ch;
            else
                temp.push_back(ch);
        }
        else
        {
            if (!temp.empty())
            {
                res.emplace_back(temp);
                ch = ' ';
                temp.clear();
            }
        }
    }
    if (!temp.empty())
        res.emplace_back(temp);
    return res;
}