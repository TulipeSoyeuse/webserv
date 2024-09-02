#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <string>
#include <cstdlib>
#include <map>
#include <iostream>
#include <ostream>
#include <sstream>
#include <algorithm>

enum type_e
{
	GET,
	POST
};

typedef std::map<std::string, std::string> Map;

class Request
{
private:
	std::string _brut_request;

	type_e _Type;
	Map _params;
	Map _request;

	void parse();
	void parse_params();
	void parse_payload();

public:
	Request(char *http_package);
	~Request();

	bool _status;
	const Map &get_request() const;
	const Map &get_params() const;
	const type_e &get_type() const;
};

std::ostream &operator<<(std::ostream &out, const Request &c);
std::istream &safeGetline(std::istream &is, std::string &t);

#endif
