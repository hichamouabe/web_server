#include "Parser.hpp"
#include <sstream>

Parser::Parser(const std::vector<Token>& tokens) : _tokens(tokens), _pos(0) {}
Parser::~Parser() {}

ConfigNode* Parser::parse() {
    ConfigNode* root = new ConfigNode(NODE_BLOCK);
    root->name = "ROOT";

    while (_pos < _tokens.size() && _tokens[_pos].type != T_EOF) {
        // [FIX 1] Capture the child in a variable first
        ConfigNode* child = parseStatement(); 
        
        child->parent = root; // Now this works because 'child' exists
        root->children.push_back(child);
    }
    return root;
}

ConfigNode* Parser::parseStatement() {
    if (_pos >= _tokens.size()) return NULL; // Should not happen if logic is correct

    if (_tokens[_pos].type != T_WORD) {
        throw std::runtime_error("Syntax Error: Expected directive name");
    }
    
    std::string name = _tokens[_pos].value;
    _pos++; 

    std::vector<std::string> args;
    while (_pos < _tokens.size() && _tokens[_pos].type == T_WORD) {
        args.push_back(_tokens[_pos].value);
        _pos++; // Eat argument
    }

    if (_pos >= _tokens.size()) throw std::runtime_error("Unexpected End of File");

    if (_tokens[_pos].type == T_SEMICOLON) {
        _pos++; // Eat the ';'
        return new ConfigNode(NODE_DIRECTIVE, name, args);
    }
    
    else if (_tokens[_pos].type == T_LBRACE) {
        _pos++; // Eat the '{'
        
        ConfigNode* node = new ConfigNode(NODE_BLOCK, name, args);
        
        while (_pos < _tokens.size() && _tokens[_pos].type != T_RBRACE) {
            //node->children.push_back(parseStatement());
	    ConfigNode* child = parseStatement();
	    child->parent = node;
	    node->children.push_back(child);
        }
        
        if (_pos >= _tokens.size() || _tokens[_pos].type != T_RBRACE) {
            throw std::runtime_error("Syntax Error: Missing closing brace '}'");
        }
        
        _pos++; // Eat the '}'
        return node;
    }
    
    else {
        throw std::runtime_error("Syntax Error: Expected ';' or '{'");
    }
}
