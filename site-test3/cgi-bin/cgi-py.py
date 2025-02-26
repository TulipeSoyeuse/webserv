#!/usr/bin/python3
import os

# Récupérer la QUERY_STRING depuis l'environnement
query_string = os.environ.get("QUERY_STRING", "")


# Fonction pour décoder les caractères spéciaux
def decode_query_string(query_string):
    decoded_string = ""
    i = 0
    while i < len(query_string):
        if query_string[i] == "%":
            hex_value = query_string[i + 1 : i + 3]
            decoded_string += chr(int(hex_value, 16))
            i += 3
        elif query_string[i] == "+":
            decoded_string += " "
            i += 1
        else:
            decoded_string += query_string[i]
            i += 1
    return decoded_string


decoded_query = decode_query_string(query_string)

param1 = None
if "param1=" in query_string:
    start_index = query_string.find("param1=") + len("param1=")
    end_index = query_string.find("&", start_index)
    if end_index == -1:
        end_index = len(query_string)
    param1 = query_string[start_index:end_index]

if param1:
    print(f"<h1>Bonjour, {param1} !</h1>")
else:
    print("<h1>Bonjour, aucune donnée reçue dans la query string.</h1>")
