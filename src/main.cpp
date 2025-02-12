#include "includes.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "Server.hpp"

#define INVALID_SOCKET -1

int incomming_fd = 0;

void sign_handler(int sig)
{
	(void)sig;
	std::cerr << "\nSIGINT " << " recevied closing server....\n";
	exit(EXIT_SUCCESS);
}

extern bool does_file_exist(const std::string &name)
{
	return (access(name.c_str(), F_OK) != -1);
}

int socket_read(int fd, t_byte &s)
{
	// * data structure pollfd -> fd = file descriptor
	// * short events -> request events
	// * short revents -> returned events
	pollfd pfd;
	char buffer[4096];
	int _read = 0;
	int i;
	// * poll() -> similar to select(), take the data struct pollfd, the numbers of items in the fd array (nfds), and the number of millisec that poll should block waiting for a fd to become ready
	// * POLL_IN -> there is data to read
	// * POLL_PRI -> There is some exceptional condition on the file descriptor
	pfd.events = POLLIN | POLLPRI;
	pfd.fd = fd;
	while (true)
	{
		int pret;
		if ((pret = poll(&pfd, 1, 30)) == -1)
		{
			perror("poll");
			return -1;
		}
		else if (!(pfd.revents & POLLIN) || pret == 0)
			break;

		memset(buffer, 0, 4096);
		std::cout << "READ CALLED" << "\n";
		i = read(fd, buffer, 4096);
		if (i <= 0)
			break;
		_read += i;
		std::cout << "BYTES_READ " << i << "-" << _read << "\n";
		s.insert(s.end(), buffer, buffer + i);
	};
	return (_read);
}

int network_accept_any(int fds[], unsigned int count,
					   struct sockaddr *addr, socklen_t *addrlen)
{
	fd_set readfds;
	int maxfd, fd;
	unsigned int i;
	int status;

	// * -> FD_ZERO -> like memset
	FD_ZERO(&readfds);
	maxfd = -1;
	for (i = 0; i < count; i++)
	{
		// * ajoute un descripteur de fichier a readfds. Si le descripteur de fichier existe déjà dans le jeu et que fd_count du fd_set est inférieur à FD_SETSIZE, un doublon est ajouté.
		FD_SET(fds[i], &readfds);
		if (fds[i] > maxfd)
			maxfd = fds[i];
	}
	// *  select -> use to supervise efficiently fds to check if one of them is ready(=if enter and exit become possible)
	status = select(maxfd + 1, &readfds, NULL, NULL, NULL);
	if (status < 0)
		return INVALID_SOCKET;
	fd = INVALID_SOCKET;
	for (i = 0; i < count; i++)
		// * FD_ISSETtest to see if a fd is part of the set
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
		// * accept a connexion on a socket
		return accept4(fd, addr, addrlen, SOCK_NONBLOCK);
	}
}

int main()
{
	// TODO: rework connection flow
	// TODO: handle PUT
	// TODO:

	// * Signal handling: ^C to quit properly
	struct sigaction act;
	act.sa_handler = sign_handler;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	sigaction(SIGINT, &act, 0);
	sigaction(SIGSTOP, &act, 0);

	// * call webserv constructor -> parse config file
	Server webserv("test.conf", false);
	if (!webserv.is_conf_valid())
	{
		std::cerr << "exiting..." << std::endl;
		return (1);
	}

	std::cout << "-------- listening ---------\n";
	// Create a socket(IPv4, TCP)

	// * get port parsed in config file
	const port_array &parray = webserv.get_ports();
	// * get number of port
	int num_fd = webserv.get_server_count();
	// * struct SOCKADDR_IN to define transport adress and port for AF_INET family
	struct sockaddr_in *sockaddr = new sockaddr_in[num_fd];
	int *sockfd = new int[num_fd];

	for (int i = 0; i < num_fd; i++)
	{
		// * AF_INET = used to designate the type of adresses that your socket can communicate with (Protocol v4)
		// * SOCK_STREAM -> for TCP
		sockfd[i] = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
		if (sockfd[i] == -1)
		{
			std::cout << "Failed to create socket n°" << i << " . errno: " << errno << std::endl;
			exit(EXIT_FAILURE);
		}
		// Listen to port 9999 on any address
		// * sin family = family adresses for tranort adress. Always be define on AF_INET
		sockaddr[i].sin_family = AF_INET;
		// * struct SIN_ADDR qui contient une adresse de transport IPv4 and INADDR_ANY flag can accept any incoming messages
		sockaddr[i].sin_addr.s_addr = INADDR_ANY;
		// * sin_port = port number of transport protocol
		// * The htons() function converts the unsigned short integer hostshort from host byte order to network byte order.
		sockaddr[i].sin_port = htons(parray[i]);

		int yes = 1;
		// * manipulate option for the socket referred to by the fd sockfd,
		// * SOL_SOCKET -> to manipulate options at the socket API level
		// * SO_REUSEADDR -> can force a socket to link to a port used by another socket. (for details https://learn.microsoft.com/fr-fr/windows/win32/winsock/using-so-reuseaddr-and-so-exclusiveaddruse)
		if (setsockopt(sockfd[i], SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes) == -1)
		{
			perror("setsockopt");
			exit(EXIT_FAILURE);
		}
		// * It is necessary to assign a local address with bind() before a SOCK_STREAM socket can receive connections.
		if (bind(sockfd[i], (struct sockaddr *)&(sockaddr[i]), sizeof(sockaddr[i])) < 0)
		{
			std::cout << "Failed to bind to port " << parray[i] << ". errno: " << errno << std::endl;
			exit(EXIT_FAILURE);
		}
		// * listen() -> The listen function places a socket in a state in which it is listening for an incoming connection.
		if (listen(sockfd[i], 10) < 0)
		{
			std::cout << "Failed to listen on socket n°" << i << ". errno: " << errno << std::endl;
			exit(EXIT_FAILURE);
		}
	}
	// * -> get size of all sockadress
	socklen_t addrlen = sizeof(sockaddr) * num_fd;

	while (1)
	{
		// * accept the connexion with a ready socket
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
		// * buffer to read request
		// * read the data in the socket (cd comment in function)
		t_byte brut_request;
		socket_read(connection, brut_request);
		char hostname[30];
		// * The gethostname function get the local computer's standard host name.
		gethostname(hostname, 30);
		std::cout << "------------------------------------------\n"
				  << "socket port: " << port << "\n"
				  << "hostname: " << hostname << "\n";
		//   << "------------------------------------------\n"
		//   << brut_request << "\n"
		//   << "------------------------------------------" << std::endl;

		// * Request class : parsew request
		Request r(brut_request, port);
		std::cout << r;
		std::cout << "------------------------------------------" << std::endl;
		// * Response class :
		Response resp(r, webserv);
		std::cout << "--------------------START----------------------\n";
		std::cout << resp;
		std::cout << "--------------------END------------------------"
				  << std::endl;
		send(connection, resp.get_response().data(), resp.get_response().size(), 0);
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
