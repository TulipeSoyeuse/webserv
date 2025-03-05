#include "Response.hpp"

Response::Response(const Request &r, Server &s) : _request(r), Status_line("HTTP/1.1 "), status_code(200),
												  _is_binary(false), _error(false), _chunk(false),
												  _isdir(false), content_length(0),
												  payload(NULL), last(false), config(s), autoindex(false)
{
	// foundIndex(false)
	if (!_request._status)
	{
		http_error(400);
		return;
	}

	// TODO: rework error code and dedicated function
	// TODO: POST handler for upload file
	if (_request.get_type() != PUT)
		Status_line = "HTTP/1.1 200 OK\r\n";
	else
		Status_line = "HTTP/1.1 201 Created\r\n";

	// * link the right server for the current request (link on "serv" var)
	set_server_conf(s);
	if (!strcmp(serv.find("host")->second.first.c_str(), "NOTFOUND"))
		http_error(400);
	check_autoindex();

	// * build header
	general_header["Server"] = "webserv/0.1\n";
	// * if i find the file
	MIME_attribute();

	if (set_payload())
	{
		entity_header["Content-Length"] = SSTR(content_length);
		if (!is_body_size_valid())
		{
			// is size is greater than client_size, then process by chunk
			serv_by_chunk();
		}
	}

	// assembly ----------------------------
	_response.fill(Status_line.c_str(), Status_line.size());
	cMap_str(general_header, _response);
	cMap_str(response_header, _response);
	cMap_str(entity_header, _response);
	_response.fill("\r\n", 2);
	if (payload && !_chunk)
	{
		_response.fill(payload, content_length);
	}
}

// @brief update header and body to serv the new purpose and update the _reste variable
void Response::serv_by_chunk()
{
	// header
	entity_header["Transfer-Encoding"] = "chunked";
	entity_header.erase("Content-Length");
	_chunk = true;
	_reste.fill(payload, content_length);
	// size
	chunk_size = CHUNK_MINI;
	while (content_length / chunk_size > 20 && chunk_size < 1000000)
		chunk_size += CHUNK_MINI;
}

bool Response::is_body_size_valid() const
{
	if (_error)
		return true;
	p_location loc = config.get_location_subconf(serv, _request.get_headers().find("URI")->second);
	Map::iterator it;
	if ((it = loc.second.find("client_size")) == loc.second.end() ||
		(atoi(it->second.c_str()) == 0 || content_length <= atoi(it->second.c_str())))
	{
		std::cout << "body size: " << content_length << " valid (limit :" << it->second << ")\n";
		return true;
	}
	else
	{
		std::cout << "body size: " << content_length << " invalid (limit :" << it->second << ")\n";
		return false;
	}
}

bool Response::check_proto()
{
	p_location l = config.get_location_subconf(serv, _request.get_headers().find("URI")->second);

	Map::iterator it = l.second.find("proto");
	if (it != l.second.end())
	{
		if (_request.get_type() == GET)
		{
			if (it->second.find("GET") != std::string::npos)
				return true;
		}
		else if (_request.get_type() == POST)
		{
			if (it->second.find("POST") != std::string::npos)
				return true;
		}
		else if (_request.get_type() == PUT)
		{
			if (it->second.find("PUT") != std::string::npos)
				return true;
		}
		else if (_request.get_type() == DELETE)
		{
			if (it->second.find("DELETE") != std::string::npos)
				return true;
		}
	}
	return false;
}

void Response::check_autoindex()
{
	if (_request.get_headers().find("URI") == _request.get_headers().end())
		return;
	p_location loc = config.get_location_subconf(serv, _request.get_headers().find("URI")->second);
	Map::iterator it;
	if ((it = loc.second.find("autoindex")) == loc.second.end())
		return;
	else if (it->second == "on")
		autoindex = true;
}

void Response::MIME_attribute()
{
	std::string uri = ((Map)_request.get_headers())["URI"];
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
	if (file_format == "php")
		entity_header["Content-Type"] = "text/html ; charset=UTF-8\n";

	else if (file_format == "py")
		entity_header["Content-Type"] = "text/html ; charset=UTF-8\n";

	else if (file_format == "sh")
		entity_header["Content-Type"] = "text/plain ; charset=UTF-8\n";
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

bool Response::match_file()
{
	const std::string root_dir = serv.find("route")->second.first + serv.find("location")->second.first;
	std::string uri = _request.get_headers().find("URI")->second;
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
				closedir(dir);
				return true;
			}
		}
		closedir(dir);
		http_error(404);
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
			closedir(dir);
			return true;
		}
	}
	closedir(dir);
	http_error(404);
	return false;
}
// setup file path and check his accessibility
bool Response::check_file()
{
	std::string uri(_request.get_headers().find("URI")->second);
	root_dir = serv.find("route")->second.first + serv.find("location")->second.first;

	if (*--uri.end() == '/')
	{
		DIR *dir = opendir((root_dir + uri).c_str());
		if (dir)
			_isdir = true;
		struct dirent *diread;
		while ((diread = readdir(dir)) != NULL)
			if (std::strncmp(diread->d_name, "index", 5) == 0)
				file_path = root_dir + uri + diread->d_name;
		closedir(dir);
	}
	else
	{
		file_path = root_dir + uri;
	}
	if (autoindex && _isdir && file_path == "")
		return false;

	std::cout << "file requested: \"" << file_path << "\"\n";
	return (does_file_exist(file_path));
}

void Response::set_server_conf(Server &s)
{
	// * get host
	std::string host = _request.get_headers().find("Host")->second;
	// * get port
	serv = s.get_config(host, _request.get_in_port());

	if (serv.find("client_size") != serv.end() && serv.find("client_size")->second.first != "0")
		client_size = std::atoi(serv.find("client_size")->second.first.c_str());
	else
		client_size = 65536;
}

// TODO : .conf authorise to delete ?
//  TODO : delete only file or directory ?
//  * can send 200, 202, 204
bool Response::delete_file()
{

	struct stat path_stat;

	if (stat(file_path.c_str(), &path_stat) == -1)
	{
		std::cerr << "problem with path" << std::endl;
	}

	if (access(file_path.c_str(), R_OK | W_OK) == -1)
	{
		if (errno == EACCES)
		{
			// * droits inssufisants
			std::cerr << "Error :droits insuffisants" << std::endl;
			http_error(403);
			return false;
		}
		// * n'existe pas
		if (errno == ENOENT)
		{
			std::cerr << "Error: file not found" << std::endl;
			http_error(404);
			return false;
		}
	}

	// * si c un file
	if (S_ISREG(path_stat.st_mode))
	{
		if (!std::remove(file_path.c_str()))
			return false;
	}

	// * si c un dossier
	else if (S_ISDIR(path_stat.st_mode))
	{
		std::cout << "je suis un dir" << std::endl;
		http_error(403);
		return false;
	}

	return true;
}

bool is_dir(std::string file_path)
{
	struct stat path_stat;

	if (stat(file_path.c_str(), &path_stat) == -1)
	{
		std::cerr << "problem with path" << std::endl;
	}

	// * si c un dossier
	if (S_ISDIR(path_stat.st_mode))
		return true;
	return false;
}

bool Response::set_payload()
{
	if (_error)
		return false;
	std::pair<std::string, std::string> proto = *_request.get_headers().find("Protocol");
	if (proto.second != "HTTP/1.1")
	{
		http_error(505);
		return (false);
	}
	if (!check_proto())
	{
		http_error(405);
		return (false);
	}
	if (!check_file())
	{
		if (_request.get_type() == PUT)
		{
			content_length = 0;
			if (write_file())
			{
				return (true);
			}
			else
				return (false);
		}
		else
		{
			// TODO: change error from 403 to 404, dit i miss something ? autoindex ?
			if (_isdir)
			{
				if (autoindex)
					return (generate_autoindex());
				else
					http_error(403);
			}
			else
				http_error(404);
		}
		return (false);
	}
	// ? on part du principe que ca a passe check_file et que le fichier existe
	if (_request.get_type() == DELETE)
	{
		if (!delete_file())
			return false;
	}
	else if (_request.get_type() == PUT)
	{
		http_error(403);
		return (false);
	}
	// * Pardon j'avais pas vu cette fonction avant de faire mon refacto, j'ai integré la logique au dessus
	// * la si c un dossier ca va plus
	// if (is_dir(file_path))
	// {
	// 	http_error(403);
	// 	return false;
	// }

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

std::string dir_listing(std::string root_dir)
{
	std::cout << "dir:" << root_dir << "\n";
	std::string html;
	DIR *dir = opendir(root_dir.c_str());
	struct dirent *entry;

	if (!dir)
	{
		html += "<p>Error opening directory</p>\n</body>\n</html>";
		return html;
	}

	html += "<ul>\n";
	while ((entry = readdir(dir)) != NULL)
	{
		if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
		{
			html += "<li><a href=\"" + std::string(entry->d_name) + "\">" + entry->d_name + "</a></li>\n";
		}
	}
	html += "</ul>\n";
	closedir(dir);
	return html;
}

bool Response::generate_autoindex()
{

	entity_header["Content-Type"] = "text/html ; charset=UTF-8\n";
	std::string dir = dir_listing(root_dir + _request.get_headers().find("URI")->second);
	std::string html_content =
		"<!DOCTYPE html>"
		"<html lang=\"en\">"
		"<head>"
		"<meta charset=\"UTF-8\">"
		"<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"
		"<title>Simple Page</title>"
		"</head>"
		"<body>"
		"<h1>Autoindex</h1>";
	html_content += dir;
	html_content += "</body>"
					"</html>";

	content_length = html_content.length();
	if (payload)
		delete payload;
	payload = new char[html_content.length() + 1];

	std::strcpy(payload, html_content.c_str());
	std::cout << "Payload:\n"
			  << payload << std::endl;
	return true;
}

bool Response::CGI_from_file(CGI c)
{
	std::cout << "---------CGI from file--------\n";
	std::cout << "reading cgi: " << c << "\n";

	payload = new char[client_size];
	hm_popen hmpop(file_path, c, _request);
	content_length = hmpop.read_out(payload, client_size);
	if (hmpop.is_good() != 200)
	{
		std::cerr << "----child error----\n"
				  << payload << "\n--------------\n";
		delete payload;
		content_length = 0;
		if (hmpop.is_good() == 408)
			http_error(408);
		else
			http_error(500);
		return (false);
	}
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
		// TODO : check client size, si c trop petit je me casse
		payload = new char[content_length + 1];
		bzero(payload, content_length + 1);
		f.read(payload, content_length);
		f.close();
		return (true);
	}
	else
	{
		std::cerr << "Reading error\n";
		return (false);
	}
}

bool Response::error_path()
{

	std::string dir;
	size_t pos = file_path.rfind('/');
	if (pos != std::string::npos)
	{
		dir = file_path.substr(0, pos);
	}
	else
		std::cout << dir << std::endl;

	if (access(dir.c_str(), R_OK | X_OK))
	{
		if (errno == ENOENT)
		{
			std::cerr << "Error: not found\n";
			http_error(404);
			return false;
		}
		else if (errno == EACCES)
		{
			std::cerr << "Error: no permission to access\n";
			http_error(403);
			return false;
		}
	}
	return true;
}

bool Response::write_file()
{

	if (!error_path())
		return false;

	std::ofstream f;
	std::ios_base::openmode m = std::ios::trunc;
	if (_is_binary)
		m = m | std::ios::binary;

	std::cout << "CREATING: " << file_path << "\n";
	f.open(file_path.c_str(), m);
	if (f.good())
	{
		// TODO : segfault with bad request
		std::cout << _request.get_headers().find("Content-Length")->second.c_str() << std::endl;
		f.write(_request.get_body().get_data(),
				std::atoi(_request.get_headers().find("Content-Length")->second.c_str()));
	}
	else
		return (false);
	return (true);
}

void Response::cMap_str(Map &m, bytes_container &s)
{
	for (Map::const_iterator it = m.begin(); it != m.end(); ++it)
	{
		s.fill((it->first + ": ").c_str(), it->first.size() + 2);
		s.fill(it->second.c_str(), it->second.length());
		if (++it != m.end())
			s.fill('\n');
		it--;
	}
}

void Response::http_error(int code)
{
	// set env
	if (_error)
		return;
	_error = true;
	status_code = code;
	entity_header["Content-Type"] = HTML "; charset=UTF-8\n";
	// code
	if (code == 505)
		Status_line = ("HTTP/1.1 " + SSTR(code << " ") + "HTTP Version not supported\r\n");
	else if (code == 403)
		Status_line = ("HTTP/1.1 " + SSTR(code << " ") + "Forbidden\r\n");
	else if (code == 404)
		Status_line = ("HTTP/1.1 " + SSTR(code << " ") + "Not Found\r\n");
	else if (code == 500)
		Status_line = ("HTTP/1.1 " + SSTR(code << " ") + "Internal Server Error\r\n");
	else if (code == 400)
		Status_line = ("HTTP/1.1 " + SSTR(code << " ") + "Bad Request\r\n");
	else if (code == 204)
		Status_line = ("HTTP/1.1 " + SSTR(code << " ") + "No content\r\n");
	else if (code == 413)
		Status_line = ("HTTP/1.1 " + SSTR(code << " ") + "Request Entity Too Large\r\n");
	else if (code == 405)
		Status_line = ("HTTP/1.1 " + SSTR(code << " ") + "Method Not Allowed\r\n");
	// read error file if provided in server conf
	_is_binary = false;
	server_m::iterator error_page = serv.find("error_page");
	if (error_page != serv.end() &&
		serv.find("host")->first != "NOTFOUND" &&
		error_page->second.second.find(SSTR(code)) != error_page->second.second.end())
	{
		// dynamic error page
		file_path = serv.find("route")->second.first + serv.find("location")->second.first + "/" + error_page->second.second.find(SSTR(code))->second;
		std::cout << "file_path:" << file_path << '\n';
		read_payload_from_file();
		entity_header["Content-Length"] = SSTR(content_length);
	}
	else
	{
		file_path = config.get_default_config().find("error_page")->second.second.find(SSTR(code))->second;
		std::cout << "file_path:" << file_path << '\n';
		read_payload_from_file();
		entity_header["Content-Length"] = SSTR(content_length);
	}
}

const bool &Response::is_chunked() const
{
	return (_chunk);
}

int Response::get_next_chunk(bytes_container &res)
{
	res.clear();

	if (last)
		return (-1);

	bytes_container buf;
	size_t l = _reste.read(buf, chunk_size);

	std::string s(SSTRH(l) + "\r\n");
	res.fill(s.c_str(), s.length());
	res.fill(buf.get_data(), l);

	if (!last && l == 0)
	{
		res.clear();
		last = true;
		res.fill("0\r\n", 3);
		return (0);
	}
	return (1);
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

const bytes_container &Response::get_response() const
{
	return (_response);
}

std::ostream &operator<<(std::ostream &out, const Response &c)
{
	out << c.get_response();
	return (out);
}
