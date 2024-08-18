#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "Request.hpp"

class Response
{
private:
	Request& _request;
	std::string _response;

	void generate_response();
public:
	Response(Request &request);
	~Response();

	std::string &get_response();
};

#endif
