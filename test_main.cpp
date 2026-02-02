#include "Lexer.hpp"
#include "Parser.hpp"
#include <iostream>
#include <fstream>  // For file handling
#include <sstream>  // For reading file into string
#include <cstdlib>  // For exit()

// Recursive Tree Printer
void printTree(ConfigNode* node, int depth) {
    if (!node) return;
    std::string indent(depth * 4, ' ');

    if (node->name == "ROOT") {
        std::cout << "[ROOT]" << std::endl;
    } else {
        std::cout << indent;
        if (node->type == NODE_BLOCK) std::cout << "[BLOCK] ";
        else                          std::cout << "[DIR]   ";
        
        std::cout << node->name;
        for (size_t i = 0; i < node->args.size(); ++i) {
            std::cout << " " << node->args[i];
        }
        std::cout << std::endl;
    }

    for (size_t i = 0; i < node->children.size(); ++i) {
        printTree(node->children[i], depth + 1);
    }
}

// Helper to read file content
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
        std::cerr << "Usage: ./webserv <config_file>" << std::endl;
        return 1;
    }

    try {
        // 1. Read File
        std::string configContent = readFile(argv[1]);

        // 2. Tokenize
        Lexer lexer(configContent);
        std::vector<Token> tokens = lexer.tokenize();

        // 3. Parse
        Parser parser(tokens);
        ConfigNode* root = parser.parse();

        // 4. Print & Validate (Visual Check)
        std::cout << "--- SUCCESSFULLY PARSED ---" << std::endl;
        printTree(root, 0);

        // 5. Cleanup
        delete root;

    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}
