#include "Request.hpp"
#include <iostream>
#include <sstream>

Request::Request(char *http_package) : _brut_request(http_package)
{
	parse();
}

void Request::parse()
{
	std::stringstream s;
	s.str(_brut_request);

	std::string line;
	std::getline(s, line);
	std::cout << _brut_request << std::endl;
	_request.Content_Length = 0;

	int f1 = line.find_first_of(' ');
	int f2 = line.find_last_of(' ');

	if (f1 == f2 || f1 == (int)std::string::npos)
	{
		std::cout << "request Malformed" << std::endl;
	}

	if (line.find("GET") == 0)
		_request.Type = GET;
	else if (line.find("POST") == 0)
		_request.Type = POST;

	_request.URI = line.substr(f1 + 1, f2 - f1);
	_request.PROTOCOL = line.substr(f2 + 1);

	while (std::getline(s, line), line.size() > 1)
	{
		// std::cout << line << line.size() << std::endl;
		parse_header_param(line);
		if (line.find("\r\n"))
			break;
	}

	if (_request.Content_Length)
	{
		f1 = _brut_request.find("\n\n") + 2;
		_request.Payload = _brut_request.substr(f1)
							   .erase(f1 + _request.Content_Length);
	}
}

void Request::parse_header_param(std::string &line)
{
	std::string key = line.substr(0, line.find_first_of(':'));
	std::string value = line.substr(line.find_first_of(':') + 2, line.length());

	if (key == "Accept")
		_request.Accept = value;
	else if (key == "Accept-Charset")
		_request.Accept_Charset = value;
	else if (key == "Accept-Encoding")
		_request.Accept_Encoding = value;
	else if (key == "Accept-Language")
		_request.Accept_Language = value;
	else if (key == "Authorization")
		_request.Authorization = value;
	else if (key == "Expect")
		_request.Expect = value;
	else if (key == "From")
		_request.From = value;
	else if (key == "Host")
		_request.Host = value;
	else if (key == "If-Match")
		_request.If_Match = value;
	else if (key == "If-Modified-Since")
		_request.If_Modified_Since = value;
	else if (key == "If-None-Match")
		_request.If_None_Match = value;
	else if (key == "If-Unmodified-Since")
		_request.If_Unmodified_Since = value;
	else if (key == "Max-Forwards")
		_request.Max_Forwards = value;
	else if (key == "Proxy-Authorization")
		_request.Proxy_Authorization = value;
	else if (key == "Range")
		_request.Range = value;
	else if (key == "Referer")
		_request.Referer = value;
	else if (key == "TE")
		_request.TE = value;
	else if (key == "User-Agent")
		_request.User_Agent = value;
	else if (key == "Content-Length")
		_request.Content_Length = std::atoll(value.c_str());
}

Request::~Request()
{
}

void Request::display_request()
{
	std::cout << "Type: " << _request.Type << std::endl;
	std::cout << "URI: " << _request.URI << std::endl;
	std::cout << "PROTOCOL: " << _request.PROTOCOL << std::endl;
	std::cout << "Accept: " << _request.Accept << std::endl;
	std::cout << "Accept_Charset: " << _request.Accept_Charset << std::endl;
	std::cout << "Accept_Encoding: " << _request.Accept_Encoding << std::endl;
	std::cout << "Accept_Language: " << _request.Accept_Language << std::endl;
	std::cout << "Authorization: " << _request.Authorization << std::endl;
	std::cout << "Expect: " << _request.Expect << std::endl;
	std::cout << "From: " << _request.From << std::endl;
	std::cout << "Host: " << _request.Host << std::endl;
	std::cout << "If_Match: " << _request.If_Match << std::endl;
	std::cout << "If_Modified_Since: " << _request.If_Modified_Since << std::endl;
	std::cout << "If_None_Match: " << _request.If_None_Match << std::endl;
	std::cout << "If_Unmodified_Since: " << _request.If_Unmodified_Since << std::endl;
	std::cout << "Max_Forwards: " << _request.Max_Forwards << std::endl;
	std::cout << "Proxy_Authorization: " << _request.Proxy_Authorization << std::endl;
	std::cout << "Range: " << _request.Range << std::endl;
	std::cout << "Referer: " << _request.Referer << std::endl;
	std::cout << "TE: " << _request.TE << std::endl;
	std::cout << "Content-Length: " << _request.Content_Length << std::endl;
	std::cout << "User_Agent: " << _request.User_Agent << std::endl
			  << std::endl;
}
