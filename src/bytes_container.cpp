#include "bytes_container.hpp"

bytes_container::bytes_container() : _cursor(0)
{
}

bytes_container::~bytes_container()
{
}

// @brief insert at the end
void bytes_container::fill(const char *s, size_t i)
{
	_data.insert(_data.end(), s, s + i);
}

// @brief replace content
void bytes_container::assign(ct::iterator b, ct::iterator e)
{
	_data.assign(b, e);
}

void bytes_container::fill(bytes_container &b, std::streamsize e)
{
	fill(b.get_data(), e);
}

// @brief push back
void bytes_container::fill(char c)
{
	_data.push_back(c);
}

void bytes_container::clear()
{
	_data.clear();
}

void bytes_container::seek(unsigned int i)
{
	if (i < _data.size())
		_cursor = i;
}

size_t bytes_container::safeGetline(std::string &s)
{
	s.clear();
	size_t i = 0;
	for (ct::iterator it = _data.begin() + _cursor; it != _data.end(); ++it)
	{
		if (*it == '\n')
		{
			_cursor++;
			break;
		}
		else
			s.push_back(*it);
		_cursor++, i++;
	}
	s.erase(std::remove(s.begin(), s.end(), '\r'), s.end());
	return (i);
}

size_t bytes_container::read(std::string &res, size_t len)
{
	res.clear();
	if (_cursor >= _data.size())
		return (0);
	for (ct::iterator it = _data.begin() + _cursor; it != _data.end() && len--; ++it)
	{
		res.push_back(*it);
	}
	_cursor += res.length();
	return (res.length());
}

size_t bytes_container::read(bytes_container &res, size_t len)
{
	res.clear();
	if (_cursor >= _data.size())
		return (0);

	ct::iterator it = _data.begin() + _cursor;
	size_t i = 0;

	for (; it != _data.end() && len--; ++it)
		i++;

	res.assign(_data.begin() + _cursor, it);
	_cursor += i;
	return (i);
}

int bytes_container::find_last_of(const char *p)
{
	size_t l = strlen(p);
	size_t r = _data.size() - 1 - l;
	ct::iterator it = _data.end() - l;
	for (; it != _data.begin(); --it)
	{
		if (strncmp((char *)&it, p, l))
			r--;
		else
			return (r);
	}
	return -1;
}

int bytes_container::find_last_of(const char c)
{
	size_t r = _data.size();
	for (ct::iterator it = _data.end(); it != _data.begin(); --it)
	{
		if (*it == c)
			return r;
		else
			r--;
	}
	return (-1);
}

bytes_container bytes_container::subcontainer(size_t t)
{
	bytes_container res;
	res.assign(_data.begin() + t, _data.end());
	return res;
}

bytes_container bytes_container::subcontainer()
{
	bytes_container res;
	res.assign(_data.begin() + _cursor, _data.end());
	_cursor = _data.size();
	return res;
}

const char *bytes_container::get_data() const
{
	return (_data.data());
}

int bytes_container::get_data_size() const
{
	return (_data.size());
}

std::ostream &operator<<(std::ostream &out, const bytes_container &c)
{
	out.write(c.get_data(), (c.get_data_size() > 1000) ? 300 : c.get_data_size());
	return (out);
}
