#include "includes.hpp"

typedef unsigned int socketfd;
enum e_read_buffer
{
	BUFFER_EMPTY,
	COMPLETE_REQUEST_READ,
	IMCOMPLETE_REQUEST,
};

#define npos std::string::npos

// @brief the idea is to have a blocking class which return a complete request
// on each call to "next" until "read" timeout or client end connexion
class get_next_request
{
private:
	bool _status;
	std::string buffer;
	std::string &res;
	socketfd fd;
	char read_array[4096];

	int socket_read();
	e_read_buffer read_request_from_buffer();
	int populate_buffer();

public:
	get_next_request(socketfd, std::string &);
	~get_next_request();
	bool next();
};
