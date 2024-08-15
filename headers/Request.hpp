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

struct request_t
{
	type_e Type;
	std::string URI;
	std::string PROTOCOL;
	std::string Accept;
	std::string Accept_Charset;
	std::string Accept_Encoding;
	std::string Accept_Language;
	std::string Authorization;
	std::string Expect;
	std::string From;
	std::string Host;
	std::string If_Match;
	std::string If_Modified_Since;
	std::string If_None_Match;
	std::string If_Unmodified_Since;
	std::string Max_Forwards;
	std::string Proxy_Authorization;
	std::string Range;
	std::string Referer;
	std::string TE;
	std::string User_Agent;
	std::string Payload;
	unsigned long Content_Length;
};

class Request
{
private:
	std::string _brut_request;

	std::map<std::string, std::string> params;
	request_t _request;

	void parse();
	void parse_header_param(std::string &line);

public:
	Request(char *http_package);
	~Request();

	void display_request();
};

#endif
