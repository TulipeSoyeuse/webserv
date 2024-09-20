#include "Server.hpp"

Server::Server(char *config)
{
	std::ifstream _config_file;

	if (_config_file.open(config), _config_file.good())
	{
		_config_file.ignore(std::numeric_limits<std::streamsize>::max());
		std::streamsize content_length = _config_file.gcount();
		_config_file.clear();
		_config_file.seekg(0, std::ios_base::beg);
		char *payload = new char[content_length + 1];
		payload[content_length] = 0;
		_config_file.read(payload, content_length);
		_config.assign(payload);
		delete[] payload;
		read_config();
	}
}

bool Server::read_config()
{
	size_t cursor = 0;

	while (cursor = _config.find("server", cursor), cursor != std::string::npos)
	{
		std::map<std::string, std::string> server;
		cursor += 6;
		if (_config.get_server_name(cursor) != config_string::npos)
		{
			server.insert(
				std::pair<std::string, std::string>(
					"name",_config.get_next_word(_config.get_server_name(cursor))
					)
				);

		}
	}
}

bool Server::add_serv(std::string s)
{

}

Server::~Server()
{
}
