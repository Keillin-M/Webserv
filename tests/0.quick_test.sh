#!/bin/bash

# ===============================================
#         WEBSERV QUICK TEST SUITE
# ===============================================
# Versão rápida dos testes mais importantes

GREEN='\033[0;32m'
RED='\033[0;31m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
NC='\033[0m'

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}    WEBSERV QUICK TEST SUITE${NC}"  
echo -e "${BLUE}========================================${NC}"
echo ""

# Check if server is running
if ! pgrep -x "webserv" > /dev/null; then
    echo -e "${RED}✗ Webserv not running!${NC}"
    echo "Start with: ./webserv config/default.conf"
    exit 1
fi

echo -e "${GREEN}✓ Server is running${NC}"
echo ""

# Quick tests
echo -e "${BLUE}[1/5] Testing basic HTTP GET${NC}"
if curl -s http://localhost:8080/ | head -5 | grep -q "DOCTYPE"; then
    echo -e "${GREEN}✓ GET: OK${NC}"
else
    echo -e "${RED}✗ GET: FAILED${NC}"
fi

echo -e "${BLUE}[2/5] Testing Python CGI${NC}"
if curl -s http://localhost:8080/cgi-bin/py/hello.py | grep -q "Hello"; then
    echo -e "${GREEN}✓ Python CGI: OK${NC}"
else
    echo -e "${RED}✗ Python CGI: FAILED${NC}"
fi

echo -e "${BLUE}[3/5] Testing PHP CGI${NC}"
if curl -s http://localhost:8080/cgi-bin/php/hello.php | grep -q "PHP"; then
    echo -e "${GREEN}✓ PHP CGI: OK${NC}"
else
    echo -e "${RED}✗ PHP CGI: FAILED${NC}"
fi

echo -e "${BLUE}[4/5] Testing POST with CGI${NC}"
if curl -s -X POST -d "name=test&value=42" http://localhost:8080/cgi-bin/py/form.py | grep -q "POST"; then
    echo -e "${GREEN}✓ POST CGI: OK${NC}"
else
    echo -e "${RED}✗ POST CGI: FAILED${NC}"
fi

echo -e "${BLUE}[5/5] Testing 404 error${NC}"
status=$(curl -s -o /dev/null -w "%{http_code}" http://localhost:8080/nonexistent.html)
if [[ "$status" == "404" ]]; then
    echo -e "${GREEN}✓ 404 Error: OK${NC}"
else
    echo -e "${RED}✗ 404 Error: Got $status${NC}"
fi

echo ""
echo -e "${YELLOW}Quick test completed!${NC}"
echo -e "${BLUE}For comprehensive testing, run: ./full_test_suite.sh${NC}"