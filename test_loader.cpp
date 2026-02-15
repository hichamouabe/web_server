#include "Lexer.hpp"
#include "Parser.hpp"
#include "ConfigValidator.hpp"
#include "ConfigLoader.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>

// Helper to read file
std::string readFile(const char* filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        exit(1);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cout << "Usage: ./test_loader <config_file>" << std::endl;
        return 1;
    }

    try {
        // 1. Pipeline: Lexer -> Parser -> Validator
        std::string content = readFile(argv[1]);
        Lexer lexer(content);
        std::vector<Token> tokens = lexer.tokenize();
        
        Parser parser(tokens);
        ConfigNode* root = parser.parse();
        
        ConfigValidator validator;
        validator.validate(root);

        // 2. GENERATION (The New Part)
        ConfigLoader loader;
        std::vector<ServerConfig> servers = loader.loadServers(root);

        // 3. Print Results (Prove it works)
        std::cout << "Successfully loaded " << servers.size() << " servers.\n" << std::endl;

        for (size_t i = 0; i < servers.size(); ++i) {
            ServerConfig& server = servers[i];
            std::cout << "=== SERVER " << (i + 1) << " ===" << std::endl;
            std::cout << "Host: " << server.host << " | Port: " << server.port << std::endl;
            std::cout << "Server Names: ";
            for (size_t k = 0; k < server.server_names.size(); k++) std::cout << server.server_names[k] << " ";
            std::cout << "\n" << std::endl;

            std::cout << "  --- Locations (Flattened) ---" << std::endl;
            for (size_t j = 0; j < server.locations.size(); ++j) {
                LocationConfig& loc = server.locations[j];
                std::cout << "  [" << loc.path << "]" << std::endl;
                std::cout << "    Root:       " << (loc.root.empty() ? "(Inherited/Empty)" : loc.root) << std::endl;
                std::cout << "    Index:      " << loc.index << std::endl;
                std::cout << "    Autoindex:  " << (loc.autoindex ? "ON" : "off") << std::endl;
                std::cout << "    Max Body:   " << loc.client_max_body_size << std::endl;
                
                if (!loc.allowed_methods.empty()) {
                    std::cout << "    Methods:    ";
                    for (size_t k = 0; k < loc.allowed_methods.size(); k++) std::cout << loc.allowed_methods[k] << " ";
                    std::cout << std::endl;
                }
                
                if (loc.return_url.first != 0) {
                    std::cout << "    Redirect:   " << loc.return_url.first << " -> " << loc.return_url.second << std::endl;
                }
                std::cout << std::endl;
            }
        }

        delete root;

    } catch (const std::exception& e) {
        std::cerr << "❌ Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
