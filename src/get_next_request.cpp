#include "get_next_request.hpp"

int get_next_request::socket_read()
{
	bzero(read_array, 4096);
	// * data structure pollfd -> fd = file descriptor
	// * short events -> request events
	// * short revents -> returned events
	pollfd pfd;

	// * poll() -> similar to select(), take the data struct pollfd, the numbers of items in the fd array (nfds),
	// * and the number of millisec that poll should block waiting for a fd to become ready
	// * POLL_IN -> there is data to read
	// * POLL_PRI -> There is some exceptional condition on the file descriptor
	pfd.events = POLL_IN | POLL_PRI;
	pfd.fd = fd;
	if (poll(&pfd, 1, 30) == 1)
		return (read(fd, read_array, 4096));
	return (-1);
}

int get_next_request::populate_buffer()
{
	int t;
	if ((t = socket_read()) == -1)
		return (-1);
	else
	{
		buffer.append(read_array, t);
		return (t);
	}
}

// @brief add part or complete request to res
e_read_buffer get_next_request::read_request_from_buffer()
{
	int v;

	if (buffer.empty())
	{
		std::cout << "BUFFER_EMPTY\n";
		return (BUFFER_EMPTY);
	}
	else if (v = buffer.find("\r\n\0", 0, 3), v != npos || (v = buffer.find("\n\0", 0, 2), v != npos))
	{
		res.assign(buffer, 0, v);
		buffer.erase(0, v);
		std::cout << "COMPLETE_REQUEST_READ\n";
		return (COMPLETE_REQUEST_READ);
	}
	else
	{
		std::cout << "IMCOMPLETE_REQUEST\n";
		return (IMCOMPLETE_REQUEST);
	}
}

// @brief populate the constructor string reference with a request if true is return.
// if false, do nothing.
bool get_next_request::next()
{
	if (!_status)
		return (false);
	e_read_buffer e = read_request_from_buffer();
	if (e == COMPLETE_REQUEST_READ)
		return (true);
	else if (e == IMCOMPLETE_REQUEST)
	{
		if (populate_buffer() == -1)
		{
			_status = false;
			return (false);
		}
		else
			next();
	}
}

get_next_request::get_next_request(socketfd fd, std::string &res) : _status(true), res(res), fd(fd)
{
	std::cout << "acknowledge connexion...\n"
			  << "ready to perform I/O action\n";
}

get_next_request::~get_next_request()
{
}
