#include "Lexer.hpp"
#include <iostream>

// Helper to print token type names
const char* getTokenName(TokenType type) {
    switch (type) {
        case T_WORD:      return "WORD";
        case T_LBRACE:    return "LBRACE";
        case T_RBRACE:    return "RBRACE";
        case T_SEMICOLON: return "SEMICOLON";
        case T_EOF:       return "EOF";
        default:          return "UNKNOWN";
    }
}

int main() {
    // A tricky config that tests ALL edge cases:
    std::string config = 
        "server{\n"                        // No space test
        "    listen 80;#Inline comment\n"  // Comment test
        "    root \"/var/www/my site\";\n" // Quoted string test
        "}";

    std::cout << "--- INPUT CONFIG ---" << std::endl;
    std::cout << config << std::endl;
    std::cout << "--------------------" << std::endl;

    Lexer lexer(config);
    std::vector<Token> tokens = lexer.tokenize();

    std::cout << "--- GENERATED TOKENS ---" << std::endl;
    for (size_t i = 0; i < tokens.size(); i++) {
        std::cout << "[" << getTokenName(tokens[i].type) << "] " 
                  << "'" << tokens[i].value << "' "
                  << "(Line " << tokens[i].line << ")" << std::endl;
    }

    return 0;
}
