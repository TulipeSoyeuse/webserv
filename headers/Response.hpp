#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "Request.hpp"

class Response
{
private:
	const Request &_request;
	std::string _response;

	int status_code;
	std::string status_line;

public:
	Response(const Request &request);
	~Response();

	const int &get_status();
};

#endif
