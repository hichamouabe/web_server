#ifndef CONFIGVALIDATOR_HPP
#define CONFIGVALIDATOR_HPP

#include "ConfigNode.hpp"
#include <map>
#include <string>
#include <vector>
#include <stdexcept>
#include <sstream>


enum	Context {
	CTX_MAIN	= 0,
	CTX_SERVER	= 1,
	CTX_LOCATION	= 2
};

struct	DirectiveRule {
	int	min_args;
	int	max_args;
	int	allowed_contexts;
	bool	is_unique;
	bool	is_block;
};

class	ConfigValidator {
	private:
		std::map<std::string, DirectiveRule> _rules;

		void	initRules();
		void	validateNode(ConfigNode* node, int context);
		void	validateBehaviour();
	public:
		ConfigValidator();
		~ConfigValidator();
		void	validate(ConfigNode* root);
};

#endif
