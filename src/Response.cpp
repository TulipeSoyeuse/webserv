#include "Response.hpp"

Response::Response(const Request &r, Server &s) : _request(r), Status_line("HTTP/1.1 "),
												  _is_binary(false), payload(NULL)
{
	// TODO: handle HTTPS
	// TODO: rework error code and dedicated function
	// TODO: handle CGI env
	// --------------- HTTP version check ------------------------------
	std::pair<std::string, std::string> proto = *_request.get_request().find("Protocol");
	// * check the http version
	if (proto.second != "HTTP/1.1")
	{
		status_code = 505;
		(Status_line += SSTR(status_code << " ")) += "HTTP Version not supported\r\n";
		_response = Status_line + "\r\n";
	}
	// -----------------------------------------------------------------
	else
	{
		// * get info about request ?
		set_server_conf(s);
		// * if request is GET
		if (_request.get_type() == GET)
		{
			// * build header
			build_header();
			_response.assign(Status_line.c_str());
			cMap_str(general_header, _response);
			cMap_str(response_header, _response);
			cMap_str(entity_header, _response);
			_response += "\r\n";
			if (status_code == 200)
			{
				if (_is_binary)
					_response += "\r\n";
				_response.append(payload, content_length);
			}
		}
		// TODO: POST
	}
}

void Response::MIME_attribute()
{
	std::string uri = ((Map)_request.get_request())["URI"];
	if (uri == "/")
	{
		entity_header["Content-Type"] = HTML "; charset=UTF-8\n";
		return;
	}
	std::string file_format = uri.substr(uri.find_last_of('.') + 1);

	std::cout << "file format: " << file_format << std::endl;

	if (file_format == "aac")
		entity_header["Content-Type"] = AAC "; charset=UTF-8\n";
	// ! Rajout de cette ligne pour tester le script bash et php
	if (file_format == "sh")
		entity_header["Content-Type"] = "text/html ; charset=UTF-8\n";
	else if (file_format == "svg")
	{
		entity_header["Content-Type"] = SVG;
		_is_binary = true;
	}
	else if (file_format == "png")
	{
		entity_header["Content-Type"] = PNG;
		_is_binary = true;
	}
	else if (file_format == "jpg" || file_format == "jpeg")
	{
		entity_header["Content-Type"] = JPEG;
		_is_binary = true;
	}
	else if (file_format == "html")
		entity_header["Content-Type"] = HTML "; charset=UTF-8\n";
	else if (file_format == "txt")
		entity_header["Content-Type"] = TXT "; charset=UTF-8\n";
	else if (file_format == "js")
		entity_header["Content-Type"] = JS "; charset=UTF-8\n";
	else if (file_format == "css")
		entity_header["Content-Type"] = CSS "; charset=UTF-8\n";
	else
		entity_header["Content-Type"] = TXT "; charset=UTF-8\n";
}

void Response::build_header()
{

	general_header["Server"] = "webserv/0.1\n";
	// * if i find the file
	if (match_file())
		(Status_line += SSTR(status_code)) += " Sucess\r\n";
	else
	{
		(Status_line += SSTR(status_code)) += " Not Found\r\n";
		return;
	}
	// * if everything is ok
	if (status_code == 200)
	{
		// *
		MIME_attribute();
		set_payload();
		entity_header["Content-Length"] = SSTR(content_length);
	}
}

bool Response::match_file()
{
	// TODO: implement location
	const std::string root_dir = serv.find("route")->second + serv.find("location")->second;
	std::string uri = _request.get_request().find("URI")->second;
	std::cout << "root dir : " << root_dir << std::endl;
	DIR *dir = opendir((root_dir + uri.substr(0, uri.find_last_of('/'))).c_str());
	struct dirent *diread;

	if (uri == "/")
	{
		while ((diread = readdir(dir)) != NULL) // TODO: when location conf path don't exist
		{
			if (std::strncmp(diread->d_name, "index", 5) == 0)
			{
				struct stat _stat;
				stat(diread->d_name, &_stat);
				file_path = root_dir + '/' + diread->d_name;
				std::cout << file_path << "\n";
				status_code = 200;
				closedir(dir);
				return true;
			}
		}
		status_code = 404;
		closedir(dir);
		return false;
	}

	const std::string file = uri.substr(uri.find_last_of('/') + 1);
	std::cout << "file asked: " << "\"" << file << "\"" << "\n";
	std::cout << "full path: " << "\"" << root_dir << uri.c_str() << "\"" << "\n";

	while ((diread = readdir(dir)) != NULL)
	{
		if (std::strcmp(diread->d_name, file.c_str()) == 0)
		{
			struct stat _stat;
			stat(diread->d_name, &_stat);
			file_path = root_dir + uri;
			std::cout << file_path << "\n";
			status_code = 200;
			closedir(dir);
			return true;
		}
	}
	status_code = 404;
	closedir(dir);
	return false;
}

void Response::set_server_conf(Server &s)
{
	// * get host
	std::string host = _request.get_request().find("Host")->second;
	// * get port
	serv = s.get_config(host, _request.get_in_port());
	// * get client_size
	if (serv.find("client_size") != serv.end() && serv.find("client_size")->second != "0")
		client_size = std::atoi(serv.find("client_size")->second.c_str());
	else
		client_size = 4096;
}

bool Response::set_payload()
{
	// * call the cgi
	if (file_path.substr(file_path.find_last_of('.')) == PHP_ext)
		return CGI_from_file(PHP);
	else if (file_path.substr(file_path.find_last_of('.')) == SH_ext)
		return CGI_from_file(BASH);
	else if (file_path.substr(file_path.find_last_of('.')) == PY_ext)
		return CGI_from_file(PYTHON);
	// * return de body
	else
		return read_payload_from_file();
}

bool Response::CGI_from_file(CGI c)
{
	std::cout << "---------CGI from file--------\n";
	std::cout << "reading cgi: " << c << "\n";

	payload = new char[client_size];
	hm_popen hmpop(file_path, c);
	if (!hmpop.is_good())
		std::cout << "C CASSE\n";
	// TODO: after error response rework -> handle good flag from popen
	content_length = hmpop.read_out(payload, client_size);
	std::cout << "content length: " << content_length << "\n";
	std::cout << "---------PAYLOAD--------\n"
			  << payload;
	return (true);
}

bool Response::read_payload_from_file()
{
	std::ifstream f;
	// --------------- extension check ------------------------------
	if (_is_binary)
		f.open(file_path.c_str(), std::ifstream::binary);
	else
		f.open(file_path.c_str());
	// --------------------------------------------------------------
	std::cout << "is binary : " << _is_binary << "\n";
	if (f.good())
	{
		f.ignore(std::numeric_limits<std::streamsize>::max());
		content_length = f.gcount();
		f.clear();
		f.seekg(0, std::ios_base::beg);

		payload = new char[content_length + 1];

		f.read(payload, content_length);
		payload[content_length] = 0;
		f.close();
		return (true);
	}
	else
	{
		std::cerr << "Reading error\n";
		return (false);
	}
}

void Response::cMap_str(Map &m, std::string &s)
{
	for (Map::const_iterator it = m.begin(); it != m.end(); ++it)
	{
		s += it->first + ": ";
		s += it->second;
		if (++it != m.end())
			s += '\n';
		it--;
	}
}

const int &Response::get_status()
{
	return (status_code);
}

Response::~Response()
{
	if (payload)
		delete[] payload;
}

const std::string &Response::get_response() const
{
	return (_response);
}

std::ostream &operator<<(std::ostream &out, const Response &c)
{
	out << c.get_response().c_str();
	return (out);
}
