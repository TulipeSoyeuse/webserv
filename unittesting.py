#!/usr/bin/env python
import time
import requests
import unittest
import subprocess
import sys
import signal
import os
from urllib.parse import urljoin

from unittest_ressources.unitest_global import *

BASE_URL = "http://localhost:9997"


class TestRequest(unittest.TestCase):
    def __init__(self, methodName="runTest"):
        super().__init__(methodName)
        self.maxDiff = None

    # ------------------------------------ TEST GET -----------------------------------
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
        with open("test.txt", "w") as f:
            f.write(response.url)
        self.assertEqual(response.status_code, 200)
        with open("unittest_ressources/get_query_string_2.html") as f:
            self.assertEqual(f.read(), response.content.decode())

    # ------------------------------------- TEST POST -----------------------------------
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
            urljoin(BASE_URL, "html/"), headers={"Host": "www.webserv_test.fr"}
        )
        self.assertEqual(response.status_code, 403)
        with open("site-test3/error/error_403.html") as f:
            self.assertEqual(f.read(), response.content.decode())

    # ------------------------------------ TEST ERROR ------------------------------------
    def test_error1(self):
        "404"
        response = requests.get(
            urljoin(BASE_URL, "this page does not exits"),
            headers={"Host": "www.webserv_test.fr"},
        )
        self.assertEqual(response.status_code, 404)
        with open("site-test3/error/error_404.html") as f:
            self.assertEqual(f.read(), response.content.decode())

    def test_error2(self):
        "400"
        pass

    # ------------------------------------ TEST UPLOAD -----------------------------------
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
        self.assertEqual(response.status_code, 405)
        try:
            self.assertFalse(os.path.exists("site-test3/unittest_upload_file1.txt"))
        except Exception as e:
            os.remove("site-test3/unittest_upload_file1.txt")
            raise e

    # ------------------------------------ TEST CHUNK -----------------------------------

    def test_chunk(self):
        "GET on chunk on a big file"
        with open("unittest_ressources/kitten.jpg", "br") as f:
            kitten = f.read()
        response = requests.put(
            urljoin(BASE_URL, "upload/dirchunk/.unittest_chunk_file1.jpg"),
            headers={"Host": "www.webserv_test.fr"},
            data=kitten,
        )
        self.assertEqual(response.status_code, 201)
        self.assertTrue(os.path.exists(CHUNK_FILE1))
        try:
            response = requests.get(
                urljoin(BASE_URL, "upload/dirchunk/.unittest_chunk_file1.jpg"),
                headers={"Host": "www.webserv_test.fr"},
            )
            self.assertEqual(kitten, response.content)
        finally:
            os.remove(CHUNK_FILE1)


if __name__ == "__main__":
    try:
        os.mkdir("webserv/site-test3/upload/dirchunk")
    except OSError:
        pass
    unittest.main()
