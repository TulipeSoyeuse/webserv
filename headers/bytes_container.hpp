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

	void clear();
	// put method ---------------------
	void fill(const char *, size_t);
	void fill(ct::iterator, ct::iterator);
	void fill(char);
	void seek(unsigned int);

	// read method ---------------------
	size_t safeGetline(std::string &);
	size_t read(std::string &, size_t);
	size_t read(bytes_container &, size_t);

	int find_last_of(const char *);
	int find_last_of(const char);

	// ------- substr ---------
	bytes_container subcontainer(size_t);
	bytes_container subcontainer();
};

std::ostream &operator<<(std::ostream &out, const bytes_container &c);
