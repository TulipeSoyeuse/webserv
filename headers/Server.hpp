#ifndef SERVER_HPP
#define SERVER_HPP
#include "includes.hpp"
#include "config_string.hpp"

typedef std::vector<std::map<std::string, std::string> > Server_lst;

class Server
{
private:
    bool _status;
    config_string _config;
    Server_lst _servers;

    bool read_config();
    bool add_serv(std::string s);

public:
    Server(char *config);

    void start();
    void reload();
    ~Server();
};

#endif
