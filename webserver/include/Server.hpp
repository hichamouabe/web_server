#ifndef SERVER_HPP
#define SERVER_HPP

#include <vector>
#include <map>
#include <string>
#include <sys/epoll.h> // Linux only. Use kqueue for Mac, or select/poll if portable.
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h> // sockaddr_in
#include <arpa/inet.h>  // inet_addr

#include "ConfigLoader.hpp"
#include "Client.hpp"

#define MAX_EVENTS 1024
#define RECV_CHUNK_SIZE 8192

class Server {
private:
    // --- CONFIGURATION ---
    std::vector<ServerConfig>   _configs;
    
    // --- NETWORK RESOURCES ---
    int                         _epoll_fd;
    std::vector<int>            _listen_fds;    // One socket per port (80, 8080)
    std::map<int, Client*>      _clients;       // Active connections (fd -> Client*)

    // --- HELPER FUNCTIONS ---
    int     createListenSocket(int port, const std::string& host);
    void    setNonBlocking(int fd);
    void    addToEpoll(int fd, uint32_t events);
    void    removeFromEpoll(int fd);
    bool    isListenSocket(int fd);

    // --- REQUEST HANDLING ---
    void    acceptNewClient(int listen_fd);
    void    handleRead(int client_fd);
    void    handleWrite(int client_fd);
    void    closeClient(int client_fd);
    
    // --- LOGIC DISPATCHER ---
    void    processRequest(Client* client); // Where the magic happens (GET/POST logic)
    // logic added to the server(the linker between the final server objects created and the server it-self)
    const ServerConfig&     getServerConfig(int port, const std::string& host);
    const LocationConfig&   getLocationConfig(const ServerConfig& server, const std::string& path);
    std::string             generateErrorPage(int code, const ServerConfig& server);
public:
    Server(const std::vector<ServerConfig>& configs);
    ~Server();

    void    run(); // The Infinite Loop
};

#endif
