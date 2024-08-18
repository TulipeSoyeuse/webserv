#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <string>
#include <cstdlib>
#include <map>

enum type_e
{
	GET,
	POST
};

class Request
{
private:
	std::string _brut_request;

	type_e _Type;
	std::map<std::string, std::string> _params;
	std::map<std::string, std::string> _request;

	void parse();

public:
	Request(char *http_package);
	~Request();

	void display_request() const;
	const std::map<std::string, std::string> &get_request() const;
	const std::map<std::string, std::string> &get_params() const;
	const type_e &get_type() const;
};

#endif
