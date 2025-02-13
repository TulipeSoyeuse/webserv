#pragma once

#include "includes.hpp"

class bytes_container
{
	typedef std::vector<char> ct;

private:
	ct _data;
	size_t _cursor;

public:
	bytes_container(/* args */);
	~bytes_container();
	void fill(const char *, size_t);
	void fill(ct::iterator, ct::iterator);
	void seek(unsigned int);
	size_t safeGetline(std::string &);
	int find_last_of(const char *);
	int find_last_of(const char);
	bytes_container subcontainer(size_t);
};
