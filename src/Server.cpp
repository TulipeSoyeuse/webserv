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

Server::Server(const char *config, bool debug = false) : _debug(debug), _valid_conf(false)
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
	if (_debug)
		display_params();
}

bool Server::read_config()
{
	size_t cursor = 0;

	if (_debug)
		std::cout << "parsing config...\n";
	while (cursor = _config.find("server", cursor), cursor != std::string::npos)
	{
		std::cout << "cursor :" << cursor << "\n";
		std::map<std::string, std::string> server;
		if (_config.get_server_name(cursor) != config_string::npos)
		{
			server.insert(
				std::pair<std::string, std::string>(
					"name", _config.get_next_word(_config.get_server_name(cursor))));
			if (_debug)
				std::cout << "server name parsed ("
						  << _config.get_next_word(_config.get_server_name(cursor))
						  << "), moving to configuration...\n";
		}
		else
		{
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
	{
		std::cout << "----------\n"
				  << "param: " << a << "\nvalue: \"" << b << "\"\n----------\n";
	}

	return (p);
}

void Server::display_params()
{
	std::cout << "-------------------\ndisplay webserv params and servers: " << _servers.size() <<"\n";
	for (Server_lst::iterator it1 = _servers.begin(); it1 != _servers.end(); ++it1)
	{
		std::cout << "Server 1:\n";
		for (std::map<std::string, std::string>::iterator it2 = (*it1).begin(); it2 != (*it1).end(); ++it2)
		{
			std::cout << "[" << it2->first << "]=\"" << it2->second << "\"; ";
		}
		std::cout << "\n";
	}
}

const std::string &get_param(cont std::string &s)
{

}

void Server::configuration_checking()
{
	_valid_conf = true;
}

// void Server::start()
// {
// }

Server::~Server()
{
}
