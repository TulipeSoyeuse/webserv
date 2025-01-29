#pragma once

#include "Request.hpp"
#include "Server.hpp"
#include "hm_popen.hpp"

#define SSTR(x) static_cast<std::ostringstream &>(std::ostringstream() << std::dec << x).str()
#define AAC "audio/aac"
#define BIN "application/octet-stream"
#define CSV "text/csv"
#define SVG "image/svg+xml"
#define PNG "image/png"
#define JPEG "image/jpeg"
#define HTML "text/html"
#define JS "text/javascript"
#define TXT "text/plain"
#define CSS "text/css"

#define PHP_ext ".php"
#define SH_ext ".sh"
#define PY_ext ".py"

class Response
{
private:
	const Request &_request;

	std::string Status_line;
	Map general_header;
	Map response_header;
	Map entity_header;

	int status_code;
	bool _is_binary;
	std::streamsize content_length;

	char *payload;
	std::string file_path;

	std::map<std::string, std::string> serv_param;
	std::map<std::string, std::string> serv;
	size_t client_size;

	// header ---------------------
	void build_header();
	bool match_file();
	void MIME_attribute();

	// server config ----------------
	void set_server_conf(Server &);

	//  payload ----------------------
	bool set_payload();
	bool read_payload_from_file();
	bool CGI_from_file(CGI);

	// concat  --------------------------
	void cMap_str(Map &m, std::string &s);
	std::string _response;

	// error response handler --------------
	void http_error(int);

public:
	Response(const Request &request, Server &s);
	~Response();

	// getter -----------------------------
	const int &get_status();
	const std::string &get_response() const;
};

std::ostream &operator<<(std::ostream &out, const Response &c);
