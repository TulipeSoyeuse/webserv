import requests
import unittest


BASE_URL = "http://localhost:9997"


class TestRequestMethods(unittest.TestCase):
    def test_get(self):
        # ------------------ BASIC REQUEST : index ----------------
        response = requests.get(BASE_URL, headers={"Host": "www.webserv_test.fr"})
        self.assertEqual(response.status_code, 200)
        with open("site-test3/index.html") as f:
            self.assertEqual(f.read(), response.content.decode())
        # ------------------ BASIC REQUEST : string arg ----------------


# with open("kitten.jpg", "rb") as f:
#     requests.put("http://localhost:9997/upload/python_script/kitten.jpg", data=f.read())


if __name__ == "__main__":
    unittest.main()
