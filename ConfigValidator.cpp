#include "ConfigValidator.hpp"
#include <sstream>
#include <iostream>

ConfigValidator::ConfigValidator() { initRules(); }

ConfigValidator::~ConfigValidator() { }

void	ConfigValidator::initRules() {

	_rules["server"] = (DirectiveRule){0, 0, (1 << CTX_MAIN), false};
	_rules["location"] = (DirectiveRule){1, 1, (1 << CTX_SERVER) | (1 << CTX_LOCATION), false};
	_rules["listen"] = (DirectiveRule){1, 1, (1 << CTX_SERVER), false};
	_rules["server_name"] = (DirectiveRule){1, 10, (1 << CTX_SERVER), true};
	_rules["root"] = (DirectiveRule){1, 1, (1 << CTX_SERVER) | (1 << CTX_LOCATION), true};
	_rules["index"] = (DirectiveRule){1, 99, (1 << CTX_SERVER) | (1 << CTX_LOCATION), false};
	_rules["error_page"] = (DirectiveRule){2, 99, (1 << CTX_SERVER) | (1 << CTX_LOCATION), false};
	_rules["autoindex"] = (DirectiveRule){1, 1, (1 << CTX_SERVER) | (1 << CTX_LOCATION), true};
	_rules["client_max_body_size"] = (DirectiveRule){1, 1, (1 << CTX_MAIN) | (1 << CTX_SERVER) | (1 << CTX_LOCATION), true};
	_rules["allow_methods"] = (DirectiveRule){1, 3, (1 << CTX_LOCATION), true};
	_rules["return"] = (DirectiveRule){2, 2, (1 << CTX_SERVER) | (1 << CTX_LOCATION), true};
	_rules["cgi_pass"] = (DirectiveRule){1, 2, (1 << CTX_LOCATION), true};
	_rules["upload_store"] = (DirectiveRule){1, 1, (1 << CTX_LOCATION), true};

}

void	ConfigValidator::validate(ConfigNode* root) {
	if (!root) 
		return;
	validateNode(root, CTX_MAIN);
}

void	ConfigValidator::validateNode(configNode* node, int current_context) {
	if (node->name == "ROOT") {
		for (size_t i = 0; i < node->children.size(); i++) {
			validateNode(node->children[i], current_context);
		}
		return ;
	}

	if (_rules.find(node->name) == _rules.end())
	{
		throw std::runtime_error("validation error: Unknown directive'" + node->name + "'");
	}

	DirectiveRule rule = _rules[node->name];

	if (!((1 << current_context) & rule.allowed_contexts)) {
		throw std::runtime_error("Validation error: Directive '" + node->name + "' is not allowed in the current context");
	}

	if (node->args.size() < (size_t)rule.min_args || node->args.size() > (size_t)rule.max_args) {
		throw std::runtime_error("Validation error: the args number is not the allowed one in the Directive '" + node->name + "'");
	}

	if (rule.is_unique && node->parent)
	{
		int	count = 0;
		for (size_t i = 0; i < node->parent->children.size(); i++) {
			if (node->parent->children[i]->name == node->name) {
				count++;
			}
		}
		if (count > 1) {
			throw std::runtime_error("Validation error: Duplicate directive'" + node->name + "'");
		}
	}

	int next_context = current_context;
	if (node->name == "server") 
		next_context = CTX_SERVER;
	if (node->name == "location")
		next_context = CTX_LOCATION;

	for (size_t i = 0; i < node->children.size(); i++) {
		validateNode(node->children[i], next_context);
	}
}
