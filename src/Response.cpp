#include "Response.hpp"

Response::Response(Request &r): _request(r)
{
    generate_response();
}

void Response::generate_response()
{
    if (_request.get_type() == GET)
    {
        
    }
}

Response::~Response()
{
}
