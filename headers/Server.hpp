#pragma once

#include "includes.hpp"
#include "config_string.hpp"

typedef std::vector<std::map<std::string, std::string> > Server_lst;
typedef std::vector<int> port_array;

class Server
{
private:
	// -------- DEBUG ----------
	bool _debug;
	void display_params();
	// -------- CONFIG ---------
	config_string _config;
	Server_lst _servers;
	bool read_config();
	std::string _empty_res;
	size_t server_count;
	std::pair<std::string, std::string> parse_config_line(config_string);
	port_array port_lst;
	// ------- ERROR CHECK ------
	bool _valid_conf;
	void configuration_checking();
	// --------------------------

public:
	Server(const char *config, bool debug);

	const std::string &get_param(const std::string &, const std::string &);
	const std::map<std::string, std::string> &get_config(std::string &, int) const;
	const size_t &get_server_count() const;
	const port_array &get_ports() const;
	const bool &is_conf_valid() const;
	~Server();
};
