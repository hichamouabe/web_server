#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <vector>
#include <utility>

bool	isNumeric(const std::string& s);

unsigned long	parseSize(const std::string& value);

std::pair<std::string, int> parseListen(const std::string& value);

#endif
