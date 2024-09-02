#include <sys/socket.h> // For socket functions
#include <netinet/in.h> // For sockaddr_in
#include <cstdlib>		// For exit() and EXIT_FAILURE
#include <iostream>		// For cout
#include <unistd.h>		// For read
#include <sys/errno.h>
#include <poll.h>
#include <signal.h>
#include <cstdio>

#include "Request.hpp"
#include "Response.hpp"

int socker_read(int fd, char *buffer, size_t size)
{
	pollfd pfd;

	pfd.events = POLL_IN | POLL_PRI;
	pfd.fd = fd;
	if (poll(&pfd, 1, 30) == 1)
		return (read(fd, buffer, size));
	return (-1);
}

int main()
{
	// Create a socket (IPv4, TCP)
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1)
	{
		std::cout << "Failed to create socket. errno: " << errno << std::endl;
		exit(EXIT_FAILURE);
	}

	// Listen to port 9999 on any address
	sockaddr_in sockaddr;
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_addr.s_addr = INADDR_ANY;
	sockaddr.sin_port = htons(9999);

	int yes = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes) == -1)
	{
		perror("setsockopt");
		exit(1);
	}
	if (bind(sockfd, (struct sockaddr *)&sockaddr, sizeof(sockaddr)) < 0)
	{
		std::cout << "Failed to bind to port 9999. errno: " << errno << std::endl;
		exit(EXIT_FAILURE);
	}

	if (listen(sockfd, 10) < 0)
	{
		std::cout << "Failed to listen on socket. errno: " << errno << std::endl;
		exit(EXIT_FAILURE);
	}

	// Grab a connection from the queue
	unsigned long addrlen = sizeof(sockaddr);
	while (1)
	{
		int connection = accept(sockfd, (struct sockaddr *)&sockaddr, (socklen_t *)&addrlen);
		if (connection < 0)
		{
			std::cout << "Failed to grab connection. errno: " << errno << std::endl;
			exit(EXIT_FAILURE);
		}

		// Read from the connection
		char buffer[100];
		std::memset(buffer, 0, 100);
		socker_read(connection, buffer, 100);
		char hostname[30];
		gethostname(hostname, 30);
		std::cout << "------------------------------------------\n"
				  << "socket: " << connection << "\n"
				  << "hostname: " << hostname << "\n"
				  << "------------------------------------------\n"
				  << buffer << "\n"
				  << "------------------------------------------" << std::endl;
		Request r(buffer);
		std::cout << r;
		std::cout << "------------------------------------------" << std::endl;
		Response resp(r);
		std::cout << resp << std::endl;
		std::cout << "------------------------------------------\nEND\n\n"
				  << std::endl;
		send(connection, resp._response.c_str(), resp._response.length(), 0);
		close(connection);
	}
	// Close the connections
	close(sockfd);
}
