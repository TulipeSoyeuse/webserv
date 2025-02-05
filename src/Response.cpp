#include "Response.hpp"

Response::Response(const Request &r, Server &s) : _request(r), Status_line("HTTP/1.1 "),
												  status_code(200), _is_binary(false),
												  payload(NULL), autoindex(false), foundIndex(false)
{
	if (!_request._status)
	{
		http_error(400);
		return;
	}

	// TODO: rework error code and dedicated function
	// TODO: POST handler for upload file
	// --------------- HTTP version check ------------------------------
	std::pair<std::string, std::string> proto = *_request.get_request().find("Protocol");
	// * check the http version
	if (_request.get_type() != PUT)
		Status_line = "HTTP/1.1 200 Sucess\r\n";
	else
		Status_line = "HTTP/1.1 201 Created\r\n";

	if (proto.second != "HTTP/1.1")
	{
		http_error(505);
		return;
	}
	else
	{
		// * link the right server for the current request (link on "serv" var)
		set_server_conf(s);
		std::map<std::string, std::string>::iterator it = serv.find("autoindex");
		if (it != serv.end() && it->second == "on")
			autoindex = true;
		// * build header
		build_header();
		if (set_payload())
			entity_header["Content-Length"] = SSTR(content_length);
		_response.assign(Status_line.c_str());
		cMap_str(general_header, _response);
		cMap_str(response_header, _response);
		cMap_str(entity_header, _response);
		_response += "\r\n";
		if (payload)
			_response.append(payload, content_length);
	}
}
// TODO: add in server multiple error page for error code
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
	if (file_format == "php")
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

void Response::build_header()
{
	general_header["Server"] = "webserv/0.1\n";
	// * if i find the file
	MIME_attribute();
}

bool Response::match_file()
{
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
	std::string uri(_request.get_request().find("URI")->second);
	root_dir = serv.find("route")->second + serv.find("location")->second;

	if (uri == "/")
	{
		DIR *dir = opendir((root_dir + uri.substr(0, uri.find_last_of('/'))).c_str());
		struct dirent *diread;
		while ((diread = readdir(dir)) != NULL)
			if (std::strncmp(diread->d_name, "index", 5) == 0)
			{
				file_path = root_dir + "/" + diread->d_name;
				foundIndex = true;
			}
	}
	else
	{
		file_path = root_dir + uri;
		foundIndex = true;
	}
	std::cout << "index is " << foundIndex << "auto is " << autoindex << std::endl;
	if (autoindex && !foundIndex)
		return true;
	std::cout << "file requested: \"" << file_path << "\"\n";
	return (does_file_exist(file_path));
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
	if (!check_file())
	{
		if (_request.get_type() == PUT)
		{
			content_length = 0;
			if (write_file())
				return (true);
			else
			{
				// * si route inexistant 404
				// * si droit insuffisant 403
				return (false);
			}
		}
		else
		{
			std::cout << "koukou\n";
			http_error(403);
		}
		http_error(404);
		return (false);
	}

	else if (_request.get_type() == PUT)
	{
		std::cout << "kaka\n";
		http_error(403);
		return (false);
	}
	if (autoindex && !foundIndex)
		return (generate_autoindex());
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

	std::string dir = dir_listing(root_dir);
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
	if (!hmpop.is_good())
	{
		std::cerr << "----child error----\n"
				  << payload << "\n--------------\n";
		delete payload;
		content_length = 0;
		http_error(500);
		return (false);
	}
	// TODO: after error response rework -> handle good flag from popen
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

bool can_write(std::string file_path)
{
	struct stat file;

	if (!stat(file_path.c_str(), &file))
	{
		std::cerr << "Error: cannot access file\n";
		return false;
	}

	if (!(file.st_mode & S_IWUSR))
	{
		std::cerr << "Error: No write permission\n";
		return false;
	}

	return true;
}

bool Response::write_file()
{
	std::string dir;
	size_t pos = file_path.rfind('/');
	if (pos != std::string::npos)
	{
		dir = file_path.substr(0, pos);
		std::cout << "kaka" << dir << std::endl;
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

	std::ofstream f;
	std::ios_base::openmode m = std::ios::trunc;
	if (_is_binary) // TODO: check ?
		m = m | std::ios::binary;

	std::cout << "this is file_path= " << std::endl;
	std::cout << "CREATING: " << file_path << "\n";
	f.open(file_path.c_str(), m);
	if (f.good())
	{
		// ! Segfault ad il y a pas de content lenght
		std::cout << _request.get_request().find("Content-Length")->second.c_str() << std::endl;
		f.write(_request.get_request().find("Payload")->second.c_str(),
				std::atoi(_request.get_request().find("Content-Length")->second.c_str()));
	}
	else
		return (false);
	return (true);
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

void Response::http_error(int code)
{
	// set env
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
	// read error file if provided in server conf
	_is_binary = false;
	Map::iterator error_page = serv.find(SSTR(code));
	if (error_page != serv.end())
	{
		// dynamic error page
		file_path = serv.find("route")->second + serv.find("location")->second + "/" + error_page->second;
		// eate_error_page(code);
		read_payload_from_file();
		entity_header["Content-Length"] = SSTR(content_length);
	}
}

// void Response::create_error_page(int code) {
// 	//verifier le path et qu'il existe
// 	std::ifstream err_out;

// 	std::string replace;

// 	//avancer jusque body
// 	// Changer l'erreur
// }

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
