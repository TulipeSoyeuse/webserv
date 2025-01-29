#pragma once
#define HM_POPEN_HPP

#include "includes.hpp"

enum CGI
{
	PHP,
	BASH,
	PYTHON,
};

class hm_popen
{
private:
	bool all_read;
	bool good;
	int subprocess_stderr_fd;
	int subprocess_stdout_fd;
	const char *get_CGI_exec(const CGI &) const;

public:
	hm_popen(std::string &, CGI);
	~hm_popen();
	size_t read_out(char *, const size_t);
	size_t read_err(char *, const size_t);
	const bool &is_all_read() const;
	const bool &is_good() const;
};
