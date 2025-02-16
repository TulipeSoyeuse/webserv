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

class Request
{
private:
	bytes_container &_brut_request;

	type_e _Type;
	Map _request;
	bytes_container _payload;

	const unsigned int in_port;

	void parse();
	void parse_payload();

public:
	Request(bytes_container &, unsigned int port);
	~Request();

	bool _status;
	const Map &get_headers() const;
	const bytes_container &get_body() const;
	const bytes_container &get_brut_request() const;
	const type_e &get_type() const;
	const unsigned int &get_in_port() const;
};

std::ostream &operator<<(std::ostream &out, const Request &c);
std::istream &safeGetline(std::istream &is, std::string &t);
