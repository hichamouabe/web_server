/* --- FILE: src/utils/Utils.cpp --- */
#include "../../include/Utils.hpp"
#include <cstdlib>
#include <stdexcept>
#include <cstring>      // For memset
#include <sys/types.h>  // For getaddrinfo
#include <sys/socket.h> // For getaddrinfo
#include <netdb.h>      // For getaddrinfo

bool isNumeric(const std::string& s) {
    if (s.empty()) return false;
    for (size_t i = 0; i < s.size(); ++i) {
        if (!isdigit(s[i])) return false;
    }
    return true;
}

void parseListenValue(const std::string& vl) {
    std::string ip;
    std::string port_str;

    size_t pos = vl.find(':');
    if (pos == std::string::npos) {
        if (isNumeric(vl)) {
            ip = "0.0.0.0";
            port_str = vl;
        } else {
            ip = vl;
            port_str = "80";
        }
    } else {
        ip = vl.substr(0, pos);
        port_str = vl.substr(pos + 1);
        if (ip.empty() || port_str.empty()) 
            throw std::runtime_error("Invalid listen format: '" + vl + "'");
    }

    if (ip == "localhost") ip = "127.0.0.1";

    struct addrinfo hints, *res;
    std::memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    int ret = getaddrinfo(ip.c_str(), NULL, &hints, &res);
    if (ret != 0) 
        throw std::runtime_error("Invalid host: " + vl);
    if (res) freeaddrinfo(res);

    if (!isNumeric(port_str)) 
        throw std::runtime_error("Listen port must be numeric: '" + port_str + "'");

    int port = std::atoi(port_str.c_str());
    if (port < 1 || port > 65535) 
        throw std::runtime_error("Listen port out of range: '" + port_str + "'");
}

void parse_http_code(const std::string& nb) {
    if (isNumeric(nb)) {
        int code = std::atoi(nb.c_str());
        if (code < 100 || code > 599 || nb.size() > 3) 
            throw std::runtime_error("HTTP code out of range: '" + nb + "'");
    } else {
        throw std::runtime_error("HTTP code must be numeric: '" + nb + "'");
    }
}

void parseCbmz(const std::string& vl) {
    if (vl.empty()) throw std::runtime_error("client_max_body_size: empty");
    char last = vl[vl.size() - 1];
    std::string nb = vl;
    if (!isdigit(last)) nb = vl.substr(0, vl.size() - 1);
    if (!isNumeric(nb)) throw std::runtime_error("Invalid body size: '" + vl + "'");
}

unsigned long parseSize(const std::string& value) {
    if (value.empty()) return 0;
    char unit = value[value.size() - 1];
    std::string numberPart = value;
    unsigned long multiplier = 1;

    if (!isdigit(unit)) {
        numberPart = value.substr(0, value.size() - 1);
        if (unit == 'k' || unit == 'K') multiplier = 1024;
        else if (unit == 'm' || unit == 'M') multiplier = 1024 * 1024;
        else if (unit == 'g' || unit == 'G') multiplier = 1024 * 1024 * 1024;
        else throw std::runtime_error("Invalid size unit: " + value);
    }
    return static_cast<unsigned long>(std::atol(numberPart.c_str())) * multiplier;
}

std::pair<std::string, int> parseListen(const std::string& value) {
    size_t colonPos = value.find(':');
    if (colonPos == std::string::npos) {
        return std::make_pair("0.0.0.0", std::atoi(value.c_str()));
    }
    std::string host = value.substr(0, colonPos);
    std::string portStr = value.substr(colonPos + 1);
    return std::make_pair(host, std::atoi(portStr.c_str()));
}
