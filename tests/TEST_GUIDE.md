# Webserv Test Guide

## Overview

This guide describes how to run and validate all major tests for your Webserv project, including error pages, CGI, stress scenarios, scalability, redirects, and front-end validation.

---

## 🌐 Testing in Your Browser

You can also test the server and error handling visually using any web browser. Just start your server and navigate to:

- `http://localhost:8080/` — Main index page
- `http://localhost:8080/pages/` — Browse available frontend pages
- Try visiting non-existent URLs (e.g., `/nonexistent.html`) or forbidden paths to see custom error pages

The frontend in `www/pages/` provides user-friendly interfaces for exploring features, forms, and error handling. This is a great way to validate both backend and frontend integration.


## 📂 Test Assets and Structure

- **Custom Error Pages**:
	Place in `www/errors/`
	- `www/errors/400.html` — Bad Request
	- `www/errors/403.html` — Forbidden
	- `www/errors/404.html` — Not Found
	- `www/errors/405.html` — Method Not Allowed
	- `www/errors/500.html` — Internal Server Error
	- `www/errors/501.html` — Not Implemented
	- `www/errors/505.html` — HTTP Version Not Supported

- **Config files**:
	- `config/default.conf` — Main server config (edit for different port/tests if needed)

- **Test Scripts** (run from repo root):
	- `tests/cgi_test.py` — CGI and Python script tests
	- `tests/full_test.sh` — Full integration and error page tests

- **Stress Testing with Siege**:
	- `siege -c10 -t30S http://localhost:8080/` — 10 concurrent users for 30 seconds
	- `siege -c20 -r100 http://localhost:8080/` — 20 concurrent users, 100 requests each

---

## 🚀 Quick Start: Full Automated Test Suite

1. **Start Webserv**
		```bash
		./webserv config/default.conf
		```

2. **Run All Tests**
		```bash
		bash tests/full_test.sh
		```

---

## 🛠️ Running Specific Tests

You can run any script individually. Examples:

```bash
bash tests/cgi_test.py                # CGI and Python script tests
bash tests/full_test.sh               # Full integration and error page tests
```

---

## 🔎 Manual Testing Examples

- **404 Not Found:**
		```bash
		curl -i http://localhost:8080/nonexistent.html
		```
		Should return a `404` status *and* your custom 404 HTML.

- **405 Method Not Allowed:**
		```bash
		curl -X POST http://localhost:8080/
		```

- **403 Forbidden (directory traversal):**
		```bash
		curl "http://localhost:8080/../etc/passwd"
		```

- **Bad Request (400):**
		```bash
		curl -X POST http://localhost:8080/upload
		```

- **501 Not Implemented:**
		```bash
		curl -X PATCH http://localhost:8080/
		```

- **client_max_body_size, test:**

	```bash
	curl -X POST -H "Content-Type: text/plain" --data "Hello world" http://localhost:8080/
	```

---

## 📨 GET, POST, DELETE and Unknown Requests

- **GET — basic page:**
	```bash
	curl -i http://localhost:8080/
	curl -i http://localhost:8080/pages/about.html
	```

- **GET — via telnet:**
	```bash
	telnet localhost 8080
	```
	Then type (blank line at the end):
	```
	GET / HTTP/1.1
	Host: localhost

	```

- **POST — upload a file:**
	```bash
	curl -i -X POST -H "Content-Type: plain/text" --data "hello world" http://localhost:8080/uploads/hello.txt
	```

- **GET — retrieve the uploaded file:**
	```bash
	curl -i http://localhost:8080/uploads/xx.txt
	```

- **DELETE — delete the file:**
	```bash
	curl -i -X DELETE http://localhost:8080/uploads/xx.txt
	```

- **Unknown method — server must not crash (should return 501):**
	```bash
	curl -i -X FOOBAR http://localhost:8080/
	```
## 📝 Expected Results

- ✅ Correct HTTP status code (`404`, `403`, etc)
- ✅ Response body contains your custom error page (include a marker for script detection, e.g., `Custom error page from webserv`)
- ✅ No server crashes or resource leaks under stress

## 🐍 CGI Tests

- **Python CGI — GET hello world:**
	```bash
	curl -i http://localhost:8080/cgi-bin/py/hello.py
	```
	Should return `200 OK` + HTML with "Hello from CGI!"

- **Python CGI — GET environment variables:**
	```bash
	curl -i http://localhost:8080/cgi-bin/py/env.py
	```
	Should return `200 OK` + HTML table with `REQUEST_METHOD`, `QUERY_STRING`, etc.

- **Python CGI — GET with query string:**
	```bash
	curl -i "http://localhost:8080/cgi-bin/py/env.py?name=websurvival&school=42"
	```
	Should return `QUERY_STRING` populated with the values in the response.

- **Python CGI — POST with body:**
	```bash
	curl -i -X POST -H "Content-Type: application/x-www-form-urlencoded" \
	  --data "name=websurvival&school=42" http://localhost:8080/cgi-bin/py/form.py
	```
	Should return `200 OK` + HTML with the parsed POST data.

- **PHP CGI — GET hello world:**
	```bash
	curl -i http://localhost:8080/cgi-bin/php/hello.php
	```
	Should return `200 OK` + HTML with PHP version and "PHP CGI is working!"

If a script is missing or fails, you should see the appropriate error page (e.g., 404 or 500).

## 🔀 Redirection Tests

You can test HTTP redirection responses using curl or your browser. Example:

- **301/302/307 Redirect (curl):**
	```bash
	curl -i http://localhost:8080/redirect-301
	curl -i http://localhost:8080/redirect-302
	curl -i http://localhost:8080/redirect-307
	```
	You should see a `Location:` header and the appropriate status code (301, 302, or 307).

- **In the browser:**
	- Visit `http://localhost:8080/redirect-302` or similar URLs. You should be redirected to the target page (e.g., `/pages/about.html`).

---

## 🌍 Virtual Host / Custom Hostname Tests

These commands simulate requests using a custom hostname without modifying `/etc/hosts`, using `--resolve` to map the hostname to `127.0.0.1`:

```bash
# Custom hostname - example.com
curl --resolve example.com:8080:127.0.0.1 -s -o /dev/null -w "example.com: %{http_code}\n" http://example.com:8080/

# Custom hostname - test.local
curl --resolve test.local:8080:127.0.0.1 -s -o /dev/null -w "test.local: %{http_code}\n" http://test.local:8080/

# Custom hostname - api.server.com
curl --resolve api.server.com:8080:127.0.0.1 -s -o /dev/null -w "api.server.com: %{http_code}\n" http://api.server.com:8080/
```
All should return `200`.

---

## ☑️ Tips and Troubleshooting

- **If tests fail with “default error page”:**
	- Ensure all needed HTML files are present and readable in `www/errors/`.
	- Check your webserv config error_page directives match those files.
	- The server process must have permissions to read them.
	- Double-check your server actually loads and returns these files on error.

- **If CGI/POST/Uploads fail:**
	- Confirm scripts in `www/cgi-bin/` are present and executable and `cgi_path`/`cgi_extension` in config is correct.
	- Validate your config for allowed methods in location blocks.
