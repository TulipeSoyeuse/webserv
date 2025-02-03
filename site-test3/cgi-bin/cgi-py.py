import os
import sys

for k, v in os.environ.items():
    print("[{}]:{}".format(k, v))
