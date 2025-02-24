#pragma once

#include "includes.hpp"
#include "config_string.hpp"

// dict of string match value
typedef std::map<std::string, std::string> Map;

// unique dict type entry in server, the value can be a string,
// a dictionary or both
// key is always a string
typedef std::pair<std::string, std::pair<std::string, Map> > server_p;

// unique server : dict of key (string) / v	value (server_p)
typedef std::map<std::string, std::pair<std::string, Map> > server_m;

// list of all different servers
typedef std::vector<server_m> Server_lst;

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
	server_p parse_config_line(config_string);
	port_array port_lst;
	server_m _default;
	// ------- ERROR CHECK ------
	bool _valid_conf;
	void configuration_checking();
	// --------------------------
	void insert(server_m &, std::string, std::string);
	void insert(server_m &, std::string, Map);
	void insert(server_m &m, server_p p);

public:
	Server(const char *config, bool debug);

	const std::string &get_param(const std::string &, const std::string &);
	const std::string &get_param(const std::string &, const std::string &, const std::string &);
	const server_m &get_config(std::string &, int) ;
	const server_m &create_default_config();
	const Server_lst &get_servers() const;
	const std::pair<std::string, Map> &get_location_subconf(const server_m &, const std::string &) const;
	const size_t &get_server_count() const;
	const port_array &get_ports() const;
	const bool &is_conf_valid() const;
	~Server();
};
