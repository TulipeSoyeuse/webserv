#!/usr/bin/env python
import time
import requests
import unittest
import subprocess
import sys
import signal
from urllib.parse import urljoin

from unittest_ressources.unitest_global import *

BASE_URL = "http://localhost:9997"


class TestRequestMethods(unittest.TestCase):
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

    # ------------------------------------ TEST POST -----------------------------------
    def test_POST1(self):
        "LOREM IPSUM + right test"
        response = requests.post(urljoin(BASE_URL, "cgi-bin/post.py"), data=LOREM_IPSUM)
        self.assertEqual(LOREM_IPSUM, response.content.decode())

    def test_POST2(self):
        "EMPTY"
        response = requests.post(urljoin(BASE_URL, "cgi-bin/post.py"))
        self.assertEqual("", response.content.decode())

    # ------------------------------------ TEST CONF -----------------------------------
    def test_autoindex(self):
        pass


if __name__ == "__main__":
    unittest.main()
