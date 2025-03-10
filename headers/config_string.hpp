#pragma once

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
	config_string &operator=(const config_string &);

	std::string get_next_word(const size_t pos) const;
	std::string get_config_subpart(const size_t pos) const;
	size_t get_server_name(const size_t pos) const;
	std::string get_next_line();
	std::string get_next_conf();
	//--------- GETTER ---------
	std::string &get_str();
	void set_c(size_t n);
};

char *ft_itoa(int n);
