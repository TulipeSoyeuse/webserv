#ifndef CONFIG_STRING_HPP
#define CONFIG_STRING_HPP

#include "includes.hpp"

class config_string : public std::string
{
private:
	/* data */
public:
	config_string(/* args */);
	~config_string();
	std::string &get_next_word(const size_t pos) const;
	std::string &get_config_subpart(const size_t pos) const;
	size_t get_server_name(const size_t pos) const;
};

#endif
