import requests
import unittest


BASE_URL = "http://localhost:9997"


class TestRequestMethods(unittest.TestCase):
    def test_get(self):
        response = requests.get(BASE_URL, headers=("Host", "www.webserv_test.fr"))
        self.assertEqual(response.status_code == 200)
        # self.assertEqual()


# with open("kitten.jpg", "rb") as f:
#     requests.put("http://localhost:9997/upload/python_script/kitten.jpg", data=f.read())



if __name__=="__main__":
