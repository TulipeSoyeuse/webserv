#include "Response.hpp"

Response::Response(const Request &r) : _request(r), Status_line("HTTP/1.1 "), _is_binary(false), payload(NULL)
{
    // --------------- HTTP version check ------------------------------
    std::pair<std::string, std::string> proto = *_request.get_request().find("Protocol");
    if (proto.second != "HTTP/1.1")
    {
        status_code = 505;
        (Status_line += SSTR(status_code << " ")) += "HTTP Version not supported\r\n";
        _response = (Status_line + "\n\n") + "\r\n";
    }
    // -----------------------------------------------------------------
    else
    {
        if (_request.get_type() == GET)
        {
            build_header();
            _response.assign(Status_line);
            cMap_str(general_header, _response);
            cMap_str(response_header, _response);
            cMap_str(entity_header, _response);
            _response += "\r\n";
            if (status_code == 200)
            {
                _response += payload;
                _response += "\r\n";
            }
        }
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
    else if (file_format == "svg")
    {
        entity_header["Content-Type"] = SVG "; charset=UTF-8\n";
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

void Response::Date()
{
    const time_t _time = time(NULL);
    struct tm *_tm = gmtime(&_time);

    // Date: <day-name>, <day> <month> <year> <hour>:<minute>:<second> GMT
    // <day-name>
    general_header["Date"] += "Date: ";
    if (_tm->tm_wday == 0)
        general_header["Date"] += "Mon, ";
    else if (_tm->tm_wday == 1)
        general_header["Date"] += "Tue, ";
    else if (_tm->tm_wday == 2)
        general_header["Date"] += "Wed, ";
    else if (_tm->tm_wday == 3)
        general_header["Date"] += "Thu, ";
    else if (_tm->tm_wday == 4)
        general_header["Date"] += "Fri, ";
    else if (_tm->tm_wday == 5)
        general_header["Date"] += "Sat, ";
    else if (_tm->tm_wday == 6)
        general_header["Date"] += "Sun, ";

    // <day>
    if (_tm->tm_mday < 10)
        general_header["Date"] += '0';
    general_header["Date"] += _tm->tm_mday;

    // <month> One of "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul",
    // "Aug", "Sep", "Oct", "Nov", "Dec" (case sensitive).
    if (_tm->tm_mon == 0)
        general_header["Date"] += " Jan ";
    else if (_tm->tm_mon == 1)
        general_header["Date"] += " Feb ";
    else if (_tm->tm_mon == 2)
        general_header["Date"] += " Mar ";
    else if (_tm->tm_mon == 3)
        general_header["Date"] += " Apr ";
    else if (_tm->tm_mon == 4)
        general_header["Date"] += " May ";
    else if (_tm->tm_mon == 5)
        general_header["Date"] += " Jun ";
    else if (_tm->tm_mon == 6)
        general_header["Date"] += " jul ";
    else if (_tm->tm_mon == 7)
        general_header["Date"] += " Aug ";
    else if (_tm->tm_mon == 8)
        general_header["Date"] += " Sep ";
    else if (_tm->tm_mon == 9)
        general_header["Date"] += " Oct ";
    else if (_tm->tm_mon == 10)
        general_header["Date"] += " Nov ";
    else if (_tm->tm_mon == 11)
        general_header["Date"] += " Dec ";

    // <year>
    general_header["Date"] += 1900 + _tm->tm_year;
    general_header["Date"] += " ";

    // <hour>:<minute>:<second> GMT
    (_response += _tm->tm_hour) += ':';
    ((_response += _tm->tm_min) += ':') += _tm->tm_sec;
    _response += " GMT\n";
}

void Response::build_header()
{
    general_header["Server"] = "webserv/0.1\n";
    // Date();
    if (match_file())
        (Status_line += SSTR(status_code)) += " Sucess\r\n";
    else
    {
        (Status_line += SSTR(status_code)) += " Not Found\r\n";
        return;
    }
    if (status_code == 200)
    {
        MIME_attribute();
        set_payload();
        entity_header["Content-Length"] = SSTR(content_length);
    }
}

bool Response::match_file()
{
    // TODO: implement location
    const std::string root_dir = "/code/site-test1";
    std::string uri = _request.get_request().find("URI")->second;
    DIR *dir = opendir((root_dir + uri.substr(0, uri.find_last_of('/'))).c_str());
    struct dirent *diread;

    // TODO: check if URI here ?
    if (std::strcmp("/", uri.c_str()) == 0)
    {
        uri.assign("/index.html");
    }

    const std::string file = uri.substr(uri.find_last_of('/') + 1);
    std::cout << "file asked: " << "\"" << file << "\"" << "\n";
    std::cout << "full path: " << "\"" << std::string("/code/site-test1").append(uri).c_str() << "\"" << "\n";

    while ((diread = readdir(dir)) != NULL)
    {
        if (std::strcmp(diread->d_name, file.c_str()) == 0)
        {
            struct stat _stat;
            stat(uri.c_str(), &_stat);
            // std::cout << "ID of device containing file: " << _stat.st_dev << "\n"
            //           << "File type and mode: " << _stat.st_mode << "\n"
            //           << std::endl;
            file_path = std::string("/code/site-test1").append(uri).c_str();
            status_code = 200;
            closedir(dir);
            return true;
        }
    }
    status_code = 404;
    closedir(dir);
    return false;
}

bool Response::set_payload()
{
    std::ifstream f;

    // --------------- extension check ------------------------------
    if (_is_binary)
        f.open(file_path.c_str(), std::ifstream::out | std::ifstream::binary);
    else
        f.open(file_path.c_str(), std::ifstream::out);
    // --------------------------------------------------------------
    if (f.good())
    {
        f.ignore(std::numeric_limits<std::streamsize>::max());
        content_length = f.gcount();
        f.clear();
        f.seekg(0, std::ios_base::beg);
        payload = new char[content_length + 1];
        for (unsigned int i = 0; i <= content_length; i++)
            payload[i] = 0;
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

std::ostream &operator<<(std::ostream &out, const Response &c)
{
    out << c._response;
    return (out);
}
