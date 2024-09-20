#include "config_string.hpp"

config_string::config_string(/* args */)
{
}

config_string::~config_string()
{
}

std::string &config_string::get_next_word(const size_t pos) const
{
	const char *data = this->c_str();
	size_t cursor_start = pos;

	while (isspace(data[cursor_start]) && data[cursor_start] != 0)
		cursor_start++;
	size_t cursor_end = cursor_start;
	while (!isspace(data[cursor_end]) && data[cursor_end] != 0)
		cursor_start++;

	std::string res = substr(cursor_start, cursor_end - cursor_start);
	return (res);
}

std::string &config_string::get_config_subpart(const size_t pos) const
{
	const char *data = this->c_str();
	size_t cursor_start = find('{');
	size_t cursor_end;

	if (cursor_end = find('}', pos), cursor_end != npos)
	{
		std::string res = substr(pos, cursor_end - cursor_start);
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
