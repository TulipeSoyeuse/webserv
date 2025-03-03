#!/usr/bin/python3
import os
from urllib.parse import parse_qs, parse_qsl

# Récupérer la QUERY_STRING depuis l'environnement
HTML_TEMPLATE = """<html>
    <head>
        <title>Query GET request</title>
    </head>
    <body>
{}  </body>
</html>
"""

HTML_PAIR = "       <h2>{} : {}</h2>\n"

query_string = os.environ.get("QUERY_STRING", "")
# query_string = "param1=dae+dezc&pece=ezdz&pece=def"

decoded_query = parse_qs(query_string)
# print(decoded_query)
body = ""
for k, v in decoded_query.items():
    body += HTML_PAIR.format(k, v)

print(HTML_TEMPLATE.format(body))
