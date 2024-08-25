#include "Response.hpp"

Response::Response(const Request &r) : _request(r)
{
    if (_request.get_type() == GET)
    {
    }
}

Response::~Response()
{
}
