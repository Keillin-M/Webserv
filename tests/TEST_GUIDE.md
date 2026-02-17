# Webserv Test Guide

## Overview

This guide describes how to run and validate all major tests for your Webserv project, including error pages, CGI, stress scenarios, scalability, redirects, and front-end validation.

---

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

---

## 🚀 Quick Start: Full Automated Test Suite

1. **Start Webserv**
		```bash
		./webserv config/default.conf
		```
	 *(Make sure your errors and sample pages are in `www/`!)*

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

---

## 📝 Expected Results

- ✅ Correct HTTP status code (`404`, `403`, etc)
- ✅ Response body contains your custom error page (include a marker for script detection, e.g., `Custom error page from webserv`)
- ✅ No server crashes or resource leaks under stress

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

---

## 🏗️ Extending Test Coverage

- Add new `.sh` or Python tests to `tests/` for new features or bug checks.
- Edit the test scripts to add more URLs or methods.

---

## 🤖 CI Integration

You can automate testing in CI pipelines by calling:
```yaml
- name: Run all tests
	run: bash tests/full_test.sh
```
