#ifndef CPPSERVER_UTIL_H
#define CPPSERVER_UTIL_H

#include <iostream>

bool startsWith(std::string s, std::string pattern);

std::vector<std::string> split(std::string const &s, char delim);

#endif //CPPSERVER_UTIL_H
