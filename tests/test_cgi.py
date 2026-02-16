#!/usr/bin/env python3
"""
CGI Test Suite for Webserv
Tests GET, POST, error handling, and edge cases
"""

import requests
import sys
import time

BASE_URL = "http://localhost:8080"

class Colors:
    GREEN = '\033[92m'
    RED = '\033[91m'
    YELLOW = '\033[93m'
    BLUE = '\033[94m'
    END = '\033[0m'

def test(name, condition, details=""):
    if condition:
        print(f"{Colors.GREEN}✓ PASS{Colors.END}: {name}")
        return True
    else:
        print(f"{Colors.RED}✗ FAIL{Colors.END}: {name} {details}")
        return False

def test_cgi_get_hello():
    """Test basic CGI GET request"""
    print(f"\n{Colors.BLUE}=== Testing CGI GET: hello.py ==={Colors.END}")
    try:
        r = requests.get(f"{BASE_URL}/cgi-bin/hello.py", timeout=5)
        test("Status 200", r.status_code == 200, f"got {r.status_code}")
        test("Content-Type text/html", "text/html" in r.headers.get("Content-Type", ""))
        test("Body contains 'Hello'", "Hello" in r.text)
        return True
    except Exception as e:
        print(f"{Colors.RED}Error: {e}{Colors.END}")
        return False

def test_cgi_get_env():
    """Test CGI environment variables"""
    print(f"\n{Colors.BLUE}=== Testing CGI GET: env.py ==={Colors.END}")
    try:
        r = requests.get(f"{BASE_URL}/cgi-bin/env.py", timeout=5)
        test("Status 200", r.status_code == 200)
        test("Shows REQUEST_METHOD", "REQUEST_METHOD" in r.text)
        test("Shows SERVER_PORT", "SERVER_PORT" in r.text)
        return True
    except Exception as e:
        print(f"{Colors.RED}Error: {e}{Colors.END}")
        return False

def test_cgi_get_query_string():
    """Test CGI with query string"""
    print(f"\n{Colors.BLUE}=== Testing CGI GET with Query String ==={Colors.END}")
    try:
        r = requests.get(f"{BASE_URL}/cgi-bin/env.py?name=test&value=42", timeout=5)
        test("Status 200", r.status_code == 200)
        test("Query string in response", "name=test" in r.text or "QUERY_STRING" in r.text)
        return True
    except Exception as e:
        print(f"{Colors.RED}Error: {e}{Colors.END}")
        return False

def test_cgi_post_form():
    """Test CGI POST with form data"""
    print(f"\n{Colors.BLUE}=== Testing CGI POST: form.py ==={Colors.END}")
    try:
        data = "name=Alice&age=30"
        headers = {"Content-Type": "application/x-www-form-urlencoded"}
        r = requests.post(f"{BASE_URL}/cgi-bin/form.py", data=data, headers=headers, timeout=5)
        test("Status 200", r.status_code == 200)
        test("Shows POST method", "POST" in r.text)
        test("Shows received data", "Alice" in r.text or "name" in r.text)
        return True
    except Exception as e:
        print(f"{Colors.RED}Error: {e}{Colors.END}")
        return False

def test_cgi_404():
    """Test CGI script not found"""
    print(f"\n{Colors.BLUE}=== Testing CGI 404: nonexistent.py ==={Colors.END}")
    try:
        r = requests.get(f"{BASE_URL}/cgi-bin/nonexistent.py", timeout=5)
        test("Status 404", r.status_code == 404, f"got {r.status_code}")
        return True
    except Exception as e:
        print(f"{Colors.RED}Error: {e}{Colors.END}")
        return False

def test_cgi_error():
    """Test CGI script with runtime error"""
    print(f"\n{Colors.BLUE}=== Testing CGI Error: error.py ==={Colors.END}")
    try:
        r = requests.get(f"{BASE_URL}/cgi-bin/error.py", timeout=5)
        # Server should not crash, may return partial output or 500
        test("Server responds (no crash)", r.status_code in [200, 500], f"got {r.status_code}")
        print(f"  {Colors.YELLOW}Note: Script has runtime error, server handled it{Colors.END}")
        return True
    except Exception as e:
        print(f"{Colors.RED}Error: {e}{Colors.END}")
        return False

def test_server_still_alive():
    """Verify server is still running after all tests"""
    print(f"\n{Colors.BLUE}=== Verifying Server Health ==={Colors.END}")
    try:
        r = requests.get(f"{BASE_URL}/", timeout=5)
        test("Server still responding", r.status_code == 200)
        return True
    except Exception as e:
        test("Server still responding", False, str(e))
        return False

def main():
    print(f"{Colors.BLUE}{'='*50}")
    print("        WEBSERV CGI TEST SUITE")
    print(f"{'='*50}{Colors.END}")
    print(f"Target: {BASE_URL}")
    
    passed = 0
    failed = 0
    
    tests = [
        test_cgi_get_hello,
        test_cgi_get_env,
        test_cgi_get_query_string,
        test_cgi_post_form,
        test_cgi_404,
        test_cgi_error,
        test_server_still_alive,
    ]
    
    for t in tests:
        try:
            if t():
                passed += 1
            else:
                failed += 1
        except Exception as e:
            print(f"{Colors.RED}Test crashed: {e}{Colors.END}")
            failed += 1
    
    print(f"\n{Colors.BLUE}{'='*50}")
    print(f"        RESULTS: {passed} passed, {failed} failed")
    print(f"{'='*50}{Colors.END}")
    
    return 0 if failed == 0 else 1

if __name__ == "__main__":
    sys.exit(main())
