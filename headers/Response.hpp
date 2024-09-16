#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "Request.hpp"
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <cstring>
#include <string>
#include <fstream>
#include <ctime>
#include <limits>
#include <sstream>
#include <iterator>
#include <vector>

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

	// header --------------
	void build_header();
	bool match_file();
	void MIME_attribute();
	void Date();
	// payload -------------
	bool set_payload();
	void read_file(std::ifstream f);

	// concat  -------------
	void cMap_str(Map &m, std::string &s);
	std::string _response;

	// getter --------------

public:
	Response(const Request &request);
	~Response();

	const int &get_status();
	const std::string &get_response() const;
};

std::ostream &operator<<(std::ostream &out, const Response &c);

#endif
