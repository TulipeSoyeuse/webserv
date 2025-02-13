#include "Request.hpp"

Request::Request(bytes_container &s, unsigned int port) : _brut_request(s), in_port(port), _status(true)
{
	parse();
}

void Request::parse()
{

	std::string line;
	_brut_request.safeGetline(line);

	int f1 = line.find_first_of(' ');
	int f2 = line.find_last_of(' ');

	if (f1 == f2 || f1 == (int)std::string::npos)
	{
		_status = false;
		return;
	}

	// * get http method
	// TODO : gerer method not allowed;
	if (line.find("GET") == 0)
		_Type = GET;
	else if (line.find("POST") == 0)
		_Type = POST;
	else if (line.find("PUT") == 0)
		_Type = PUT;
	else if (line.find("DELETE") == 0)
		_Type = DELETE;
	else
		_Type = UNKNOWN;

	_request["URI"] = line.substr(f1 + 1, f2 - f1 - 1);
	size_t qs = _request["URI"].find('?');
	if (qs != std::string::npos)
	{
		_request["request_string"] = _request["URI"].substr(qs + 1);
		_request["URI"] = _request["URI"].substr(0, qs);
	}
	_request["Protocol"] = line.substr(f2 + 1);
	while (_brut_request.safeGetline(line) && line.length() > 0)
	{
		_request[line.substr(0, line.find_first_of(':'))] =
			line.substr(line.find_first_of(':') + 2);
	}
	parse_payload();
}

void Request::parse_payload()
{
	if (_request.find("Content-Length") != _request.end() &&
		_request.find("Content-Length")->second != "0")
	{
		int f1 = _brut_request.find_last_of("\r\n") + 1;
		payload = _brut_request.subcontainer(f1);
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
		if (it->first != "Payload")
			out << "[" << it->first << "]: \"" << it->second << "\"\n";

	return (out);
}

std::istream &safeGetline(std::istream &is, std::string &t)
{
	std::getline(is, t);
	t.erase(std::remove(t.begin(), t.end(), '\r'), t.end());
	return (is);
}
