#include "../../include/Client.hpp"
#include <iostream>
#include <sstream>

Client::Client(int fd, int port) 
    : _fd(fd), _server_port(port), _state(STATE_READ_REQUEST), 
      _content_length(0), _file_size(0), _bytes_sent(0), _headers_sent(false) {}

Client::~Client() {
    if (_file_stream.is_open()) {
        _file_stream.close();
    }
}

// --- Getters & Setters ---
int Client::getFd() const { return _fd; }
int Client::getPort() const { return _server_port; }
ClientState Client::getState() const { return _state; }
void Client::setState(ClientState s) { _state = s; }

std::string& Client::recvBuffer() { return _recv_buffer; }
std::string& Client::sendBuffer() { return _send_buffer; }

// --- Request Parsing Helpers ---
void Client::parseRequestLine(const std::string& line) {
    std::stringstream ss(line);
    ss >> _method >> _path >> _version;

    // Extract Query String (e.g., /page?id=5)
    size_t qPos = _path.find('?');
    if (qPos != std::string::npos) {
        _query = _path.substr(qPos + 1);
        _path = _path.substr(0, qPos);
    }
}

void Client::addHeader(const std::string& key, const std::string& value) {
    _headers[key] = value;
    if (key == "Content-Length") {
        _content_length = std::atoi(value.c_str());
    }
}

std::string Client::getMethod() const { return _method; }
std::string Client::getPath() const { return _path; }

bool Client::hasHeader(const std::string& key) const {
    return _headers.find(key) != _headers.end();
}

std::string Client::getHeader(const std::string& key) const {
    if (hasHeader(key)) return _headers.at(key);
    return "";
}

// --- Response Helpers ---
void Client::openFile(const std::string& path) {
    if (_file_stream.is_open()) _file_stream.close();
    _file_stream.open(path.c_str(), std::ios::binary);
}

bool Client::isFileOpen() { return _file_stream.is_open(); }
std::ifstream& Client::fileStream() { return _file_stream; }

void Client::setFileSize(size_t size) { _file_size = size; }
size_t Client::getFileSize() const { return _file_size; }
size_t Client::getBytesSent() const { return _bytes_sent; }
void Client::addBytesSent(size_t bytes) { _bytes_sent += bytes; }

bool Client::isHeadersSent() const { return _headers_sent; }
void Client::setHeadersSent(bool sent) { _headers_sent = sent; }

// --- Reset for Keep-Alive ---
void Client::reset() {
    _state = STATE_READ_REQUEST;
    _recv_buffer.clear();
    _send_buffer.clear();
    _method.clear();
    _path.clear();
    _query.clear();
    _headers.clear();
    _content_length = 0;
    
    if (_file_stream.is_open()) _file_stream.close();
    _file_size = 0;
    _bytes_sent = 0;
    _headers_sent = false;
}
