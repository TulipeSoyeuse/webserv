#pragma once

#include "Request.hpp"
#include "Server.hpp"
#include "hm_popen.hpp"

#define SSTR(x) static_cast<std::ostringstream &>(std::ostringstream() << std::dec << x).str()
#define SSTRH(x) static_cast<std::ostringstream &>(std::ostringstream() << std::hex << x).str()

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

#define CHUNK_MINI 2000;
typedef std::pair<std::string, Map> p_location;

class Response
{
private:
	const Request &_request;

	std::string Status_line;
	Map _header;

	int status_code;
	bool _is_binary;
	bool _error;
	bool _chunk;
	bool _isdir;
	size_t chunk_size;

	std::streamsize content_length;

	bytes_container payload;
	std::string file_path;

	std::map<std::string, std::string> serv_param;
	server_m serv;
	size_t client_size;

	bytes_container _reste;
	bytes_container _response;

	bool last;
	Server &config;
	// header ---------------------
	bool match_file(); // deprecated
	bool check_file();
	void MIME_attribute();

	// server config ----------------
	void set_server_conf(Server &);

	// checking ---------------------
	void check_autoindex();
	bool check_proto();
	//  payload ----------------------
	bool is_body_size_valid() const;
	bool set_payload();
	bool read_payload_from_file();
	bool CGI_from_file(CGI);
	bool write_file();
	bool generate_autoindex();
	bool error_path();
	bool delete_file();

	// chunk   --------------------------
	void serv_by_chunk();

	// concat  --------------------------
	void cMap_str(Map &m, bytes_container &s);

	// error response handler --------------
	void http_error(int);
	// id create_error_page(int code);

	bool autoindex;
	std::string root_dir;

public:
	Response(const Request &request, Server &s);
	~Response();

	// getter -----------------------------
	const int &get_status();
	const bytes_container &get_response() const;
	const bool &is_binary() const;
	// chunk ------------------------------
	const bool &is_chunked() const;
	int get_next_chunk(bytes_container &);
};

std::ostream &operator<<(std::ostream &out, const Response &c);
