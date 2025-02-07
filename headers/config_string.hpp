#pragma once
#define CONFIG_STRING_HPP

#include "includes.hpp"

class config_string
{
private:
	std::string in;
	size_t c;

public:
	config_string();
	config_string(std::ifstream &f);
	config_string(const std::string &s);
	~config_string();
	std::string get_next_word(const size_t pos) const;
	std::string get_config_subpart(const size_t pos) const;
	size_t get_server_name(const size_t pos) const;
	std::string get_next_line();
	//--------- GETTER ---------
	std::string &get_str();
};

char *ft_itoa(int n);
