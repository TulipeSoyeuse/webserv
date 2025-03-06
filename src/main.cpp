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
	return (access(name.c_str(), R_OK) == 0);
}

int socket_read(int fd, bytes_container &s)
{
	// * data structure pollfd -> fd = file descriptor
	// * short events -> request events
	// * short revents -> returned events
	pollfd pfd;
	char buffer[4096];
	int _read = 0;
	int i;
	// * poll() -> similar to select(), take the data struct pollfd, the numbers of items in the fd array
	// (nfds), and the number of millisec that poll should block waiting for a fd to become ready
	// * POLL_IN -> there is data to read
	// * POLL_PRI -> There is some exceptional condition on the file descriptor
	pfd.events = POLLIN | POLLPRI;
	pfd.fd = fd;
	while (true)
	{
		int ready;
		if ((ready = poll(&pfd, 1, 30)) == -1)
		{
			perror("poll");
			return -1;
		}
		else if (!(pfd.revents & POLLIN) || ready == 0)
			break;

		memset(buffer, 0, 4096);
		i = read(fd, buffer, 4096);
		if (i <= 0)
			break;
		_read += i;
		s.fill(buffer, i);
	};
	return (_read);
}

int socket_write(int fd, const bytes_container &b)
{
	pollfd pfd;
	pfd.events = POLLOUT;
	pfd.fd = fd;
	int ready;
	if ((ready = poll(&pfd, 1, 30)) == -1)
	{
		perror("poll");
		return -1;
	}
	else if (!(pfd.revents & POLLOUT) || ready == 0)
		return 0;
	return (send(fd, b.get_data(), b.get_data_size(), 0));
}

int socket_write(int fd, const std::string &b)
{
	pollfd pfd;
	pfd.events = POLLOUT;
	pfd.fd = fd;
	int ready;
	if ((ready = poll(&pfd, 1, 30)) == -1)
	{
		perror("poll");
		return -1;
	}
	else if (!(pfd.revents & POLLOUT) || ready == 0)
		return 0;
	return (send(fd, b.data(), b.size(), 0));
}

int network_accept_any(fd_vecset &fds, struct sockaddr *addr, socklen_t *addrlen)
{
	std::vector<struct pollfd> pollfds(fds.size());
	for (size_t i = 0; i < fds.size(); ++i)
	{
		pollfds[i].fd = fds[i];
		pollfds[i].events = POLLIN;
	}

	int status = poll(pollfds.data(), fds.size(), 0);
	if (status < 0)
		return INVALID_SOCKET;

	// Look for the first file descriptor that is ready to read
	for (size_t i = 0; i < fds.size(); ++i)
		if (pollfds[i].revents & POLLIN)
			return accept4(fds[i], addr, addrlen, SOCK_NONBLOCK);

	return INVALID_SOCKET;
}

void close_connection(t_clean_p &t)
{
	for (fd_vecset::iterator it = t.sockfd.begin(); it < t.sockfd.end(); ++it)
	{
		if (*it > 0)
			close(*it);
	}

	delete[] t.sockaddr;
	delete t.webserv;
}

int main(int ac, char **argv)
{
	// * Signal handling: ^C to quit properly
	t_clean_p t;
	struct sigaction act;
	act.sa_handler = sign_handler;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	sigaction(SIGINT, &act, 0);

	// * call webserv constructor -> parse config file
	if (ac == 1)
		t.webserv = new Server("default.conf", false);
	else if (ac == 2)
		t.webserv = new Server(argv[1], false);
	else
	{
		std::cerr << "invalid number of argument\nUsage: ./webserv <optional: config file path>\n";
	}

	if (!t.webserv->is_conf_valid())
	{
		std::cerr << "Usage: ./webserv <optional: config file path>\n";
		return (1);
	}

	std::cout << "-------- listening ---------\n";
	// Create a socket(IPv4, TCP)

	// * get port parsed in config file
	const port_array &parray = t.webserv->get_ports();
	// * struct SOCKADDR_IN to define transport adress and port for AF_INET family
	t.sockaddr = new sockaddr_in[parray.size()];

	for (int i = 0; i < (int)parray.size(); i++)
	{
		// * AF_INET = used to designate the type of adresses that your socket can communicate with (Protocol v4)
		// * SOCK_STREAM -> for TCP
		t.sockfd.push_back(socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0));
		if (t.sockfd[i] == -1)
		{
			std::cout << "Failed to create socket n°" << i << " . errno: " << errno << std::endl;
			close_connection(t);
			return (1);
		}
		// * sin family = family adresses for tranort adress. Always be define on AF_INET
		t.sockaddr[i].sin_family = AF_INET;
		// * struct SIN_ADDR qui contient une adresse de transport IPv4 and INADDR_ANY flag can accept any incoming messages
		t.sockaddr[i].sin_addr.s_addr = INADDR_ANY;
		// * sin_port = port number of transport protocol
		// * The htons() function converts the unsigned short integer hostshort from host byte order to network byte order.
		t.sockaddr[i].sin_port = htons(parray[i]);

		int yes = 1;
		// * manipulate option for the socket referred to by the fd t.sockfd,
		// * SOL_SOCKET -> to manipulate options at the socket API level
		// * SO_REUSEADDR -> can force a socket to link to a port used by another socket. (for details https://learn.microsoft.com/fr-fr/windows/win32/winsock/using-so-reuseaddr-and-so-exclusiveaddruse)
		if (setsockopt(t.sockfd[i], SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes) == -1)
		{
			perror("setsockopt");
			close_connection(t);
			return (1);
		}
		// * It is necessary to assign a local address with bind() before a SOCK_STREAM socket can receive connections.
		if (bind(t.sockfd[i], (struct sockaddr *)(&t.sockaddr[i]), sizeof(t.sockaddr[i])) < 0)
		{
			std::cout << "Failed to bind to port " << parray[i] << ". errno: " << errno << std::endl;
			close_connection(t);
			return (1);
		}
		// * listen() -> The listen function places a socket in a state in which it is listening for an incoming connection.
		if (listen(t.sockfd[i], 20) < 0)
		{
			std::cout << "Failed to listen on socket n°" << i << ". errno: " << errno << std::endl;
			close_connection(t);
			return (1);
		}
	}
	// * -> get size of all sockadress
	socklen_t addrlen = sizeof(t.sockaddr) * parray.size();

	std::cout << "Enter 'exit' to quit, or anything else to continue: ";
	while (1)
	{
		// std::string userInput;
		// if (std::cin.rdbuf()->in_avail())
		// {
		// 	std::getline(std::cin, userInput);
		// 	if (userInput == "exit")
		// 	{
		// 		std::cout << "Exiting loop..." << std::endl;
		// 		break; // Exit the loop
		// 	}
		// 	else
		// 	{
		// 		std::cout << "You entered: " << userInput << std::endl;
		// 	}
		// }
		// * accept the connexion with a ready socket
		int connection = network_accept_any(t.sockfd, (struct sockaddr *)t.sockaddr, &addrlen);
		if (connection == INVALID_SOCKET)
			continue;

		std::cout << "incomming fd:" << incomming_fd << "\n";
		int port = parray[incomming_fd];

		// Read from the connection
		// * buffer to read request
		// * read the data in the socket (cd comment in function)
		bytes_container brut_request;
		if (socket_read(connection, brut_request) == -1)
			continue;
		char hostname[30];
		// * The gethostname function get the local computer's standard host name.
		gethostname(hostname, 30);
		std::cout << "------------------------------------------\n"
				  << "socket port: " << port << "\n"
				  << "hostname: " << hostname << "\n";
		// 		  << "------------- BRUT REQUEST ---------------\n"
		// 		  << brut_request << "\n"
		// 		  << "------------------------------------------" << std::endl;

		// * Request class : parsew request
		std::cout << brut_request.get_data_size() << "\n";
		Request r(brut_request, port);
		std::cout << "-------------- PARSED REQUEST -----------\n"
				  << r
				  << "------------------------------------------" << std::endl;
		// * Response class :
		Response resp(r, *t.webserv);
		std::cout << "---------------- RESPONSE ---------------\n";
		std::cout << resp;
		std::cout << "------------------ END -------------------"
				  << std::endl;
		int bw = socket_write(connection, resp.get_response());
		if (bw == -1 || bw == 0)
			continue;
		if (resp.is_chunked())
		{
			bytes_container b;
			while (resp.get_next_chunk(b) != -1)
			{
				std::string s;
				b.safeGetline(s);
				b.seek(0);
				std::cout << s << '\n';
				bw = socket_write(connection, b);
				if (bw == -1 || bw == 0)
					break;
			}
		}
		close(connection);
	}

	// Close the connections
	close_connection(t);
}
