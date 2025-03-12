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

#include <sys/types.h>
#include <sys/socket.h> // For socket functions
#include <netinet/in.h> // For sockaddr_in
#include <cstdlib>
#include <iostream>
#include <unistd.h>
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

#define TIME_OUT 1
#define ERROR_CONFIG 2

class Server;

typedef std::vector<int> fd_vecset;

struct t_clean_p
{
	Server *webserv;
	struct sockaddr_in *sockaddr;
	fd_vecset sockfd;
};

extern char **environ;
extern bool does_file_exist(const std::string &name);
char **ft_split(char const *s, char c);
char **ft_freestrs(char **strs, size_t words);
void free_split(char **strs);
