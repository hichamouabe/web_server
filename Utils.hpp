#ifndef UTILS_HPP
#define UTILS_HPP

#include <exception>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <netdb.h>
#include <cstring>
#include <utility>

void parseListenValue(const std::string& vl);
bool isNumeric(const std::string &s);
void parse_http_code(const std::string& nb);
void parseCbmz(const std::string& tokens);
unsigned long parseSize(const std::string& value);
std::pair<std::string, int> parseListen(const std::string& value);
#endif
