LOREM_IPSUM = """Lorem ipsum dolor sit amet, consectetur adipiscing elit.
Nam fringilla turpis in nisi varius, sit amet mattis mi faucibus.
Nunc risus purus, dictum in dui in, gravida aliquam eros.
Nunc egestas ut enim quis ornare.
Sed massa nulla, condimentum consectetur nulla at, pretium lacinia sem.
In sit amet tellus magna.
Etiam fermentum mollis erat id vehicula.
Vivamus in est turpis.
Cras non ex molestie, mattis ligula vitae, imperdiet velit.
Nulla semper elementum turpis, in porta turpis dapibus rhoncus.
Nam pulvinar ligula a pretium mollis.
Donec sed eros a mauris dapibus suscipit non id sapien.
Nulla quis gravida elit.
Mauris quis nisl semper, venenatis enim a, vulputate erat.
Fusce varius enim vitae sem volutpat vehicula."""

PUT_FILE1 = "site-test3/upload/.unittest_upload_file1.txt"
PUT_FILE2 = "site-test3/upload/.unittest_upload_file2.jpg"

CHUNK_FILE1 = "site-test3/upload/dirchunk/.unittest_chunk_file1.txt"
CHUNK_FILE2 = "site-test3/upload/dirchunk/.unittest_chunk_file2.jpg"

BASE_URL = "http://localhost:9997"
BASE_URL2 = "http://localhost:9998"


TEST_CONFIG_FILE = "unittest_ressources/config_test.conf"
LOG_SETUP = """
\n--------------------------------- WEBSERV TESTING --------------------------------
DATE:{date}
\n"""

TEST_CONFIG_1 = """server testing
{
    host www.webserv_test.fr webserv_test.com
    port 9997
    error_page {
        400 error/error_400.html
        403 error/error_403.html
        405 error/error_405.html
        408 error/error_408.html
        500 error/error_500.html
        505 error/error_505.html
    }
    route   ./site-test3
    location / {
        index index.html
        proto GET
        autoindex off
        client_size 1500
    }
    location /cgi-bin {
        proto GET,POST
        autoindex on
    }
    location /upload {
        proto PUT,DELETE
        autoindex off
    }
    location /upload/dirchunk {
        proto PUT,GET
        client_size 40000
    }
}

server testing2
{
    port 9998
    location / {
        proto GET
    }

}"""

TEST_CONFIG_3 = """server testing
{
    host www.webserv_test.fr webserv_test.com
    port 9997
    error_page {
        400 error/error_400.html
        403 error/error_403.html
        405 error/error_405.html
        408 error/error_408.html
        500 error/error_500.html
        505 error/error_505.html
    }
    route   ./site-test3
    location /
    {
        index index.html
        proto GET
        autoindex off
        client_size 1500
    }
    location /cgi-bin
    {
        proto GET,POST
        autoindex on
    }
    location /upload
    {
        proto PUT,DELETE
        autoindex off
    }
    location /upload/dirchunk {
        proto PUT,GET
        client_size 40000
    }
}

server testing2
{
    host www.web_test2.fr
    port 9998
    host www.webserv_test.fr webserv_test.com


            port 9998

    location / {
        proto GET
    }
}"""

TEST_CONFIG_4 = """server testing
{
    host www.webserv_test.fr webserv_test.com
    port 9997
    error_page {
        400 error/error_400.html
        403 error/error_403.html
        405 error/error_405.html
        408 error/error_408.html
        500 error/error_500.html
        505 error/error_505.html
    }
    route   ./site-test3
    location / {
        index index.html
        proto GET
        autoindex off
        client_size 1500
    }
    location /cgi-bin {
        proto GET,POST
        autoindex on
    }
    location /upload {
        proto PUT,DELETE
        autoindex off
    }
    location /upload/dirchunk {
        proto PUT,GET
        client_size 40000
    }
}

server testing2
{
    host www.test.com
    port 9998
    route ./fzefazefea
    location / {
        proto GET
    }

}"""

TEST_CONFIG_5 = """server testing
{
    host www.webserv_test.fr webserv_test.com
    port 9997
    error_page {
        400 error/error_400.html
        403 error/error_403.html
        405 error/error_405.html
        408 error/error_408.html
        500 error/error_500.html
        505 error/error_505.html
    }
    route   ./site-test3
    location / {
        index index.html
        proto GET
        autoindex off
        client_size 1500
    }
    location /cgi-bin {
        proto GET,POST
        autoindex on
    }
    location /upload {
        proto PUT,DELETE
        autoindex off
    }
    location /upload/dirchunk {
        proto PUT,GET
        client_size 40000
    }
}

server testing2
{
    host www.test.com
    port 9998
    route ./unittest_ressources/lorem_ipsum.txt
    location / {
        proto GET
    }

}"""
