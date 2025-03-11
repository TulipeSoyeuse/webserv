#include "config_string.hpp"
#include "Server.hpp"

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
		in.assign(payload);
		delete[] payload;
	}
	else
		std::cerr << "filestream not good\n";
}

config_string &config_string::operator=(const config_string &cs)
{
	in = cs.in;
	c = cs.c;
	return (*this);
}

void config_string::set_c(size_t n)
{
	c = n;
}

std::string config_string::get_next_word(const size_t pos) const
{
	size_t i = -1;
	std::string res;
	bool start = false;

	for (std::string::const_iterator it = in.begin(); it != in.end(); ++it)
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
	size_t cursor_start = in.find('{', pos) + 1, c_open = 0, i = -1;

	for (std::string::const_iterator it = in.begin(); it != in.end(); ++it, i++)
	{
		if (c_open == 0)
		{
			c_open = 1;
			it += cursor_start;
			i += cursor_start;
		}
		else if (*it == '{')
			c_open++;
		else if (*it == '}')
		{
			if (c_open == 1)
				return (in.substr(cursor_start, i - cursor_start + 1));
			else
				c_open--;
		}
	}
	std::string res;
	return (res);
}

size_t config_string::get_server_name(const size_t pos) const
{
	if (get_next_word(pos) == "server")
	{
		size_t cursor = pos + 6;
		const char *data = in.c_str();
		while (isspace(data[cursor]) && data[cursor] != 0)
			cursor++;
		if (data[cursor] == '{')
			return (in.npos);
		return (cursor);
	}
	else
		return (in.npos);
}

std::string config_string::get_next_line()
{
	size_t find_return = in.find('\n', c);
	std::string res;

	if (c == (size_t)-1)
		return res;

	if (find_return == in.npos)
	{
		c = -1;
		if (c >= in.length())
		{
			return (res);
		}
		else
		{
			res = in.substr(c);
			return res;
		}
	}
	else
		res = in.substr(c, find_return - c + 1);
	c = find_return + 1;
	return (res);
}

std::string config_string::get_next_conf()
{
    std::string key;
    std::string result;
    std::string::const_iterator it = in.begin() + c;
    std::string::const_iterator end = in.end();
    size_t count = c;

    while (it != end && isspace(*it))
    {
        ++it;
        ++count;
    }
    if (it == end)
        return result;

    while (it != end && !isspace(*it) && *it != '{')
    {
        key.push_back(*it);
        ++it;
        ++count;
    }

	if(key == "host") {
		while(it != end && *it != '\n') {
			key.push_back(*it);
			++it;
			++count;
		}
		c = count;
		return key;
	}

    while (it != end && isspace(*it))
    {
        ++it;
        ++count;
    }

    std::string path;
    while (it != end && *it != '{' && !isspace(*it))
    {
        path.push_back(*it);
        ++it;
        ++count;
    }

    // Si un path existe on speaare key et path
    if (!path.empty())
        key += " " + path;

    // Passe les espaces après le path
    while (it != end && isspace(*it))
    {
        ++it;
        ++count;
    }

    // Si { est pas la on lit la ligne d'apres
    if (it == end || *it != '{')
    {
        std::string next_line = get_next_line();
        if (!next_line.empty() && next_line.find('{') != std::string::npos)
        {
            result = key + " {";
        }
        else
        {
            return key; // Pas d'accolade on renvoie la ligne
        }
    }
    else 
    {
        result = key + " {";
        ++it;
        ++count;
    }
    while (it != end && *it != '}')
    {
        result.push_back(*it);
        ++it;
        ++count;
    }
    if (it != end && *it == '}')
    {
        result.push_back('}');
        ++it;
        ++count;
    }
    c = count;
    return result;
}



std::string &config_string::get_str()
{
	return (in);
}

config_string::config_string() : c(0)
{
}

config_string::config_string(const std::string &s) : c(0)
{
	in.assign(s);
}

config_string::~config_string()
{
}


