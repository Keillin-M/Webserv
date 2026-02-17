#!/usr/bin/env python3
# CGI script to handle POST form data

import os
import sys

print("Content-Type: text/html\r")
print("\r")

print("<html>")
print("<head><title>Form Handler</title></head>")
print("<body>")
print("<h1>CGI Form Handler</h1>")

# Get request method
method = os.environ.get('REQUEST_METHOD', 'UNKNOWN')
print("<p><strong>Request Method:</strong> " + method + "</p>")

if method == 'POST':
    # Read POST data from stdin
    content_length = os.environ.get('CONTENT_LENGTH', '0')
    if content_length.isdigit():
        length = int(content_length)
        post_data = sys.stdin.read(length)
        
        print("<h2>Received POST Data:</h2>")
        print("<pre>" + post_data + "</pre>")
        
        # Parse simple form data (key=value&key=value)
        if post_data:
            print("<h2>Parsed Parameters:</h2>")
            print("<ul>")
            pairs = post_data.split('&')
            for pair in pairs:
                if '=' in pair:
                    key, value = pair.split('=', 1)
                    # Simple URL decode for spaces
                    value = value.replace('+', ' ')
                    print("<li><strong>" + key + ":</strong> " + value + "</li>")
            print("</ul>")
    else:
        print("<p>No POST data received.</p>")

elif method == 'GET':
    query = os.environ.get('QUERY_STRING', '')
    print("<p><strong>Query String:</strong> " + query + "</p>")

print("<hr>")
print("<h2>Test Form</h2>")
print('<form method="POST" action="/cgi-bin/py/form.py">')
print('  <label>Name: <input type="text" name="name" value="John"></label><br>')
print('  <label>Age: <input type="text" name="age" value="25"></label><br>')
print('  <button type="submit">Submit</button>')
print('</form>')

print("</body>")
print("</html>")
