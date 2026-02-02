#include "ConfigNode.hpp"

// Implementation of Constructor
// (Note: No default values here, they are already in the .hpp)
ConfigNode::ConfigNode(NodeType t, std::string n, std::vector<std::string> a, ConfigNode* p) 
    : type(t), name(n), args(a), parent(p) {}

// Implementation of Destructor
ConfigNode::~ConfigNode() {
    for (size_t i = 0; i < children.size(); ++i) {
        delete children[i];
    }
}
