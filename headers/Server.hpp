#ifndef SERVER_HPP
#define SERVER_HPP
#include "includes.hpp"

class Server
{
private:
    /* data */
public:
    Server(char *config);

    void start();
    void reload();
    ~Server();
};

#endif
