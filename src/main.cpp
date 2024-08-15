#include <sys/socket.h> // For socket functions
#include <netinet/in.h> // For sockaddr_in
#include <cstdlib>		// For exit() and EXIT_FAILURE
#include <iostream>		// For cout
#include <unistd.h>		// For read
#include <sys/errno.h>
#include <poll.h>
#include <signal.h>

#include "Request.hpp"

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
	sockaddr.sin_port = htons(9999); // htons is necessary to convert a number to
									 // network byte order

	if (bind(sockfd, (struct sockaddr *)&sockaddr, sizeof(sockaddr)) < 0)
	{
		std::cout << "Failed to bind to port 9999. errno: " << errno << std::endl;
		exit(EXIT_FAILURE);
	}

	// Start listening. Hold at most 10 connections in the queue
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
		socker_read(connection, buffer, 100);

		Request r(buffer);
		r.display_request();

		// Send a message to the connection
		std::string response = "Good talking to you\n";
		send(connection, response.c_str(), response.size(), 0);
		close(connection);
	}
	// Close the connections
	close(sockfd);
}
