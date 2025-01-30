#include "Request.hpp"

Request::Request(char *http_package, unsigned int port) : in_port(port), _status(true)
{
	_brut_request = http_package;
	parse();
}

void Request::parse()
{
	// TODO: query string handle
	std::stringstream s;
	s.str(_brut_request);

	std::cout << "\n\n"
			  << _brut_request;
	std::string line;
	safeGetline(s, line);

	int f1 = line.find_first_of(' ');
	int f2 = line.find_last_of(' ');

	if (f1 == f2 || f1 == (int)std::string::npos)
	{
		_status = false;
		return;
	}

	// * get http method
	if (line.find("GET") == 0)
		_Type = GET;
	else if (line.find("POST") == 0)
		_Type = POST;

	_request["URI"] = line.substr(f1 + 1, f2 - f1 - 1);
	size_t qs = _request["URI"].find('?');
	if (qs != std::string::npos)
	{
		_request["request_string"] = _request["URI"].substr(_request["URI"].find('?'));
		_request["URI"] = _request["URI"].substr(0, _request["URI"].find('?'));
	}
	_request["Protocol"] = line.substr(f2 + 1);
	while (safeGetline(s, line) && line.length() > 0)
	{
		_request[line.substr(0, line.find_first_of(':'))] =
			line.substr(line.find_first_of(':') + 2);
	}
	parse_payload();
}

void Request::parse_payload()
{
	if (_request.find("Content-Length") != _request.end())
	{
		int f1 = _brut_request.find("\n\n") + 2;
		_request["Payload"] = _brut_request.substr(f1);
	}
}

// void Request::parse_params()
// {
// 	if (_Type == GET && _request["URI"].find('?') != std::string::npos)
// 	{
// 		std::string uri = _request["URI"].substr(0, _request["URI"].find("?"));
// 		std::stringstream s;
// 		s.str(_request["URI"].substr(_request["URI"].find("?") + 1));

// 		std::string param;
// 		while (std::getline(s, param, '&'))
// 		{
// 			_params[param.substr(0, param.find('='))] =
// 				param.substr(param.find('=') + 1);
// 		}
// 		_request["URI"] = uri;
// 	}
// }

const Map &Request::get_request() const
{
	return (_request);
}

const Map &Request::get_params() const
{
	return (_params);
}

const type_e &Request::get_type() const
{
	return (_Type);
}

const unsigned int &Request::get_in_port() const
{
	return (in_port);
}

Request::~Request()
{
}

std::ostream &operator<<(std::ostream &out, const Request &c)
{
	if (c.get_type() == GET)
		out << "GET request" << "\n";
	else if (c.get_type() == POST)
		out << "POST request" << "\n";

	for (std::map<std::string, std::string>::const_iterator it = c.get_request().begin();
		 it != c.get_request().end(); ++it)
		out << "[" << it->first << "]: \"" << it->second << "\"\n";

	out << "PARAMS\n";

	for (std::map<std::string, std::string>::const_iterator it = c.get_params().begin();
		 it != c.get_params().end(); ++it)
		out << it->first << " = " << it->second << "\n";
	return (out);
}

std::istream &safeGetline(std::istream &is, std::string &t)
{
	std::getline(is, t);
	t.erase(std::remove(t.begin(), t.end(), '\r'), t.end());
	return (is);
}
