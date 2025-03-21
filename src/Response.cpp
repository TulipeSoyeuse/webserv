#include "Response.hpp"

Response::Response(const Request &r, Server &s) : _request(r), Status_line("HTTP/1.1 "), status_code(200),
												  _is_binary(false), _error(false), _chunk(false),
												  _isdir(false), content_length(0),
												  last(false), config(s), autoindex(false)
{
	// foundIndex(false)
	if (!_request._status)
	{
		http_error(400);
		std::cout << "ici\n";
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
	_header["Server"] = "webserv/0.1";
	// * if i find the file
	MIME_attribute();

	if (set_payload())
		_header["Content-Length"] = SSTR(content_length);

	if (!is_body_size_valid())
		// is size is greater than client_size, then process by chunk
		serv_by_chunk();

	// assembly ----------------------------
	_response.fill(Status_line.c_str(), Status_line.size());
	cMap_str(_header, _response);
	_response.fill("\r\n", 2);
	if (payload.get_data_size() != 0 && !_chunk)
	{
		_response.fill(payload, content_length);
	}
	// else if (_chunk)
	// 	_response.fill("\r\n", 2);
}

// @brief update header and body to serv the new purpose and update the _reste variable
void Response::serv_by_chunk()
{
	// header
	_header["Transfer-Encoding"] = "chunked";
	_header.erase("Content-Length");
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
	it = loc.second.find("client_size");

	if (it == loc.second.end())
	{
		std::cout << "body size valid (no param)\n";
		return true;
	}
	else if (atoi(it->second.c_str()) == 0 || content_length <= atoi(it->second.c_str()))
	{
		std::cout << "body size: " << content_length << " valid (limit : undefined )\n";
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
		_header["Content-Type"] = HTML "; charset=UTF-8";
		return;
	}
	std::string file_format = uri.substr(uri.find_last_of('.') + 1);

	std::cout << "file format: " << file_format << std::endl;

	if (file_format == "aac")
		_header["Content-Type"] = AAC "; charset=UTF-8";
	// ! Rajout de cette ligne pour tester le script bash et php
	if (file_format == "php")
		_header["Content-Type"] = "text/html ; charset=UTF-8";

	else if (file_format == "py")
		_header["Content-Type"] = "text/html ; charset=UTF-8";

	else if (file_format == "sh")
		_header["Content-Type"] = "text/plain ; charset=UTF-8";
	else if (file_format == "svg")
	{
		_header["Content-Type"] = SVG;
		_is_binary = true;
	}
	else if (file_format == "png")
	{
		_header["Content-Type"] = PNG;
		_is_binary = true;
	}
	else if (file_format == "jpg" || file_format == "jpeg")
	{
		_header["Content-Type"] = JPEG;
		_is_binary = true;
	}
	else if (file_format == "html")
		_header["Content-Type"] = HTML "; charset=UTF-8";
	else if (file_format == "txt")
		_header["Content-Type"] = TXT "; charset=UTF-8";
	else if (file_format == "js")
		_header["Content-Type"] = JS "; charset=UTF-8";
	else if (file_format == "css")
		_header["Content-Type"] = CSS "; charset=UTF-8";
	else
		_header["Content-Type"] = TXT "; charset=UTF-8";
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
	file_path.clear();

	bool is_index = false;
	p_location l = config.get_location_subconf(serv, _request.get_headers().find("URI")->second);
	if (l.second.find("index") != l.second.end())
		is_index = true;

	DIR *dir = opendir((root_dir + uri).c_str());
	if (!dir)
		file_path = root_dir + uri;
	else
	{
		_isdir = true;
		struct dirent *diread;
		while ((diread = readdir(dir)) != NULL)
			if ((is_index && l.second.find("index")->second == diread->d_name) ||
				(!is_index && std::string(diread->d_name).find("index.") != std::string::npos))
			{
				if (*--uri.end() != '/')
					file_path = root_dir + uri + '/' + diread->d_name;
				else
					file_path = root_dir + uri + diread->d_name;
				std::cout << "file path " << file_path << "\n";
			}
		closedir(dir);
	}
	if (autoindex && _isdir && file_path == "")
		return false;
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
	if (!check_file())
	{
		if (!check_proto())
		{
			http_error(405);
			return (false);
		}
		if (_request.get_type() == PUT)
		{
			content_length = 0;
			if (write_file())
				return (true);
		}
		else
		{
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
	if (!check_proto())
	{
		http_error(405);
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
	_header["Content-Type"] = "text/html ; charset=UTF-8";
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
	payload.clear();
	payload.fill(html_content.c_str(), content_length);
	std::cout << "Payload:\n"
			  << payload << std::endl;
	return true;
}

bool Response::CGI_from_file(CGI c)
{
	std::cout << "---------CGI from file--------\n";
	hm_popen hmpop(file_path, c, _request);
	content_length = hmpop.read_out(payload);
	if (hmpop.is_good() != 200)
	{
		payload.clear();
		content_length = 0;
		http_error(hmpop.is_good());
		return (false);
	}
	std::cout << "content length: "
			  << content_length << "\n";
	std::cout << "---------PAYLOAD--------\n"
			  << payload;
	return (true);
}

bool Response::read_payload_from_file()
{
	std::ifstream f;
	// --------------- extension check ------------------------------
	if (_is_binary)
		f.open(file_path.c_str(), std::ifstream::binary | std::ifstream::ate);
	else
		f.open(file_path.c_str(), std::ifstream::ate);
	// --------------------------------------------------------------
	if (f.good())
	{
		content_length = f.tellg();
		f.clear();
		f.seekg(0, std::ios_base::beg);
		char *buffer = new char[content_length];
		f.read(buffer, content_length);
		f.close();

		payload.fill(buffer, content_length);
		delete[] buffer;
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
		s.fill("\r\n", 2);
	}
}

void Response::http_error(int code)
{
	// set env
	if (_error)
		return;
	_error = true;
	status_code = code;
	_header["Content-Type"] = HTML "; charset=UTF-8";
	// code
	if (code == 505)
		Status_line = ("HTTP/1.1 " + SSTR(code << " ") + "HTTP Version not supported\r\n");
	else if (code == 204)
		Status_line = ("HTTP/1.1 " + SSTR(code << " ") + "No content\r\n");
	else if (code == 400)
		Status_line = ("HTTP/1.1 " + SSTR(code << " ") + "Bad Request\r\n");
	else if (code == 403)
		Status_line = ("HTTP/1.1 " + SSTR(code << " ") + "Forbidden\r\n");
	else if (code == 404)
		Status_line = ("HTTP/1.1 " + SSTR(code << " ") + "Not Found\r\n");
	else if (code == 405)
		Status_line = ("HTTP/1.1 " + SSTR(code << " ") + "Method Not Allowed\r\n");
	else if (code == 444)
		Status_line = ("HTTP/1.1 " + SSTR(code << " ") + "No Response\r\n");
	else if (code == 413)
		Status_line = ("HTTP/1.1 " + SSTR(code << " ") + "Request Entity Too Large\r\n");
	else if (code == 500)
		Status_line = ("HTTP/1.1 " + SSTR(code << " ") + "Internal Server Error\r\n");
	// read error file if provided in server conf
	_is_binary = false;
	server_m::iterator error_page = serv.find("error_page");
	server_m def;
	if (error_page != serv.end() &&
		error_page->second.second.find(SSTR(code)) != error_page->second.second.end())
	{
		// dynamic error page
		file_path = serv.find("route")->second.first + serv.find("location")->second.first + "/" + error_page->second.second.find(SSTR(code))->second;
		std::cout << "file_path:" << file_path << '\n';
		read_payload_from_file();
		_header["Content-Length"] = SSTR(content_length);
	}
	else if (def = config.get_default_config(), def.find("error_page") != def.end())
	{
		file_path = def.find("error_page")->second.second.find(SSTR(code))->second;
		std::cout << "file_path:" << file_path << '\n';
		read_payload_from_file();
		_header["Content-Length"] = SSTR(content_length);
	}
	else
	{
		_header["Content-Length"] = SSTR(0);
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

	{
		std::string s(SSTRH(l));
		res.fill(s.c_str(), s.length());
	}
	res.fill("\r\n", 2);
	res.fill(buf.get_data(), l);
	res.fill("\r\n", 2);

	if (!last && l == 0)
	{
		res.clear();
		last = true;
		res.fill("0\r\n\r\n", 5);
		return (0);
	}
	return (1);
}

const bool &Response::is_binary() const
{
	return (_is_binary);
}

const int &Response::get_status()
{
	return (status_code);
}

Response::~Response()
{
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
