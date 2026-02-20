#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <vector>
#include <utility>
// --- VALIDATION HELPERS (Used by ConfigValidator) ---
void parseListenValue(const std::string& vl);       // Validates "80" or "127.0.0.1:80"
void parse_http_code(const std::string& nb);        // Validates "404", "500"
void parseCbmz(const std::string& vl);              // Validates format "10m", "1k"
bool	isNumeric(const std::string& s);

unsigned long	parseSize(const std::string& value);

std::pair<std::string, int> parseListen(const std::string& value);

#endif
