#include "includes.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "Server.hpp"

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

	Server webserv("test.conf", false);

	// Create a socket(IPv4, TCP)
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
		exit(EXIT_FAILURE);
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
		char buffer[2048];
		std::memset(buffer, 0, 500);
		socker_read(connection, buffer, 500);
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
		Response resp(r, webserv);
		std::cout << resp << std::endl;
		std::cout << "------------------------------------------\nEND\n\n"
				  << std::endl;
		send(connection, resp.get_response().data(), resp.get_response().size(), 0);
		close(connection);
	}
	// Close the connections
	close(sockfd);
}
