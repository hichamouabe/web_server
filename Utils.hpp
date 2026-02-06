#ifndef UTILS_HPP
#define UTILS_HPP

#include <exception>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <netdb.h>
#include <cstring>


void parseListenValue(const std::string& vl);
bool isNumeric(const std::string &s);
void parse_http_code(const std::string& nb);
void parseCbmz(const std::string& tokens);

#endif