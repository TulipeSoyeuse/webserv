#pragma once
#define HM_POPEN_HPP

#include "includes.hpp"
#include "Request.hpp"

enum CGI
{
	PHP,
	BASH,
	PYTHON,
};

class hm_popen
{
private:
	const Request &_Request;
	bool all_read;
	int good;
	int subprocess_stderr_fd;
	int subprocess_stdout_fd;
	const char *get_CGI_exec(const CGI &) const;
	void build_env(std::string &f);

public:
	hm_popen(std::string &, CGI, const Request &_request);
	~hm_popen();
	size_t read_out(bytes_container &);
	size_t read_err(bytes_container &);
	const bool &is_all_read() const;
	const int &is_good() const;
};
