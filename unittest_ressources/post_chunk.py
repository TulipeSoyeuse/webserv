import sys
from urllib.parse import parse_qs

# data = parse_qs(sys.stdin.read())
data = parse_qs("lorem_ipsum=yes")

try:
    if (data.get("lorem_ipsum", [])[0]) == "yes":
        with open("unittest_ressources/lorem_ipsum.txt") as f:
            print(f.read(), end="")
    else:
        print("error")
except Exception as e:
    print(e)
