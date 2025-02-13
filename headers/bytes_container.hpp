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

	const char *get_data() const;
	int get_data_size() const;

	void fill(const char *, size_t);
	void fill(ct::iterator, ct::iterator);
	void seek(unsigned int);

	size_t safeGetline(std::string &);

	int find_last_of(const char *);
	int find_last_of(const char);

	// ------- substr ---------
	bytes_container subcontainer(size_t);
};

std::ostream &operator<<(std::ostream &out, const bytes_container &c);
