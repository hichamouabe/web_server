#include "../../include/Server.hpp"
#include <iostream>
#include <cstring>
#include <algorithm> // for std::find
#include <sstream>  // Add this for std::stringstream
#include <fstream>  // Add this for std::ifstream
Server::Server(const std::vector<ServerConfig>& configs) : _configs(configs) {
    // 1. Create Epoll Instance
    _epoll_fd = epoll_create1(0);
    if (_epoll_fd == -1) {
        throw std::runtime_error("Epoll Create Failed");
    }

    // 2. Setup Listening Sockets based on Config
    for (size_t i = 0; i < _configs.size(); ++i) {
        int port = _configs[i].port;
        std::string host = _configs[i].host;
        
        // Check duplicate ports (simple check)
        bool port_exists = false;
        for (size_t j = 0; j < i; ++j) {
            if (_configs[j].port == port) {
                port_exists = true; 
                break;
            }
        }
        if (port_exists) continue; // Skip binding same port twice

        int fd = createListenSocket(port, host);
        _listen_fds.push_back(fd);
        addToEpoll(fd, EPOLLIN); // Listen for connections
        std::cout << "Server Listening on " << host << ":" << port << " (fd: " << fd << ")" << std::endl;
    }
}

Server::~Server() {
    // Clean up Listen Sockets
    for (size_t i = 0; i < _listen_fds.size(); ++i) {
        close(_listen_fds[i]);
    }
    // Clean up Clients
    for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
        delete it->second;
        close(it->first);
    }
    close(_epoll_fd);
}

// --- HELPER: Create Socket ---
int Server::createListenSocket(int port, const std::string& host) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) throw std::runtime_error("Socket Create Failed");

    int opt = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(host.c_str());
    addr.sin_port = htons(port);

    if (bind(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        close(fd);
        throw std::runtime_error("Bind Failed");
    }

    if (listen(fd, SOMAXCONN) == -1) {
        close(fd);
        throw std::runtime_error("Listen Failed");
    }

    setNonBlocking(fd);
    return fd;
}

void Server::setNonBlocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

void Server::addToEpoll(int fd, uint32_t events) {
    struct epoll_event event;
    event.events = events;
    event.data.fd = fd;
    if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, fd, &event) == -1) {
        throw std::runtime_error("Epoll Add Failed");
    }
}

void Server::removeFromEpoll(int fd) {
    epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, fd, NULL);
}

bool Server::isListenSocket(int fd) {
    return std::find(_listen_fds.begin(), _listen_fds.end(), fd) != _listen_fds.end();
}

// --- THE MAIN LOOP ---
void Server::run() {
    struct epoll_event events[MAX_EVENTS];

    while (true) {
        int n = epoll_wait(_epoll_fd, events, MAX_EVENTS, -1); // Block forever
        
        for (int i = 0; i < n; ++i) {
            int fd = events[i].data.fd;
            uint32_t event_type = events[i].events;

            if (isListenSocket(fd)) {
                acceptNewClient(fd);
            }
            else {
                if (event_type & EPOLLIN) {
                    handleRead(fd);
                }
                if (event_type & EPOLLOUT) {
                    handleWrite(fd);
                }
                if (event_type & (EPOLLHUP | EPOLLERR)) {
                    closeClient(fd);
                }
            }
        }
    }
}
void Server::processRequest(Client* client) {
    if (client->recvBuffer().find("\r\n\r\n") == std::string::npos) return;

    // 1. Parse Request
    size_t end_line = client->recvBuffer().find("\r\n");
    std::string request_line = client->recvBuffer().substr(0, end_line);
    client->parseRequestLine(request_line);

    std::string host = client->getHeader("Host");
    size_t colon = host.find(':');
    if (colon != std::string::npos) host = host.substr(0, colon);

    const ServerConfig& serverConf = getServerConfig(client->getPort(), host);
    const LocationConfig& locConf = getLocationConfig(serverConf, client->getPath());

    std::string fullPath = locConf.root + client->getPath();

    // Handle Directory Index
    if (fullPath[fullPath.size() - 1] == '/') {
        if (!locConf.index.empty()) {
            fullPath += locConf.index;
        }
    }

    std::cout << "Request: " << client->getPath() << " -> Serving: " << fullPath << std::endl;

    std::ifstream file(fullPath.c_str());
    if (file.good()) {
        file.close();
        client->openFile(fullPath);
        client->setState(STATE_WRITE_RESPONSE);
    } else {
        // 404 Error
        std::string body = "<html><body><h1>404 Not Found</h1></body></html>";
        std::stringstream ss;
        ss << "HTTP/1.1 404 Not Found\r\n"
           << "Content-Length: " << body.size() << "\r\n"
           << "Content-Type: text/html\r\n"
           << "Connection: close\r\n\r\n"
           << body;
        client->sendBuffer() = ss.str();
        client->setState(STATE_WRITE_RESPONSE);
    }

    // --- CRITICAL FIX START ---
    // We must tell epoll we are ready to WRITE now.
    struct epoll_event event;
    event.events = EPOLLOUT | EPOLLIN; // Monitor for Write AND Read
    event.data.fd = client->getFd();
    if (epoll_ctl(_epoll_fd, EPOLL_CTL_MOD, client->getFd(), &event) == -1) {
        std::cerr << "Error: Failed to modify epoll event for writing" << std::endl;
        closeClient(client->getFd());
    }
    // --- CRITICAL FIX END ---
}
// --- NETWORK EVENTS ---
void Server::acceptNewClient(int listen_fd) {
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    
    // Accept connection
    int client_fd = accept(listen_fd, (struct sockaddr*)&client_addr, &client_len);
    if (client_fd == -1) return;

    setNonBlocking(client_fd);
    
    // Find which port this client connected to (so we can find the right config later)
    struct sockaddr_in server_addr;
    socklen_t server_len = sizeof(server_addr);
    getsockname(listen_fd, (struct sockaddr*)&server_addr, &server_len);
    int port = ntohs(server_addr.sin_port);

    // Create Client Object
    Client* client = new Client(client_fd, port);
    _clients[client_fd] = client;
    
    // Monitor for READING first (Request)
    addToEpoll(client_fd, EPOLLIN);
    std::cout << "New Connection: fd " << client_fd << " on port " << port << std::endl;
}

void Server::handleRead(int client_fd) {
    char buffer[RECV_CHUNK_SIZE];
    ssize_t bytes_read = recv(client_fd, buffer, RECV_CHUNK_SIZE, 0);

    if (bytes_read <= 0) {
        closeClient(client_fd);
        return;
    }

    Client* client = _clients[client_fd];
    client->recvBuffer().append(buffer, bytes_read);

    // Call Process Logic (Parse Request -> Prepare Response)
    processRequest(client);
}


void Server::handleWrite(int client_fd) {
    Client* client = _clients[client_fd];

    // 1. Prepare Headers if file is open and headers not sent
    if (client->isFileOpen() && !client->isHeadersSent()) {
        client->fileStream().seekg(0, std::ios::end);
        size_t fileSize = client->fileStream().tellg();
        client->fileStream().seekg(0, std::ios::beg);

        std::stringstream ss;
        ss << "HTTP/1.1 200 OK\r\n"
           << "Content-Length: " << fileSize << "\r\n"
           << "Content-Type: text/html\r\n"
           << "Connection: close\r\n\r\n"; // Using "close" to keep it simple for now
        
        client->sendBuffer().insert(0, ss.str());
        client->setHeadersSent(true);
    }

    // 2. Read File Data
    if (client->isFileOpen() && client->sendBuffer().size() < RECV_CHUNK_SIZE) {
        char fileBuf[RECV_CHUNK_SIZE];
        client->fileStream().read(fileBuf, RECV_CHUNK_SIZE);
        std::streamsize bytesRead = client->fileStream().gcount();
        if (bytesRead > 0) client->sendBuffer().append(fileBuf, bytesRead);
        if (client->fileStream().eof()) client->fileStream().close();
    }

    std::string& data = client->sendBuffer();
    if (data.empty()) return;

    // 3. Send Data
    ssize_t bytes_sent = send(client_fd, data.c_str(), data.size(), 0);
    if (bytes_sent == -1) return;

    data.erase(0, bytes_sent);

    // 4. Cleanup
    // If buffer is empty AND (file is closed OR we never opened one aka 404)
    if (data.empty() && !client->isFileOpen()) {
        closeClient(client_fd); 
        // Note: For Keep-Alive, instead of closing, we would reset() the client 
        // and use epoll_ctl to switch back to EPOLLIN only.
    }
}

/////////////////////////////////////////////

void Server::closeClient(int client_fd) {
    if (_clients.find(client_fd) == _clients.end()) return;
    
    removeFromEpoll(client_fd);
    close(client_fd);
    delete _clients[client_fd];
    _clients.erase(client_fd);
    std::cout << "Closed Connection: fd " << client_fd << std::endl;
}



// THE LOGIC ADDED 
//




const ServerConfig& Server::getServerConfig(int port, const std::string& host) {
    // 1. Find all servers listening on this port
    std::vector<const ServerConfig*> port_matches;
    for (size_t i = 0; i < _configs.size(); ++i) {
        if (_configs[i].port == port) {
            port_matches.push_back(&_configs[i]);
        }
    }

    // 2. Try to match 'Host' header against 'server_name'
    for (size_t i = 0; i < port_matches.size(); ++i) {
        const std::vector<std::string>& names = port_matches[i]->server_names;
        for (size_t j = 0; j < names.size(); ++j) {
            if (names[j] == host) {
                return *port_matches[i]; // Exact match found!
            }
        }
    }

    // 3. Fallback: Return the first server for this port (Default)
    return *port_matches[0];
}




const LocationConfig& Server::getLocationConfig(const ServerConfig& server, const std::string& path) {
    const LocationConfig* best_match = NULL;
    size_t best_length = 0;

    for (size_t i = 0; i < server.locations.size(); ++i) {
        const std::string& loc_path = server.locations[i].path;
        
        // Check if Request Path starts with Location Path
        if (path.compare(0, loc_path.length(), loc_path) == 0) {
            
            // We found a match! Is it better (longer) than the previous one?
            // Special case: ensure strict matching for folders (/image vs /images)
            if (loc_path.length() > best_length) {
                best_match = &server.locations[i];
                best_length = loc_path.length();
            }
        }
    }
    
    // Should never return NULL because ConfigLoader ensures a default "/" location exists
    return *best_match; 
}




