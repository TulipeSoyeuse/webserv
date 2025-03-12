#!/usr/bin/env python
import requests
import sys
import unittest
import subprocess
import os
from urllib.parse import urljoin
from datetime import datetime
import time
import pathlib
import re

from unittest_ressources.unitest_global import *


def do_skip():
    regexp = re.compile(r"TestRequestCONFIG.*")
    if len(sys.argv) == 2 and regexp.search(sys.argv[1]):
        return True
    else:
        return False


# ------------------------------------ TEST GET -----------------------------------
class TestRequestGET(unittest.TestCase):
    def __init__(self, methodName="runTest"):
        super().__init__(methodName)
        self.maxDiff = 200

    def test_GET1(self):
        "GET REQUEST : index"
        response = requests.get(BASE_URL, headers={"Host": "www.webserv_test.fr"})
        self.assertEqual(response.status_code, 200)
        with open("site-test3/index.html") as f:
            self.assertEqual(f.read(), response.content.decode())

    def test_GET2(self):
        "GET REQUEST : QUERY STRING SIMPLE"
        response = requests.get(
            urljoin(BASE_URL, "cgi-bin/cgi-py.py"),
            headers={"Host": "www.webserv_test.fr"},
            params={"test_param": "test_value"},
        )
        self.assertEqual(response.status_code, 200)
        with open("unittest_ressources/get_query_string_1.html") as f:
            self.assertEqual(f.read(), response.content.decode())

    def test_GET3(self):
        "GET REQUEST : QUERY STRING COMPLEX"
        response = requests.get(
            urljoin(BASE_URL, "cgi-bin/cgi-py.py"),
            headers={"Host": "www.webserv_test.fr"},
            params={
                "test_param": ["test_value", "test_value_2"],
                "test param": "an encoded value",
            },
        )
        self.assertEqual(response.status_code, 200)
        with open("unittest_ressources/get_query_string_2.html") as f:
            self.assertEqual(f.read(), response.content.decode())

    def test_GET4(self):
        "GET REQUEST : index (testing 3)"
        response = requests.get(BASE_URL2, headers={"Host": "www.webserv_test_demo.fr"})
        self.assertEqual(response.status_code, 200)
        with open("site-test2/index.html") as f:
            self.assertEqual(f.read(), response.content.decode())


# ------------------------------------- TEST POST -----------------------------------


class TestRequestPOST(unittest.TestCase):
    def __init__(self, methodName="runTest"):
        super().__init__(methodName)
        self.maxDiff = 80

    def test_POST1(self):
        "LOREM IPSUM + right test"
        response = requests.post(
            urljoin(BASE_URL, "cgi-bin/post.py"),
            data=LOREM_IPSUM,
            headers={"Host": "www.webserv_test.fr"},
        )
        self.assertEqual(response.status_code, 200)
        self.assertEqual(LOREM_IPSUM, response.content.decode())

    def test_POST2(self):
        "EMPTY"
        response = requests.post(
            urljoin(BASE_URL, "cgi-bin/post.py"),
            headers={"Host": "www.webserv_test.fr"},
        )
        self.assertEqual(response.status_code, 200)
        self.assertEqual("", response.content.decode())


# ----------------------------------- TEST AUTOINDEX ---------------------------------


class TestRequestAUTO(unittest.TestCase):
    def __init__(self, methodName="runTest"):
        super().__init__(methodName)
        self.maxDiff = 80

    def test_autoindex1(self):
        "autoindex on allowed directory"
        response = requests.get(
            urljoin(BASE_URL, "cgi-bin/"), headers={"Host": "www.webserv_test.fr"}
        )
        self.assertEqual(response.status_code, 200)
        with open("unittest_ressources/autoindex_cgi.html") as f:
            self.assertEqual(f.read(), response.content.decode())

    def test_autoindex2(self):
        "forbiden autoindex"
        response = requests.get(
            urljoin(BASE_URL, "upload/"), headers={"Host": "www.webserv_test.fr"}
        )
        self.assertEqual(response.status_code, 403)
        with open("site-test3/error/error_403.html") as f:
            self.assertEqual(f.read(), response.content.decode())

    def test_autoindex3(self):
        "tricky allowed autoindex"
        response = requests.get(
            BASE_URL + "/cgi-bin", headers={"Host": "www.webserv_test.fr"}
        )
        self.assertEqual(response.status_code, 200)
        with open("unittest_ressources/autoindex_cgi.html") as f:
            self.assertEqual(f.read(), response.content.decode())

    def test_autoindex4(self):
        "autoindex with index directive"
        response = requests.get(
            BASE_URL + "/html", headers={"Host": "www.webserv_test.fr"}
        )
        self.assertEqual(response.status_code, 200)
        with open("site-test3/html/definitlynotaindex.txt") as f:
            self.assertEqual(f.read(), response.content.decode())


# ------------------------------------ TEST ERROR ------------------------------------


class TestRequestERROR(unittest.TestCase):
    def __init__(self, methodName="runTest"):
        super().__init__(methodName)
        self.maxDiff = 80

    def test_error1(self):
        "404 - Bad Request with host"
        response = requests.get(
            urljoin(BASE_URL, "this page does not exits"),
            headers={"Host": "www.webserv_test.fr"},
        )
        self.assertEqual(response.status_code, 404)
        with open("error_pages/error_404.html") as f:
            self.assertEqual(f.read(), response.content.decode())

    def test_error2(self):
        "400"
        response = requests.get(
            urljoin(BASE_URL, "this page does not exits"),
            headers={"Host": "www.this_is_not_a_valid_host.fr"},
        )
        self.assertEqual(response.status_code, 400)
        with open("site-test3/error/error_400.html") as f:
            self.assertEqual(f.read(), response.content.decode())

    def test_error3(self):
        "400 - no host"
        response = requests.get(BASE_URL, headers={"Host": ""})
        self.assertEqual(response.status_code, 400)
        with open("site-test3/error/error_400.html") as f:
            self.assertEqual(f.read(), response.content.decode())

    def test_error4(self):
        "405 - Method not allowed"
        response = requests.post(
            BASE_URL,
            headers={"Host": "www.webserv_test.fr"},
        )
        self.assertEqual(response.status_code, 405)
        with open("site-test3/error/error_405.html") as f:
            self.assertEqual(f.read(), response.content.decode())

    def test_error5(self):
        "408 - Time out"
        response = requests.get(
            urljoin(BASE_URL, "cgi-bin/infinite.py"),
            headers={"Host": "www.webserv_test.fr"},
        )
        self.assertEqual(response.status_code, 444)
        with open("site-test3/error/error_444.html") as f:
            self.assertEqual(f.read(), response.content.decode())

    def test_error6(self):
        "500 - internal server"
        response = requests.get(
            urljoin(BASE_URL, "/cgi-bin/internal-error.py"),
            headers={"Host": "www.webserv_test.fr"},
        )
        self.assertEqual(response.status_code, 500)
        with open("site-test3/error/error_500.html") as f:
            self.assertEqual(f.read(), response.content.decode())

    def test_error7(self):
        "405 - error priority"
        response = requests.post(
            urljoin(BASE_URL, "site-test3/cgi-bin/post_chunk.py"),
            headers={"Host": "www.webserv_test2.fr"},
            data="blabla",
        )
        self.assertEqual(response.status_code, 405)
        with open("error_pages/error_405.html") as f:
            self.assertEqual(f.read(), response.content.decode())


# ------------------------------------ TEST UPLOAD -----------------------------------


class TestRequestPUT(unittest.TestCase):
    def __init__(self, methodName="runTest"):
        super().__init__(methodName)
        self.maxDiff = 80

    def test_upload1(self):
        "simple txt file (authorized)"
        response = requests.put(
            urljoin(BASE_URL, "upload/.unittest_upload_file1.txt"),
            headers={"Host": "www.webserv_test.fr"},
            data=LOREM_IPSUM,
        )
        self.assertEqual(response.status_code, 201)
        self.assertTrue(os.path.exists(PUT_FILE1))
        try:
            with open(PUT_FILE1) as f:
                self.assertEqual(LOREM_IPSUM, f.read())
        finally:
            os.remove(PUT_FILE1)

    def test_upload2(self):
        "upload twice"
        response = requests.put(
            urljoin(BASE_URL, "upload/.unittest_upload_file1.txt"),
            headers={"Host": "www.webserv_test.fr"},
            data=LOREM_IPSUM,
        )
        self.assertEqual(response.status_code, 201)
        self.assertTrue(os.path.exists(PUT_FILE1))
        try:
            with open(PUT_FILE1) as f:
                self.assertEqual(LOREM_IPSUM, f.read())
            response = requests.put(
                urljoin(BASE_URL, "upload/.unittest_upload_file1.txt"),
                headers={"Host": "www.webserv_test.fr"},
                data=LOREM_IPSUM,
            )
            self.assertEqual(response.status_code, 403)
        finally:
            os.remove(PUT_FILE1)

    def test_upload3(self):
        "heavy 4K jpeg file (authorized)"
        with open("unittest_ressources/kitten.jpg", "br") as f:
            kitten = f.read()
        response = requests.put(
            urljoin(BASE_URL, "upload/.unittest_upload_file2.jpg"),
            headers={"Host": "www.webserv_test.fr"},
            data=kitten,
        )
        self.assertEqual(response.status_code, 201)
        self.assertTrue(os.path.exists(PUT_FILE2))
        try:
            with open(PUT_FILE2, "br") as f:
                self.assertEqual(kitten, f.read())
        finally:
            os.remove(PUT_FILE2)

    def test_upload4(self):
        "unauthorized upload"
        response = requests.put(
            urljoin(BASE_URL, "unittest_upload_file1.txt"),
            headers={"Host": "www.webserv_test.fr"},
            data=LOREM_IPSUM,
        )
        try:
            self.assertEqual(response.status_code, 405)
            self.assertFalse(os.path.exists("site-test3/unittest_upload_file1.txt"))
        except Exception as e:
            os.remove("site-test3/unittest_upload_file1.txt")
            raise e


# ------------------------------------ TEST CHUNK -----------------------------------


class TestRequestCHUNK(unittest.TestCase):
    def __init__(self, methodName="runTest"):
        super().__init__(methodName)
        self.maxDiff = 80

    def test_chunk1(self):
        "GET on chunk on a big txt file"
        with open("unittest_ressources/lorem_ipsum.txt") as f:
            loip = f.read()
        response = requests.put(
            urljoin(BASE_URL, "upload/dirchunk/.unittest_chunk_file1.txt"),
            headers={"Host": "www.webserv_test.fr"},
            data=loip,
        )
        self.assertEqual(response.status_code, 201)
        self.assertTrue(os.path.exists(CHUNK_FILE1))
        try:
            response = requests.get(
                urljoin(BASE_URL, "upload/dirchunk/.unittest_chunk_file1.txt"),
                headers={"Host": "www.webserv_test.fr"},
            )
            with open("unittest_ressources/response_file.txt", "w") as w:
                for k, v in response.headers.items():
                    w.write(f"{k}:{v}\n")
                w.write(response.content.decode())
            response.raise_for_status()
            self.assertEqual(response.headers.get("Transfer-Encoding", None), "chunked")
            self.assertEqual(loip, response.content.decode())
        finally:
            os.remove(CHUNK_FILE1)

    def test_chunk2(self):
        "GET on chunk on a big binary file"
        with open("unittest_ressources/kitten.jpg", "br") as f:
            kitten = f.read()
        response = requests.put(
            urljoin(BASE_URL, "upload/dirchunk/.unittest_chunk_file2.jpg"),
            headers={"Host": "www.webserv_test.fr"},
            data=kitten,
        )
        self.assertEqual(response.status_code, 201)
        self.assertTrue(os.path.exists(CHUNK_FILE2))
        try:
            response = requests.get(
                urljoin(BASE_URL, "upload/dirchunk/.unittest_chunk_file2.jpg"),
                headers={"Host": "www.webserv_test.fr"},
            )
            response.raise_for_status()
            self.assertEqual(kitten, response.content)
            self.assertEqual(response.headers.get("Transfer-Encoding", None), "chunked")
        finally:
            os.remove(CHUNK_FILE2)

    def test_chunk3(self):
        "chunk on regular file"
        response = requests.get(
            BASE_URL,
            headers={"Host": "www.webserv_test2.fr"},
        )
        response.raise_for_status()
        self.assertEqual(response.status_code, 200)
        with open("site-test3/index.html") as f:
            self.assertEqual(f.read(), response.content.decode())
        self.assertEqual(response.headers.get("Transfer-Encoding", None), "chunked")

    def test_chunk4(self):
        "chunk with CGI response"
        os.system(
            "cp unittest_ressources/post_chunk.py site-test3/cgi-bin/post_chunk.py"
        )
        response = requests.post(
            urljoin(BASE_URL, "cgi-bin/post_chunk.py"),
            headers={"Host": "www.webserv_test2.fr"},
            data="lorem_ipsum=yes",
        )
        try:
            self.assertEqual(response.status_code, 200)
            with open("unittest_ressources/lorem_ipsum.txt") as f:
                self.assertEqual(f.read(), response.content.decode())
            self.assertEqual(response.headers.get("Transfer-Encoding", None), "chunked")
        finally:
            os.remove("site-test3/cgi-bin/post_chunk.py")


# ------------------------------------ TEST CONFIG -----------------------------------


@unittest.skipUnless(do_skip(), "must be called directly")
class TestRequestCONFIG(unittest.TestCase):
    def __init__(self, methodName="runTest"):
        super().__init__(methodName)
        self.maxDiff = 80
        self.webserv = None
        self.addCleanup(self.cleanup)
        self.setup()

    def setup(self):
        if not os.path.exists("unittest_ressources/output.log"):
            pathlib.Path("unittest_ressources/output.log").touch()

        self.log = open("unittest_ressources/output.log", "a")

    def launch_webserv(self, config_file: str):
        if self.webserv and self.webserv.poll() != None:
            return
        else:
            self.webserv = subprocess.Popen(
                [
                    "valgrind",
                    "--track-origins=yes",
                    "--track-fds=yes",
                    "--leak-check=full",
                    "--show-leak-kinds=all",
                    "--error-exitcode=1",
                    "./webserv",
                    config_file,
                ],
                stdout=self.log,
                stderr=self.log,
            )
            time.sleep(0.5)

    def cleanup(self):
        if self.webserv and self.webserv.poll() == None:
            self.webserv.send_signal(2)
            time.sleep(0.5)
            if self.webserv and self.webserv.poll() == None:
                self.webserv.send_signal(2)
        time.sleep(0.5)
        self.log.close()

    def test_config1(self):
        self.log.write(
            LOG_SETUP.format(date=datetime.now(), test=self.test_config1.__name__)
        )
        "no host parameter"
        with open(TEST_CONFIG_FILE, "w") as conf:
            conf.write(TEST_CONFIG_1)
        self.launch_webserv(TEST_CONFIG_FILE)
        time.sleep(0.5)
        self.assertEqual(self.webserv.poll(), 2)

    def test_config2(self):
        "empty file"
        self.log.write(
            LOG_SETUP.format(date=datetime.now(), test=self.test_config2.__name__)
        )
        with open(TEST_CONFIG_FILE, "w") as conf:
            conf.write("")
        self.launch_webserv(TEST_CONFIG_FILE)
        time.sleep(0.5)
        self.assertEqual(self.webserv.poll(), 2)

    def test_config3(self):
        'return line after "location /"'
        self.log.write(
            LOG_SETUP.format(date=datetime.now(), test=self.test_config3.__name__)
        )
        with open(TEST_CONFIG_FILE, "w") as conf:
            conf.write(TEST_CONFIG_2)
        self.launch_webserv(TEST_CONFIG_FILE)
        time.sleep(0.5)
        self.assertEqual(self.webserv.poll(), 2)

    def test_config4(self):
        "no route"
        self.log.write(
            LOG_SETUP.format(date=datetime.now(), test=self.test_config4.__name__)
        )
        with open(TEST_CONFIG_FILE, "w") as conf:
            conf.write(TEST_CONFIG_3)
        self.launch_webserv(TEST_CONFIG_FILE)
        time.sleep(0.5)
        self.assertEqual(self.webserv.poll(), 2)

    def test_config5(self):
        "route don't exist"
        self.log.write(
            LOG_SETUP.format(date=datetime.now(), test=self.test_config5.__name__)
        )
        with open(TEST_CONFIG_FILE, "w") as conf:
            conf.write(TEST_CONFIG_4)
        self.launch_webserv(TEST_CONFIG_FILE)
        time.sleep(0.5)
        self.assertEqual(self.webserv.poll(), 2)

    def test_config6(self):
        "route is a file and not a directory"
        self.log.write(
            LOG_SETUP.format(date=datetime.now(), test=self.test_config6.__name__)
        )
        with open(TEST_CONFIG_FILE, "w") as conf:
            conf.write(TEST_CONFIG_5)
        self.launch_webserv(TEST_CONFIG_FILE)
        time.sleep(0.5)
        self.assertEqual(self.webserv.poll(), 2)

    def test_config7(self):
        "wierd but fonctional"
        self.log.write(
            LOG_SETUP.format(date=datetime.now(), test=self.test_config7.__name__)
        )
        with open(TEST_CONFIG_FILE, "w") as conf:
            conf.write(TEST_CONFIG_6)
        self.launch_webserv(TEST_CONFIG_FILE)
        time.sleep(0.5)
        self.assertIsNone(self.webserv.poll())


if __name__ == "__main__":
    try:
        os.mkdir("webserv/site-test3/upload/dirchunk")
    except OSError:
        pass

    unittest.main()
