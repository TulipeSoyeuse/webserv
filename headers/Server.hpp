#ifndef SERVER_HPP
#define SERVER_HPP
#include "includes.hpp"
#include "config_string.hpp"

typedef std::vector<std::map<std::string, std::string> > Server_lst;

class Server
{
private:
    // -------- DEBUG ----------
    bool _debug;
    void display_params();
    // -------------------------
    // -------- CONFIG ---------
    config_string _config;
    Server_lst _servers;
    bool read_config();
    std::string _empty_res;
    std::pair<std::string, std::string> parse_config_line(config_string);
    // --------------------------
    // ------- ERROR CHECK ------
    bool _valid_conf;
    void configuration_checking();
    // --------------------------

public:
    Server(const char *config, bool debug);
    const std::string &get_param(const std::string &, const std::string &);
    const std::map<std::string, std::string> &get_config(std::string &) const;
    // void start();
    //  void reload();
    ~Server();
};

#endif
