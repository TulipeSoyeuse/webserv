#pragma once
#define REQUEST_HPP
#include "includes.hpp"

enum type_e
{
	GET,
	POST,
	PUT,
	DELETE,
	UNKNOWN,
};

typedef std::map<std::string, std::string> Map;
typedef std::vector<char> t_byte;

class Request
{
private:
	t_byte _brut_request;
	std::string str_request;

	type_e _Type;
	Map _request;

	const unsigned int in_port;

	void parse();
	void parse_payload();

public:
	Request(t_byte &, unsigned int port);
	~Request();

	bool _status;
	const Map &get_request() const;
	const type_e &get_type() const;
	const unsigned int &get_in_port() const;
};

std::ostream &operator<<(std::ostream &out, const Request &c);
std::istream &safeGetline(std::istream &is, std::string &t);
