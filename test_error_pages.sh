#!/bin/bash

# Colors for output
GREEN='\033[0;32m'
RED='\033[0;31m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo "=========================================="
echo "   WEBSERV vs NGINX COMPARISON TESTS"
echo "=========================================="
echo ""

WEBSERV_PORT=8080
NGINX_PORT=8081

# Check if servers are running
check_server() {
    local port=$1
    local name=$2
    
    if curl -s -o /dev/null -w "%{http_code}" --max-time 2 http://localhost:$port > /dev/null 2>&1; then
        echo -e "${GREEN}✓ $name is running on port $port${NC}"
        return 0
    else
        echo -e "${RED}✗ $name is NOT running on port $port${NC}"
        echo "  Start it before running tests"
        return 1
    fi
}

# Compare responses
compare_response() {
    local test_name=$1
    local webserv_status=$2
    local nginx_status=$3
    local webserv_content=$4
    local nginx_content=$5
    
    echo ""
    echo -e "${BLUE}[Test] $test_name${NC}"
    echo "  Webserv: HTTP $webserv_status"
    echo "  Nginx:   HTTP $nginx_status"
    
    if [ "$webserv_status" = "$nginx_status" ]; then
        echo -e "${GREEN}  ✓ Status codes match${NC}"
    else
        echo -e "${RED}  ✗ Status codes differ${NC}"
    fi
    
    # Check for custom error page marker
    if echo "$webserv_content" | grep -q "Custom error page from webserv"; then
        echo -e "${GREEN}  ✓ Custom error page used${NC}"
    elif [ "$webserv_status" -ge 400 ]; then
        echo -e "${YELLOW}  ⚠ Using default error page${NC}"
    fi
}

# Test 1: 404 - Missing File
test_404() {
    echo ""
    echo -e "${YELLOW}=========================================="
    echo "TEST 1: Missing File (404)"
    echo -e "==========================================${NC}"
    
    webserv_resp=$(curl -s -w "\n%{http_code}" http://localhost:$WEBSERV_PORT/nonexistent.html)
    webserv_status=$(echo "$webserv_resp" | tail -1)
    webserv_content=$(echo "$webserv_resp" | head -n -1)
    
    nginx_resp=$(curl -s -w "\n%{http_code}" http://localhost:$NGINX_PORT/nonexistent.html)
    nginx_status=$(echo "$nginx_resp" | tail -1)
    nginx_content=$(echo "$nginx_resp" | head -n -1)
    
    compare_response "Missing file" "$webserv_status" "$nginx_status" "$webserv_content" "$nginx_content"
}

# Test 2: 405 - Wrong Method
test_405() {
    echo ""
    echo -e "${YELLOW}=========================================="
    echo "TEST 2: Wrong Method (405)"
    echo -e "==========================================${NC}"
    
    # POST to location that only allows GET
    webserv_resp=$(curl -s -X POST -w "\n%{http_code}" http://localhost:$WEBSERV_PORT/)
    webserv_status=$(echo "$webserv_resp" | tail -1)
    webserv_content=$(echo "$webserv_resp" | head -n -1)
    
    nginx_resp=$(curl -s -X POST -w "\n%{http_code}" http://localhost:$NGINX_PORT/)
    nginx_status=$(echo "$nginx_resp" | tail -1)
    nginx_content=$(echo "$nginx_resp" | head -n -1)
    
    compare_response "POST to GET-only location" "$webserv_status" "$nginx_status" "$webserv_content" "$nginx_content"
}

# Test 3: 403 - Forbidden (Directory Traversal)
test_403() {
    echo ""
    echo -e "${YELLOW}=========================================="
    echo "TEST 3: Forbidden Access (403)"
    echo -e "==========================================${NC}"
    
    # Try directory traversal
    webserv_resp=$(curl -s -w "\n%{http_code}" "http://localhost:$WEBSERV_PORT/../etc/passwd")
    webserv_status=$(echo "$webserv_resp" | tail -1)
    webserv_content=$(echo "$webserv_resp" | head -n -1)
    
    nginx_resp=$(curl -s -w "\n%{http_code}" "http://localhost:$NGINX_PORT/../etc/passwd")
    nginx_status=$(echo "$nginx_resp" | tail -1)
    nginx_content=$(echo "$nginx_resp" | head -n -1)
    
    compare_response "Directory traversal attempt" "$webserv_status" "$nginx_status" "$webserv_content" "$nginx_content"
}

# Test 4: 400 - Bad Request
test_400() {
    echo ""
    echo -e "${YELLOW}=========================================="
    echo "TEST 4: Bad Request (400)"
    echo -e "==========================================${NC}"
    
    # Empty POST body
    webserv_resp=$(curl -s -X POST -w "\n%{http_code}" http://localhost:$WEBSERV_PORT/upload)
    webserv_status=$(echo "$webserv_resp" | tail -1)
    webserv_content=$(echo "$webserv_resp" | head -n -1)
    
    echo -e "${BLUE}[Test] Empty POST body${NC}"
    echo "  Webserv: HTTP $webserv_status"
    
    if echo "$webserv_content" | grep -q "Custom error page from webserv"; then
        echo -e "${GREEN}  ✓ Custom error page used${NC}"
    fi
}

# Test 5: 501 - Not Implemented
test_501() {
    echo ""
    echo -e "${YELLOW}=========================================="
    echo "TEST 5: Not Implemented (501)"
    echo -e "==========================================${NC}"
    
    # Unsupported method
    webserv_resp=$(curl -s -X PATCH -w "\n%{http_code}" http://localhost:$WEBSERV_PORT/)
    webserv_status=$(echo "$webserv_resp" | tail -1)
    webserv_content=$(echo "$webserv_resp" | head -n -1)
    
    nginx_resp=$(curl -s -X PATCH -w "\n%{http_code}" http://localhost:$NGINX_PORT/)
    nginx_status=$(echo "$nginx_resp" | tail -1)
    nginx_content=$(echo "$nginx_resp" | head -n -1)
    
    compare_response "Unsupported method (PATCH)" "$webserv_status" "$nginx_status" "$webserv_content" "$nginx_content"
}

# Test 6: Custom Error Pages
test_custom_pages() {
    echo ""
    echo -e "${YELLOW}=========================================="
    echo "TEST 6: Verify Custom Error Pages"
    echo -e "==========================================${NC}"
    
    # Test each error page directly
    for code in 400 403 404 405 500 501 505; do
        webserv_resp=$(curl -s http://localhost:$WEBSERV_PORT/${code}.html)
        
        if echo "$webserv_resp" | grep -q "Custom error page from webserv"; then
            echo -e "${GREEN}  ✓ ${code}.html exists and is valid${NC}"
        else
            echo -e "${RED}  ✗ ${code}.html missing or invalid${NC}"
        fi
    done
}

# Main execution
echo "Checking if servers are running..."
echo ""

if ! check_server $WEBSERV_PORT "Webserv"; then
    echo ""
    echo "Please start webserv first: ./webserv config/test.conf"
    exit 1
fi

echo ""
echo -e "${BLUE}Note: Nginx tests are optional for comparison${NC}"
echo ""

# Run all tests
test_404
test_405
test_403
test_400
test_501
test_custom_pages

echo ""
echo "=========================================="
echo -e "${GREEN}Tests completed!${NC}"
echo "=========================================="
