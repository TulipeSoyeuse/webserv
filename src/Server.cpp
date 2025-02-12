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

inline void Server::insert(server_m &m, std::string k, std::string v)
{
	// ----- VALUE -------
	Map map;
	std::pair<std::string, Map> p(v, map);
	// ----- ADD ------
	m.insert(server_p(k, p));
}

inline void Server::insert(server_m &m, std::string k, Map v)
{
	// ----- VALUE -------
	std::string s;
	std::pair<std::string, Map> p(s, v);
	// ---- ADD -------
	m.insert(server_p(k, p));
}

inline void Server::insert(server_m &m, server_p p)
{
	m.insert(p);
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
	while (cursor = _config.get_str().find("server", cursor), cursor != std::string::npos)
	{
		server_m server;
		if (_config.get_server_name(cursor) != std::string::npos)
		{
			insert(server, "name", _config.get_next_word(_config.get_server_name(cursor)));
			std::cout << "server name parsed ("
					  << _config.get_next_word(_config.get_server_name(cursor))
					  << "), moving to configuration...\n";
		}
		else
		{
			// * default server name
			insert(server, "name", "unnamed");
			if (_debug)
				std::cout << "server unnamed, moving to configuration...\n";
		}
		cursor += 6;

		config_string server_conf(_config.get_config_subpart(cursor));
		std::string l;
		std::cout << "this is server conf :: -------- " << server_conf.get_str() << " ------------\n";

		while (l = server_conf.get_next_conf(), !l.empty())
		{
			if (!is_string_empty(l))
				insert(server, parse_config_line(l));
		}
		_servers.push_back(server);
		server_count++;
		// * get port
		if (server.find("port") != server.end())
			port_lst.push_back(std::atoi(server.find("port")->second.first.c_str()));
	}
	return (true);
}

server_p Server::parse_config_line(config_string l)
{
	std::string a;
	bool start = false;
	size_t i = 0;
	std::string::iterator it;

	if (l.get_str().find('{') != l.get_str().npos)
	{
		std::string key;
		for (it = l.get_str().begin(); it != l.get_str().end(); ++it)
		{
			if (isspace(*it))
			{
				if (start)
					break;
			}
			else
			{
				start = true;
				key.push_back(*it);
			}
		}

		std::cout << "key = " << key;
		while (*it == ' ' || *it == '{' || *it == '\n')
			it++;
		std::string m;
		std::string sub_str = l.get_str().substr(l.get_str().find('{') + 2, (l.get_str().find('}') - 1) - (l.get_str().find('{') + 1) - 1);
		config_string sub(sub_str);
		Map map;
		// std::cout << "SUB : ----" << sub_str << "-----\n";
		while (m = sub.get_next_line(), !m.empty())
		{
			std::cout << "m = " << m << std::endl;
			start = false;
			std::string c;
			std::string d;
			std::string::iterator it2;

			for (it2 = m.begin(); it2 != m.end(); ++it2)
			{
				if (isspace(*it2))
				{
					if (start == true)
						break;
				}
				else
				{
					start = true;
					if (*it2 != '\n' && !isspace(*it2))
						c.push_back(*it2);
				}
			}
			while (true)
			{
				if (isspace(*it2))
					it2++;
				else
					break;
			}
			while (it2 != m.end() && *it2 != '\n' && *it2 != ' ')
			{
				d.push_back(*it2);
				it2++;
			}
			std::cout << "this is a :" << c << std::endl;
			std::cout << "this is b " << d << std::endl;

			map.insert(std::make_pair(c, d));
		}
		server_p serv_p(key, std::pair<std::string, Map>("", map));
		return serv_p;
	}

	for (it = l.get_str().begin(); it != l.get_str().end(); ++it)
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
		if (isspace(l.get_str()[i]))
			i++;
		else
			break;
	std::string b = l.get_next_word(i);

	Map m;
	server_p p(a, std::pair<std::string, Map>(b, m));

	return (p);
}

void Server::display_params()
{
	std::cout << "Displaying all elements in the server structure:\n";

	// Parcours de la liste des serveurs
	for (Server_lst::const_iterator it1 = _servers.begin(); it1 != _servers.end(); ++it1)
	{
		std::cout << "Server entry:\n";

		for (server_m::const_iterator it2 = it1->begin(); it2 != it1->end(); ++it2)
		{
			const std::string &key = it2->first;
			const std::pair<std::string, Map> &value = it2->second;
			const std::string &first_value = value.first;
			const Map &second_value = value.second;

			if (first_value.empty() && !second_value.empty())
			{
				std::cout << "[" << key << "] has empty string value and a non-empty map:\n";
				for (Map::const_iterator it3 = second_value.begin(); it3 != second_value.end(); ++it3)
				{
					std::cout << "\t- [" << it3->first << "] -> \"" << it3->second << "\"\n";
				}
			}
			else if (!first_value.empty() && second_value.empty())
			{
				std::cout << "[" << key << "]=\"" << first_value << "\"\n";
			}
			else if (first_value.empty() && second_value.empty())
			{
				std::cout << "[" << key << "] has empty string and empty map.\n";
			}
			else
			{
				std::cout << "[" << key << "]=\"" << first_value << "\" with a non-empty map:\n";
				for (Map::const_iterator it3 = second_value.begin(); it3 != second_value.end(); ++it3)
				{
					std::cout << "\t- [" << it3->first << "] -> \"" << it3->second << "\"\n";
				}
			}
		}
		std::cout << "\n";
	}
}

const std::string &Server::get_param(const std::string &s, const std::string &host)
{
	server_m::iterator val;

	for (Server_lst::iterator it = _servers.begin(); it != _servers.end(); ++it)
	{
		// find the server
		val = it->find("host");
		if (val != it->end() && val->second.first == host)
		{
			// find the host
			server_m::const_iterator val2 = it->find(s);
			if (val2 != it->end())
				return (val2->second.first);
		}
	}
	Server_lst::iterator it = _servers.begin();
	it++;
	val = it->find(s);
	if (val != it->end())
		return (val->second.first);
	return (_empty_res);
}

const std::string &Server::get_param(const std::string &s, const std::string &host, const std::string &subk)
{
	server_m::iterator val;

	for (Server_lst::iterator it = _servers.begin(); it != _servers.end(); ++it)
	{
		// find the server
		val = it->find("host");
		if (val != it->end() && val->second.first == host)
		{
			// find the host
			server_m::const_iterator val2 = it->find(s);
			if (val2 != it->end())
			{
				Map::const_iterator val3 = val2->second.second.find(subk);
				if (val3 != val2->second.second.end())
					return (val3->second);
				else
					return (_empty_res);
			}
		}
	}
	Server_lst::iterator it = _servers.begin();
	it++;
	val = it->find(s);
	if (val != it->end())
	{
		Map::const_iterator val2 = val->second.second.find(subk);
		if (val2 != val->second.second.end())
			return (val2->second);
		else
			return (_empty_res);
	}
	return (_empty_res);
}

void Server::configuration_checking()
{
	server_m::iterator val;
	for (Server_lst::iterator it = _servers.begin(); it != _servers.end(); ++it)
	{
		// ------------------ ERROR PAGE ----------------------------
		val = it->find("error_page");
		if (val != it->end())
		{
			for (Map::iterator it2 = val->second.second.begin(); it2 != val->second.second.end(); ++it2)
			{
				std::string f(it->find("route")->first + it->find("location")->second.first + "/" + it2->second);
				if (!does_file_exist(f))
				{
					std::cerr << "config error, file " << f << " don't exist\n"
							  << "from server: " << it->find("name")->second.first << "\n";
					return;
				}
			}
		}
	}
	_valid_conf = true;
}

const server_m &Server::get_config(std::string &host, int port) const
{
	(void)host;
	for (Server_lst::const_iterator it = _servers.begin(); it != _servers.end(); ++it)
	{
		if ((*it).find("port") != it->end() && atoi(((*it).find("port")->second.first.c_str())) == port)
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
