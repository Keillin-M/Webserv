#!/usr/bin/env python3
# CGI script with syntax/runtime error

print("Content-Type: text/html\r")
print("\r")
print("<h1>About to crash...</h1>")

# This will cause a runtime error
undefined_variable
