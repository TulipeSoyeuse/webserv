#pragma once
#define INCLUDE_HPP

#include <fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <cstring>
#include <string>
#include <fstream>
#include <ctime>
#include <limits>
#include <sstream>
#include <iterator>
#include <vector>
#include <list>

#include <string>
#include <cstdlib>
#include <map>
#include <iostream>
#include <ostream>
#include <sstream>
#include <algorithm>

#include <sys/socket.h> // For socket functions
#include <netinet/in.h> // For sockaddr_in
#include <cstdlib>		// For exit() and EXIT_FAILURE
#include <iostream>		// For cout
#include <unistd.h>		// For read
#include <sys/errno.h>
#include <poll.h>
#include <signal.h>
#include <cstdio>

#include <cctype>

#include <signal.h>

#include <sys/types.h>
#include <sys/wait.h>

#include "config_string.hpp"
#include "bytes_container.hpp"

class Server;

struct t_clean_p
{
	Server *webserv;
	struct sockaddr_in *sockaddr;
	int *sockfd;
	int num_fd;
};

extern char **environ;
extern bool does_file_exist(const std::string &name);
char **ft_split(char const *s, char c);
char **ft_freestrs(char **strs, size_t words);
void free_split(char **strs);
