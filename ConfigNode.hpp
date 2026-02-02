#ifndef CONFIGNODE_HPP
#define CONFIGNODE_HPP

#include <string>
#include <vector>
#include <iostream>

enum NodeType {
    NODE_DIRECTIVE,
    NODE_BLOCK
};

struct ConfigNode {
    NodeType                    type;
    std::string                 name;
    std::vector<std::string>    args;
    std::vector<ConfigNode*>    children;
    ConfigNode* parent;

    // FIXED: Added default values back so you can call "new ConfigNode(NODE_BLOCK)"
    ConfigNode(NodeType t, 
               std::string n = "", 
               std::vector<std::string> a = std::vector<std::string>(), 
               ConfigNode* p = NULL);

    ~ConfigNode();
};

#endif
