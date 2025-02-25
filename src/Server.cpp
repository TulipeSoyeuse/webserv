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
		config_string s(_config_file);
		_config = s;
		read_config();
	}
	// * display info about all server
	display_params();
	init_default();
	// configuration_checking();
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
			std::cout << "this is conf " << l << std::endl;
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

std::string get_second_word(config_string l, std::string::iterator it, bool *start)
{
	(void)start;
	bool second = false;
	std::string key;
	while (it != l.get_str().end())
	{
		if (isspace(*it))
		{
			if (second)
				break;
		}
		else
		{
			second = true;
		}
		it++;
	}

	while (*it == ' ')
		it++;
	if (*it == '\n' || *it == '{')
		return key;
	else
	{
		while (!isspace(*it) && *it != '\n' && *it != '{')
		{
			key.push_back(*it);
			it++;
		}
	}
	return key;
}

std::string get_first_word(config_string l, std::string::iterator it, bool *start)
{
	std::string key;
	while (it != l.get_str().end())
	{
		if (isspace(*it))
		{
			if (*start)
				break;
		}
		else
		{
			*start = true;
			key.push_back(*it);
		}
		it++;
	}
	return key;
}

server_p parse_subpart_config_line(config_string l)
{
	std::string a;
	bool start = false;
	std::string::iterator it;
	std::string key;
	std::string key2;

	it = l.get_str().begin();
	key = get_first_word(l, it, &start);
	key2 = get_second_word(l, it, &start);
	while (*it == ' ' || *it == '{' || *it == '\n')
		it++;
	std::string m;
	std::string sub_str = l.get_str().substr(l.get_str().find('{') + 2, (l.get_str().find('}') - 1) - (l.get_str().find('{') + 1) - 1);
	config_string sub(sub_str);
	Map map;
	while (m = sub.get_next_line(), !m.empty())
	{
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
		map.insert(std::make_pair(c, d));
	}
	if (key == "location")
	{
		server_p serv_l(key2, std::pair<std::string, Map>("", map));
		return serv_l;
	}
	server_p serv_p(key, std::pair<std::string, Map>(key2, map));
	return serv_p;
}

server_p Server::parse_config_line(config_string l)
{
	std::string a;
	bool start = false;
	std::string b;
	size_t i = 0;
	std::string::iterator it;

	if (l.get_str().find('{') != l.get_str().npos)
		return parse_subpart_config_line(l);

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
	if (a == "host")
	{
		b = l.get_str().substr(i, (l.get_str().size() - i) - 1);
	}
	else
		b = l.get_next_word(i);

	Map m;
	if (a == "location")
	{
		server_p l(b, std::pair<std::string, Map>("", m));
		return (l);
	}
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
				std::string f(it->find("route")->second.first + it->find("location")->second.first + "/" + it2->second);
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

inline bool check_host(std::string &host, const std::string &hosts)
{
	(void)host;
	char **s_hosts = ft_split(hosts.c_str(), ' ');
	std::cout << "THIS IS HOST : " << hosts << " and host : " << host << std::endl;
	for (int i = 0; s_hosts[i]; i++)
	{
		std::string h_str = s_hosts[i];
		if (host.c_str() == h_str)
		{
			ft_freestrs(s_hosts, (size_t)i);
			return true;
		}
	}
	return false;
}

const server_m &Server::get_config(std::string &host, int port) const
{
	for (Server_lst::const_iterator it = _servers.begin(); it != _servers.end(); ++it)
	{
		bool match_host = check_host(host, (*it).find("host")->second.first);
		if ((*it).find("port") != it->end() && atoi(((*it).find("port")->second.first.c_str())) == port && match_host)
			return (*it);
	}
	return _default_server;
}

const std::pair<std::string, Map> &Server::get_location_subconf(const server_m &m, const std::string &uri) const
{
	size_t last;
	std::string substr = uri;
	while (last = substr.find_last_of('/'), last != uri.npos)
	{
		substr = uri.substr(0, last);
		std::cout << "substr uri: " << substr << "\n";
		for (server_m::const_iterator it = m.begin(); it != m.end(); ++it)
		{
			if (it->first == substr)
				return (it->second);
		}
	}
	return (m.find("/")->second);
}

void Server::init_default()
{
	_default_server["Host"] = server_m_pair("NOTFOUND", Map());

	Map error_pages;
	error_pages["400"] = "error_pages/error_400.html";
	error_pages["403"] = "error_pages/error_403.html";
	error_pages["404"] = "error_pages/error_404.html";
	error_pages["408"] = "error_pages/error_408.html";
	error_pages["500"] = "error_pages/error_500.html";
	error_pages["505"] = "error_pages/error_505.html";
	_default_server["error_page"] = server_m_pair(std::string(), error_pages);
}

const size_t &Server::get_server_count() const
{
	return (server_count);
}

const server_m &Server::get_default_config() const
{
	return (_default_server);
}

const port_array &Server::get_ports() const
{
	return (port_lst);
}

const bool &Server::is_conf_valid() const
{
	return (_valid_conf);
}

const Server_lst &Server::get_servers() const
{
	return (_servers);
}

Server::~Server()
{
}
