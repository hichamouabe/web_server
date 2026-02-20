#ifndef CONFIGLOADER_HPP
#define CONFIGLOADER_HPP

#include "ConfigNode.hpp"
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <cstdlib>

struct LocationConfig {
	std::string			path;
	std::string			root;
	std::string			index;
	bool				autoindex;
	std::vector<std::string>	allowed_methods;
	std::pair<int, std::string>	return_url;
	std::string			cgi_pass;
	std::string			upload_store;
	unsigned long			client_max_body_size;
	std::map<int, std::string>	error_pages;

	LocationConfig() {
		autoindex = false;
		client_max_body_size = 0;
	}
};

struct ServerConfig {
	std::string			host;
	int				port;
	std::vector<std::string>	server_names;
	std::map<int, std::string>	error_pages;
	unsigned long			client_max_body_size;
	std::vector<LocationConfig>	locations;

	ServerConfig() {
		host = "0.0.0.0";
		port = 80;
		client_max_body_size = 10000;
	}
};

class	ConfigLoader {
	public:
		ConfigLoader();
		~ConfigLoader();
		std::vector<ServerConfig> loadServers(ConfigNode* root);

	private:
		void	loadServer(ConfigNode* serverNode, ServerConfig& config);
		void	loadLocation(ConfigNode* locNode, LocationConfig parentLoc, std::vector<LocationConfig>& list);
};

#endif
