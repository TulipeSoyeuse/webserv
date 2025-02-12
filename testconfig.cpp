#include <iostream>
#include <sstream>
#include <map>
#include <string>

typedef std::map<std::string, std::string> Map;

typedef std::pair<std::string, std::pair<std::string, Map>> server_p;

// unique server : dict of key (string) / value (server_p)
typedef std::map<std::string, std::pair<std::string, Map>> server_m;

class config_string
{
public:
    server_m create_server(const std::string &config); // note the const ref for the argument
    void parse_config(std::string::const_iterator &it, std::string::const_iterator &end, server_m &lst, std::string str);
};

server_m config_string::create_server(const std::string &config)
{
    std::string::const_iterator it = config.begin();
    std::string::const_iterator end = config.end();
    server_m lst;
    parse_config(it, end, lst, config);
    return lst;
}

void config_string::parse_config(std::string::const_iterator &it, std::string::const_iterator &end, server_m &lst, std::string str)
{
    server_p res;
    std::string key;
    bool begin = false;

    // -- recup le keyword -----
    while (it != end)
    {
        if (isspace(*it))
        {
            if (begin)
                break;
        }
        else
        {
            begin = true;
            if (*it != '\n' && !isspace(*it))
                key.push_back(*it);
        }
        it++;
    }
    std::cout << key << std::endl;

    // ---- il y a t'il une subpart ------------

    //- on passe les espaces
    while (it != end && isspace(*it))
    {
        it++;
    }

    // -- soit on a un word , soit une bracket, soit une erreur
    if (it != end && *it != '\n' && *it != '{' && begin == true)
    {
        std::string value;
        while (it != end && *it != '\n') {
            value.push_back(*it);
            it++;
        }
        if(!value.empty()) {
            res.first = key;
            res.second = Map
        }


    else
    {
        std::cout << "subpart" << std::endl;
    }
}

int main()
{
    std::string config =
        "host www.neogym.fr\n"
        "port 9997\n"
        "proto GET,POST,PUT\n"
        "\n"
        "error_page {\n"
        "    404 error/error_404.html\n"
        "    400 error/error_400.html\n"
        "    403 error/error_403.html\n"
        "    500 error/error_500.html\n"
        "    505 error/error_505.html\n"
        "}\n"
        "\n"
        "route   .\n"
        "location /site-test3\n"
        "upload_path /upload\n"
        "client_size 0\n"
        "autoindex on\n";

    // Création d'un objet config_string pour appeler la méthode
    config_string cfg;

    // Appel de la méthode create_server avec l'objet
    server_m parsed_config = cfg.create_server(config);

    // Affichage des résultats
    // for (const auto &entry : parsed_config)
    // {
    //     std::cout << "Key: " << entry.first << " -> " << entry.second.first << std::endl;
    //     if (!entry.second.second.first.empty())
    //     {
    //         std::cout << "  Sub-Key: "
    //                   << entry.second.second.first
    //                   << " -> "
    //                   << entry.second.second.second.first << ", "
    //                   << entry.second.second.second.second
    //                   << std::endl;
    //     }
    // }

    return 0;
}
