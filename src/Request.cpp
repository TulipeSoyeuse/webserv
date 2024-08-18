#include "Request.hpp"
#include <iostream>
#include <sstream>

Request::Request(char *http_package)
{
	_brut_request = http_package;
	int pos = _brut_request.length() - 3;
	_brut_request.erase(pos, std::string::npos);
	parse();
}

void Request::parse()
{
	std::stringstream s;
	s.str(_brut_request);

	std::string line;
	std::getline(s, line);

	int f1 = line.find_first_of(' ');
	int f2 = line.find_last_of(' ');

	if (f1 == f2 || f1 == (int)std::string::npos)
	{
		std::cout << "request Malformed" << std::endl;
		return ;
	}

	if (line.find("GET") == 0)
		_Type = GET;
	else if (line.find("POST") == 0)
		_Type = POST;

	_request["URI"] = line.substr(f1 + 1, f2 - f1);
	_request["Protocol"] = line.substr(f2 + 1);

	while (std::getline(s, line))
	{
		_request[line.substr(0, line.find_first_of(':'))] =
			line.substr(line.find_first_of(':') + 2, line.length());
	}

	if (_request.find("Content-Length") != _request.end())
	{
		f1 = _brut_request.find("\n\n") + 2;
		_request["Payload"] = _brut_request.substr(f1);
	}
}

void Request::display_request() const
{
	if (_Type == GET)
		std::cout << "GET request" << "\n";
	else if (_Type == POST)
		std::cout << "POST request" << "\n";

	for (std::map<std::string, std::string>::const_iterator it = _request.begin();
		 it != _request.end(); ++it)
		std::cout << it->first << ": "<< it->second << "\n";
	std::cout << std::endl;
}

const std::map<std::string, std::string> &Request::get_request() const
{
	return (_request);
}

const std::map<std::string, std::string> &Request::get_params() const
{
	return (_params);
}

const type_e &Request::get_type() const
{
	return (_Type);
}

Request::~Request()
{
}
