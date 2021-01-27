#pragma once
#ifndef _FUNCTION_
#define _FUNCTION_

#include <string>
#include <vector>

// C style
std::vector<std::string> String2argv_argc(char *str, int &argc, char **argv);
// C++ style
std::vector<std::string> String2argv_argc(const std::string& str);

#endif