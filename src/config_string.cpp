#include "config_string.hpp"

config_string::config_string(std::ifstream &f) : c(0)
{
	if (f.good())
	{
		f.ignore(std::numeric_limits<std::streamsize>::max());
		std::streamsize content_length = f.gcount();
		f.clear();
		f.seekg(0, std::ios_base::beg);
		char *payload = new char[content_length + 1];
		payload[content_length] = 0;
		f.read(payload, content_length);
		assign(payload);
		delete[] payload;
	}
	else
		std::cerr << "filestream not good\n";
}

std::string config_string::get_next_word(const size_t pos) const
{
	size_t i = -1;
	std::string res;
	bool start = false;

	for (std::string::const_iterator it = begin(); it != end(); ++it)
	{
		if (++i < pos || (isspace(*it) && !start))
			continue;
		else if (isspace(*it) && start)
			break;
		else
		{
			if (!start)
				start = true;
			res.push_back(*it);
		}
	}

	return (res);
}

std::string config_string::get_config_subpart(const size_t pos) const
{
	size_t cursor_start = find('{', pos) + 1;
	size_t cursor_end;

	if (cursor_end = find('}', pos), cursor_end != npos)
	{
		if (at(cursor_start) == '\n' && (cursor_start + 1) < length())
			cursor_start++;

		std::string res = substr(cursor_start, cursor_end - cursor_start);
		return (res);
	}
	else
	{
		std::string res;
		return (res);
	}
}

size_t config_string::get_server_name(const size_t pos) const
{
	if (get_next_word(pos) == "server")
	{
		size_t cursor = pos + 6;
		const char *data = this->c_str();
		while (isspace(data[cursor]) && data[cursor] != 0)
			cursor++;
		if (data[cursor] == '{')
			return (npos);
		return (cursor);
	}
	else
		return (npos);
}

std::string config_string::get_next_line()
{
	size_t find_return = find('\n', c);
	std::string res;

	if (find_return == npos)
	{
		if (c >= length())
			return (res);
		else
			res = substr(c);
	}
	else
		res = substr(c, find_return - c + 1);

	c = find_return + 1;
	return (res);
}

config_string::config_string() : c(0)
{
}

config_string::config_string(const std::string &s) : c(0)
{
	assign(s);
}

config_string::~config_string()
{
}
