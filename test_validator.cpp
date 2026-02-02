#include "Lexer.hpp"
#include "Parser.hpp"
#include "ConfigValidator.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>

// Helper to read file into string
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
        std::cout << "Usage: ./tester <config_file_path>" << std::endl;
        return 1;
    }

    std::string content = readFile(argv[1]);
    std::cout << "Testing File: " << argv[1] << "\n--------------------------" << std::endl;

    try {
        // 1. Lexer
        Lexer lexer(content);
        std::vector<Token> tokens = lexer.tokenize();

        // 2. Parser
        Parser parser(tokens);
        ConfigNode* root = parser.parse();

        // 3. Validator
        ConfigValidator validator;
        validator.validate(root);

        std::cout << "✅ VALIDATION PASSED" << std::endl;
        
        // Cleanup (Optional in simple test, but good habit)
        delete root; 

    } catch (const std::exception& e) {
        std::cout << "❌ VALIDATION FAILED: " << e.what() << std::endl;
    }

    return 0;
}
