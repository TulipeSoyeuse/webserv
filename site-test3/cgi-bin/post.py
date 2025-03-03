import sys
from urllib.parse import parse_qs

# Lire le stdin
data = sys.stdin.read()

if parse_qs(data):
    for k, v in parse_qs(data).items():
        print(k, ":", v)
else:
    print(data, end="")
