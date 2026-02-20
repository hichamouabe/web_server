#include "../include/Lexer.hpp"
#include "../include/Parser.hpp"
#include "../include/ConfigValidator.hpp"
#include "../include/ConfigLoader.hpp"
#include "../include/Server.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <csignal>
#include <cstdlib>

// Global pointer allows the signal handler to access the server
Server* g_server = NULL;

void signalHandler(int signum) {
    std::cout << "\n[Signal " << signum << "] Shutting down server..." << std::endl;
    if (g_server) {
        delete g_server; // Triggers ~Server() which closes sockets
        g_server = NULL;
    }
    exit(signum);
}

std::string readFile(const char* filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Error: Could not open file " + std::string(filename));
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

int main(int argc, char** argv) {
    // 1. Validate Arguments
    if (argc != 2) {
        std::cerr << "Usage: ./webserv <config_file>" << std::endl;
        return 1;
    }

    // 2. Setup Signal Handling (Ctrl+C)
    signal(SIGINT, signalHandler);

    ConfigNode* root = NULL;

    try {
        std::cout << "--- 1. Loading Configuration ---" << std::endl;
        std::string configContent = readFile(argv[1]);

        // A. Tokenize
        Lexer lexer(configContent);
        std::vector<Token> tokens = lexer.tokenize();

        // B. Parse
        Parser parser(tokens);
        root = parser.parse();

        // C. Validate
        ConfigValidator validator;
        validator.validate(root);

        // D. Load into Structs
        ConfigLoader loader;
        std::vector<ServerConfig> configs = loader.loadServers(root);
        
        // Cleanup the Tree (We have the structs now)
        delete root; 
        root = NULL;

        std::cout << "--- 2. Starting Server ---" << std::endl;
        
        // E. Start Network Engine
        g_server = new Server(configs);
        g_server->run(); // Infinite Loop

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        if (root) delete root;
        if (g_server) delete g_server;
        return 1;
    }

    return 0;
}
