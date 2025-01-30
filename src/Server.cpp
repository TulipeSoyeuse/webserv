#include "Server.hpp"

bool is_string_empty(std::string s)
{
	bool c = true;
	for (std::string::iterator it = s.begin(); it != s.end(); ++it)
	{
		if (!(isspace(*it) || *it == '\n'))
			c = false;
	}
	return (c);
}

Server::Server(const char *config, bool debug = false) : _debug(debug), _empty_res(),
														 server_count(0), _valid_conf(false)
{
	std::ifstream _config_file;

	// TODO: regular path "/tmp/www"
	if (_debug)
		std::cout << "server init...\n";
	if (_config_file.open(config), _config_file.good())
	{
		if (_debug)
			std::cout << "file found...\nreading...\n";

		config_string s(_config_file);
		_config = s;
		read_config();
	}
	// * display info about all server
	display_params();
	configuration_checking();
}

bool Server::read_config()
{
	size_t cursor = 0;

	if (_debug)
		std::cout << "parsing config...\n";
	while (cursor = _config.find("server", cursor), cursor != std::string::npos)
	{
		std::map<std::string, std::string> server;
		if (_config.get_server_name(cursor) != config_string::npos)
		{
			server.insert(
				// * get serveur name
				std::pair<std::string, std::string>(
					"name", _config.get_next_word(_config.get_server_name(cursor))));
			if (_debug)
				std::cout << "server name parsed ("
						  << _config.get_next_word(_config.get_server_name(cursor))
						  << "), moving to configuration...\n";
		}
		else
		{
			// * default server name
			server.insert(
				std::pair<std::string, std::string>(
					"name", "unnamed"));
			if (_debug)
				std::cout << "server unnamed, moving to configuration...\n";
		}
		cursor += 6;

		config_string server_conf(_config.get_config_subpart(cursor));
		std::string l;
		while (l = server_conf.get_next_line(), !l.empty())
		{

			if (!is_string_empty(l))
				server.insert(parse_config_line(l));
		}
		_servers.push_back(server);
		server_count++;
		// * get port
		if (server.find("port") != server.end())
			port_lst.push_back(std::atoi(server.find("port")->second.c_str()));
	}
	return (true);
}

std::pair<std::string, std::string> Server::parse_config_line(config_string l)
{
	std::string a;
	bool start = false;
	size_t i = 0;
	for (std::string::iterator it = l.begin(); it != l.end(); ++it)
	{
		if (isspace(*it))
		{
			if (start)
				break;
		}
		else
		{
			start = true;
			if (*it != '\n' && !isspace(*it))
				a.push_back(*it);
		}
		i++;
	}
	while (true)
		if (isspace(l[i]))
			i++;
		else
			break;
	std::string b = l.get_next_word(i);
	std::pair<std::string, std::string> p(a, b);

	if (_debug)
		std::cout << "----------\n"
				  << "param: " << a << "\nvalue: \""
				  << b << "\"\n----------\n";

	return (p);
}

void Server::display_params()
{
	std::cout << "-------------------\ndisplay webserv params and servers: " << _servers.size() << "\n";
	for (Server_lst::iterator it1 = _servers.begin(); it1 != _servers.end(); ++it1)
	{
		std::cout << "Server 1:\n";
		for (std::map<std::string, std::string>::iterator it2 = (*it1).begin(); it2 != (*it1).end(); ++it2)
		{
			std::cout << "[" << it2->first << "]=\"" << it2->second << "\"\n";
		}
		std::cout << "\n";
	}
}

const std::string &Server::get_param(const std::string &s, const std::string &host)
{
	std::map<std::string, std::string>::iterator val;

	for (Server_lst::iterator it = _servers.begin(); it != _servers.end(); ++it)
	{
		val = it->find("host");
		if (val != it->end() && val->second == host)
		{
			std::map<std::string, std::string>::const_iterator val2 = it->find(s);
			if (val2 != it->end())
				return (val2->second);
		}
	}
	Server_lst::iterator it = _servers.begin();
	it++;
	val = it->find(s);
	if (val != it->end())
		return (val->second);
	return (_empty_res);
}

void Server::configuration_checking()
{
	std::map<std::string, std::string>::iterator val;
	for (Server_lst::iterator it = _servers.begin(); it != _servers.end(); ++it)
	{
		val = it->find("error_page");
		if (val != it->end())
		{
			std::string f(it->find("route")->second + it->find("location")->second + "/" + val->second);
			if (!does_file_exist(f))
			{
				std::cerr << "config error, file " << f << " don't exist\n"
						  << "from server: " << it->find("name")->second << "\n";
				return;
			}
		}
	}
	_valid_conf = true;
}

const std::map<std::string, std::string> &Server::get_config(std::string &host, int port) const
{
	(void)host;
	for (Server_lst::const_iterator it = _servers.begin(); it != _servers.end(); ++it)
	{
		if ((*it).find("port") != it->end() && atoi(((*it).find("port")->second.c_str())) == port)
			return (*it);
	}
	return ((*(_servers.begin()++)));
}

const size_t &Server::get_server_count() const
{
	return (server_count);
}

const port_array &Server::get_ports() const
{
	return (port_lst);
}

const bool &Server::is_conf_valid() const
{
	return (_valid_conf);
}

Server::~Server()
{
}
