#!/usr/bin/env python3
# CGI script to display all environment variables

import os

print("Content-Type: text/html\r")
print("\r")

print("<html>")
print("<head>")
print("<title>CGI Environment Variables</title>")
print("<style>")
print("  body { font-family: monospace; margin: 20px; }")
print("  table { border-collapse: collapse; width: 100%; }")
print("  th, td { border: 1px solid #ddd; padding: 8px; text-align: left; }")
print("  th { background-color: #4CAF50; color: white; }")
print("  tr:nth-child(even) { background-color: #f2f2f2; }")
print("</style>")
print("</head>")
print("<body>")
print("<h1>CGI Environment Variables</h1>")

# Important CGI variables
cgi_vars = [
    'REQUEST_METHOD',
    'QUERY_STRING',
    'CONTENT_TYPE',
    'CONTENT_LENGTH',
    'SCRIPT_NAME',
    'PATH_INFO',
    'SERVER_NAME',
    'SERVER_PORT',
    'SERVER_PROTOCOL',
    'GATEWAY_INTERFACE'
]

print("<h2>Standard CGI Variables</h2>")
print("<table>")
print("<tr><th>Variable</th><th>Value</th></tr>")
for var in cgi_vars:
    value = os.environ.get(var, '<em>not set</em>')
    print("<tr><td><strong>" + var + "</strong></td><td>" + value + "</td></tr>")
print("</table>")

# HTTP headers
print("<h2>HTTP Headers (HTTP_*)</h2>")
print("<table>")
print("<tr><th>Variable</th><th>Value</th></tr>")
http_headers = {k: v for k, v in os.environ.items() if k.startswith('HTTP_')}
if http_headers:
    for key in sorted(http_headers.keys()):
        print("<tr><td><strong>" + key + "</strong></td><td>" + http_headers[key] + "</td></tr>")
else:
    print("<tr><td colspan='2'><em>No HTTP headers found</em></td></tr>")
print("</table>")

# All environment variables
print("<h2>All Environment Variables</h2>")
print("<table>")
print("<tr><th>Variable</th><th>Value</th></tr>")
for key in sorted(os.environ.keys()):
    value = os.environ[key]
    print("<tr><td>" + key + "</td><td>" + value + "</td></tr>")
print("</table>")

print("</body>")
print("</html>")
