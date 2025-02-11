import requests

with open("kitten.jpg", "rb") as f:
    requests.put("http://localhost:9997/upload/python_script/kitten.jpg", data=f.read())
