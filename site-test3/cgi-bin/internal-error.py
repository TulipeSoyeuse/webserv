#!/usr/bin/env python3

print("Content-Type: text/html\n")  # Envoi d'en-tête HTTP

# Division par zéro (génère une exception)
result = 1 / 0
print(f"<html><body>Result: {result}</body></html>")