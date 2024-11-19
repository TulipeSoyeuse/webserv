#include "includes.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "Server.hpp"

#define INVALID_SOCKET -1

int incomming_fd = 0;

void sign_handler(int sig)
{
	std::cerr << "\nSIGINT " << sig << " recevied closing server....\n";
	exit(EXIT_FAILURE);
}

int socket_read(int fd, char *buffer, size_t size)
{
	pollfd pfd;

	pfd.events = POLL_IN | POLL_PRI;
	pfd.fd = fd;
	if (poll(&pfd, 1, 30) == 1)
		return (read(fd, buffer, size));
	return (-1);
}

int network_accept_any(int fds[], unsigned int count,
					   struct sockaddr *addr, socklen_t *addrlen)
{
	fd_set readfds;
	int maxfd, fd;
	unsigned int i;
	int status;

	FD_ZERO(&readfds);
	maxfd = -1;
	for (i = 0; i < count; i++)
	{
		FD_SET(fds[i], &readfds);
		if (fds[i] > maxfd)
			maxfd = fds[i];
	}
	status = select(maxfd + 1, &readfds, NULL, NULL, NULL);
	if (status < 0)
		return INVALID_SOCKET;
	fd = INVALID_SOCKET;
	for (i = 0; i < count; i++)
		if (FD_ISSET(fds[i], &readfds))
		{
			fd = fds[i];
			break;
		}
	if (fd == INVALID_SOCKET)
		return INVALID_SOCKET;
	else
	{
		incomming_fd = fd;
		return accept(fd, addr, addrlen);
	}
}

int main()
{
	// Signal handling:
	struct sigaction act;
	act.sa_handler = sign_handler;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	sigaction(SIGINT, &act, 0);
	sigaction(SIGSTOP, &act, 0);

	Server webserv("test.conf", false);

	// Create a socket(IPv4, TCP)
	const port_array &parray = webserv.get_ports();
	int num_fd = webserv.get_server_count();
	struct sockaddr_in *sockaddr = new sockaddr_in[num_fd];
	int *sockfd = new int[num_fd];

	for (int i = 0; i < num_fd; i++)
	{
		sockfd[i] = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
		if (sockfd[i] == -1)
		{
			std::cout << "Failed to create socket n°" << i << " . errno: " << errno << std::endl;
			exit(EXIT_FAILURE);
		}

		// Listen to port 9999 on any address
		sockaddr[i].sin_family = AF_INET;
		sockaddr[i].sin_addr.s_addr = INADDR_ANY;
		sockaddr[i].sin_port = htons(parray[i]);

		int yes = 1;
		if (setsockopt(sockfd[i], SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes) == -1)
		{
			perror("setsockopt");
			exit(EXIT_FAILURE);
		}
		if (bind(sockfd[i], (struct sockaddr *)&(sockaddr[i]), sizeof(sockaddr[i])) < 0)
		{
			std::cout << "Failed to bind to port " << parray[i] << ". errno: " << errno << std::endl;
			exit(EXIT_FAILURE);
		}
		if (listen(sockfd[i], 10) < 0)
		{
			std::cout << "Failed to listen on socket n°" << i << ". errno: " << errno << std::endl;
			exit(EXIT_FAILURE);
		}
	}

	socklen_t addrlen = sizeof(sockaddr) * num_fd;

	while (1)
	{
		int connection = network_accept_any(sockfd, num_fd, (struct sockaddr *)sockaddr, &addrlen);
		if (connection == INVALID_SOCKET)
		{
			std::cout << "Failed to grab connection. errno: " << errno
					  << std::endl;
			continue;
		}
		std::cout << "incomming fd:" << incomming_fd << "\n";
		int port = parray[incomming_fd - 3];

		// Read from the connection
		char buffer[2048];
		std::memset(buffer, 0, 2048);
		socket_read(connection, buffer, 2048);
		char hostname[30];
		gethostname(hostname, 30);
		std::cout << "------------------------------------------\n"
				  << "socket port: " << port << "\n"
				  << "hostname: " << hostname << "\n"
				  << "------------------------------------------\n"
				  << buffer << "\n"
				  << "------------------------------------------" << std::endl;
		Request r(buffer, port);
		// std::cout << r;
		// std::cout << "------------------------------------------" << std::endl;
		if (r._status == true)
		{
			Response resp(r, webserv);
			std::cout << resp << std::endl;
			std::cout << "------------------------------------------\nEND\n\n"
					  << std::endl;
			send(connection, resp.get_response().data(), resp.get_response().size(), 0);
		}
		close(connection);
	}

	// Close the connections
	for (int i = 0; i < num_fd; i++)
	{
		close(sockfd[i]);
	}
	delete[] sockfd;
	delete[] sockaddr;
}
