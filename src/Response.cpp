#include "Response.hpp"

Response::Response(const Request &r, Server &s) : _request(r), Status_line("HTTP/1.1 "),
                                                  _is_binary(false), payload(NULL)
{
    // --------------- HTTP version check ------------------------------
    std::pair<std::string, std::string> proto = *_request.get_request().find("Protocol");
    if (proto.second != "HTTP/1.1")
    {
        status_code = 505;
        (Status_line += SSTR(status_code << " ")) += "HTTP Version not supported\r\n";
        _response = Status_line + "\r\n";
    }
    // -----------------------------------------------------------------
    else
    {
        set_server_conf(s);
        if (_request.get_type() == GET)
        {
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
    // std::cout << " -> substr:" << uri.substr(0, uri.find_last_of('/')) << ":" << uri.length() << "\n";
    DIR *dir = opendir((root_dir + uri.substr(0, uri.find_last_of('/'))).c_str());
    struct dirent *diread;

    if (std::strcmp("/", uri.c_str()) == 0)
    {
        uri.assign("/index.html");
    }

    const std::string file = uri.substr(uri.find_last_of('/') + 1);
    std::cout << "file asked: " << "\"" << file << "\"" << "\n";
    std::cout << "full path: " << "\"" << serv.find("location")->second << uri.c_str() << "\"" << "\n";

    while ((diread = readdir(dir)) != NULL)
    {
        if (std::strcmp(diread->d_name, file.c_str()) == 0)
        {
            struct stat _stat;
            stat(uri.c_str(), &_stat);
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

void Response::set_server_conf(Server &s)
{
    std::string host = _request.get_request().find("Host")->second;
    serv = s.get_config(host);
}

bool Response::set_payload()
{
    std::ifstream f;
    // --------------- extension check ------------------------------
    if (_is_binary)
        f.open(file_path.c_str(), std::ifstream::binary);
    else
        f.open(file_path.c_str());
    // --------------------------------------------------------------
    if (f.good())
    {
        std::cout << "is binary : " << _is_binary << "\n";
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
