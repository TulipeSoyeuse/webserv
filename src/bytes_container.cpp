#include "bytes_container.hpp"

bytes_container::bytes_container() : _cursor(0)
{
}

bytes_container::~bytes_container()
{
}

void bytes_container::fill(const char *s, size_t i)
{
	_data.insert(_data.end(), s, s + i);
}

void bytes_container::fill(ct::iterator b, ct::iterator e)
{
	_data.assign(b, e);
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
	std::cout << "line :" << s << '\n';
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
	res.fill(_data.begin() + t, _data.end());
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
	out.write(c.get_data(), c.get_data_size());
	return (out);
}
