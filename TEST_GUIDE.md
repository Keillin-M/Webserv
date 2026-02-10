# Error Page Testing Guide

## Files Created

### Custom Error Pages
- `config/www/400.html` - Bad Request
- `config/www/403.html` - Forbidden
- `config/www/404.html` - Not Found
- `config/www/405.html` - Method Not Allowed
- `config/www/500.html` - Internal Server Error
- `config/www/501.html` - Not Implemented
- `config/www/505.html` - HTTP Version Not Supported

### Configuration Files
- `config/test.conf` - Webserv config with error_page directives
- `config/nginx_test.conf` - Nginx config for comparison (optional)

### Test Script
- `test_error_pages.sh` - Automated test suite

## Quick Start

### 1. Start Webserv
```bash
./webserv config/test.conf
```

### 2. Run Tests
```bash
./test_error_pages.sh
```

## Manual Tests

### Test 404 - Missing File
```bash
curl http://localhost:8080/nonexistent.html
```
Should return custom 404 error page.

### Test 405 - Wrong Method
```bash
curl -X POST http://localhost:8080/
```
Should return custom 405 error page (location only allows GET).

### Test 403 - Forbidden
```bash
curl "http://localhost:8080/../etc/passwd"
```
Should return custom 403 error page (directory traversal blocked).

### Test 400 - Bad Request
```bash
curl -X POST http://localhost:8080/upload
```
Should return custom 400 error page (empty POST body).

### Test 501 - Not Implemented
```bash
curl -X PATCH http://localhost:8080/
```
Should return custom 501 error page (PATCH not implemented).

### Test 200 - Success
```bash
curl http://localhost:8080/
```
Should return the index.html page successfully.

## With Nginx Comparison (Optional)

### 1. Start Nginx
```bash
sudo nginx -c $PWD/config/nginx_test.conf
```

### 2. Run Comparison Tests
The test script will automatically compare webserv (port 8080) with nginx (port 8081).

### 3. Stop Nginx
```bash
sudo nginx -s stop
```

## Expected Results

All error responses should:
1. ✅ Return correct HTTP status code
2. ✅ Display custom HTML error page
3. ✅ Include "Custom error page from webserv" marker
4. ✅ Match nginx behavior for status codes
