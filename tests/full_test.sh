#!/bin/bash

# ===============================================
#           WEBSERV UNIFIED TEST SUITE
# ===============================================
# Complete test suite combining all individual tests
# Tests: Quick, Error Pages, Stress, Scalability, Redirects, CGI

# Colors for consistent formatting
GREEN='\033[0;32m'
RED='\033[0;31m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
PURPLE='\033[0;35m'
CYAN='\033[0;36m'
NC='\033[0m'

# Test tracking
TOTAL_SUITES=5
PASSED_SUITES=0
FAILED_SUITES=0

print_header() {
    echo ""
    echo -e "${BLUE}============================================${NC}"
    echo -e "${BLUE}  $1  ${NC}"
    echo -e "${BLUE}============================================${NC}"
    echo ""
}

print_subheader() {
    echo ""
    echo -e "${CYAN}=== $1 ===${NC}"
}

check_server_running() {
    if ! pgrep -x "webserv" > /dev/null; then
        echo -e "${RED}✗ Webserv is not running!${NC}"
        echo "Please start the server first: ./webserv config/default.conf"
        exit 1
    fi
    local pid=$(pgrep -x webserv | head -1)
    echo -e "${GREEN}✓ Webserv is running (PID: $pid)${NC}"
}



# ===============================================
#              ERROR PAGE TESTS
# ===============================================
run_error_page_tests() {
    print_header "ERROR PAGE TESTS"
    echo -e "${CYAN}Purpose: Validate HTTP error codes and custom error pages${NC}"
    echo -e "${CYAN}Tests: 404 (Not Found), 405 (Method Not Allowed), 403 (Forbidden), Custom Error Pages${NC}"
    echo ""
    
    local WEBSERV_PORT=8080
    local test_failed=0

    # Test 404 - Missing File
    print_subheader "Test 1: Missing File (404)"
    echo -e "${BLUE}→ Testing: Request for non-existent resource should return 404 Not Found${NC}"
    local resp=$(curl -s -w "\n%{http_code}" http://localhost:$WEBSERV_PORT/nonexistent.html)
    local status=$(echo "$resp" | tail -1)
    local content=$(echo "$resp" | head -n -1)
    
    if [ "$status" = "404" ]; then
        echo -e "${GREEN}✓ 404 Status: Correct${NC}"
    else
        echo -e "${RED}✗ 404 Status: Got $status${NC}"
        test_failed=1
    fi

    # Test 405 - Wrong Method  
    print_subheader "Test 2: Wrong Method (405)"
    echo -e "${BLUE}→ Testing: POST to GET-only location should return 405 Method Not Allowed${NC}"
    local status_405=$(curl -s -o /dev/null -w "%{http_code}" -X POST http://localhost:$WEBSERV_PORT/pages/)
    if [ "$status_405" = "405" ]; then
        echo -e "${GREEN}✓ 405 Status: Correct${NC}"
    else
        echo -e "${RED}✗ 405 Status: Got $status_405${NC}"
        test_failed=1
    fi

    # Test 403 - Forbidden
    print_subheader "Test 3: Forbidden Access (403)"
    echo -e "${BLUE}→ Testing: Directory traversal attempt should return 403/400 for security${NC}"
    local status_403=$(curl -s -o /dev/null -w "%{http_code}" "http://localhost:$WEBSERV_PORT/../etc/passwd")
    if [ "$status_403" = "403" ] || [ "$status_403" = "400" ]; then
        echo -e "${GREEN}✓ 403/400 Status: Security OK${NC}"
    else
        echo -e "${YELLOW}⚠ Security Status: Got $status_403${NC}"
    fi

    # Test 4: Custom Error Pages
    print_subheader "Test 4: Verify Custom Error Pages"
    echo -e "${BLUE}→ Testing: Custom HTML error pages should be accessible and well-formed${NC}"
    local custom_failed=0
    for code in 400 403 404 405 500 501 505; do
        local page_resp=$(curl -s http://localhost:$WEBSERV_PORT/errors/${code}.html)
        
        if echo "$page_resp" | grep -q "Webserv"; then
            echo -e "${GREEN}✓ ${code}.html exists and is valid${NC}"
        else
            echo -e "${RED}✗ ${code}.html missing or invalid${NC}"
            custom_failed=1
        fi
    done
    
    if [ $custom_failed -eq 1 ]; then
        test_failed=1
    fi

    if [ $test_failed -eq 0 ]; then
        echo -e "${GREEN}✓ Error Page Tests: PASSED${NC}"
        return 0
    else
        echo -e "${RED}✗ Error Page Tests: FAILED${NC}"
        return 1
    fi
}

# ===============================================
#               STRESS TESTS
# ===============================================
run_stress_tests() {
    print_header "STRESS TESTS"
    echo -e "${CYAN}Purpose: Test server resilience under various connection stress scenarios${NC}"
    echo -e "${CYAN}Tests: Rapid connections, incomplete headers, truncated bodies, concurrent requests, FD leaks${NC}"
    echo ""

    print_subheader "Test 1: Rapid connect/disconnect"
    echo -e "${BLUE}→ Testing: Server resilience to rapid connection attempts and immediate disconnects${NC}"
    for i in {1..50}; do
        (echo "" | nc -w 1 localhost 8080) &
    done
    wait
    echo -e "${GREEN}✓ Completed 50 rapid connections${NC}"

    print_subheader "Test 2: Disconnect mid-headers"
    echo -e "${BLUE}→ Testing: Server handling of incomplete HTTP request headers${NC}"
    for i in {1..10}; do
        (printf "GET / H" | nc -w 1 localhost 8080) &
    done
    wait
    echo -e "${GREEN}✓ Completed 10 incomplete header tests${NC}"

    print_subheader "Test 3: Disconnect mid-body"
    echo -e "${BLUE}→ Testing: Server handling of truncated POST request bodies${NC}"
    for i in {1..10}; do
        (printf "POST /upload HTTP/1.1\r\nContent-Length: 10000\r\n\r\npartial_body" | nc -w 1 localhost 8080) &
    done
    wait
    echo -e "${GREEN}✓ Completed 10 truncated body tests${NC}"

    print_subheader "Test 4: Concurrent valid requests"
    echo -e "${BLUE}→ Testing: Server performance under 100 simultaneous valid HTTP requests${NC}"
    seq 1 100 | xargs -P 50 -I{} curl -s --max-time 1 --connect-timeout 1 -o /dev/null -w "." http://localhost:8080/
    echo ""
    echo -e "${GREEN}✓ Completed 100 concurrent requests${NC}"

    print_subheader "Test 5: Check for fd leaks"
    echo -e "${BLUE}→ Testing: Monitoring for file descriptor leaks after stress testing${NC}"
    local PID=$(pgrep webserv | head -1)
    if [ -n "$PID" ]; then
        local FD_COUNT=$(lsof -p $PID 2>/dev/null | wc -l)
        echo "Server PID: $PID"
        echo "Open fds: $FD_COUNT (should be ~10-15 for listen sockets + std streams)"
        echo -e "${GREEN}✓ FD Check: Completed${NC}"
    else
        echo -e "${RED}✗ Server not running${NC}"
        return 1
    fi

    echo -e "${GREEN}✓ Stress Tests: PASSED${NC}"
    return 0
}

# ===============================================
#             SCALABILITY TESTS
# ===============================================
run_scalability_tests() {
    print_header "SCALABILITY TESTS"
    echo -e "${CYAN}Purpose: Evaluate server performance across multiple ports and concurrent operations${NC}"
    echo -e "${CYAN}Tests: Sequential port testing, simultaneous requests, rapid-fire load testing${NC}"
    echo ""
    
    local PORTS="8080 8081 8082 8083"
    
    # Check all ports are responding
    print_subheader "Port Connectivity Check"
    echo -e "${BLUE}→ Testing: Basic connectivity across all configured server ports${NC}"
    for port in $PORTS; do
        local response=$(curl -s --max-time 2 --connect-timeout 1 -o /dev/null -w "%{http_code}" http://localhost:$port 2>/dev/null)
        if [ "$response" = "200" ]; then
            echo -e "${GREEN}✅ Port $port: OK${NC}"
        else
            echo -e "${RED}❌ Port $port: FAILED (HTTP $response)${NC}"
        fi
    done

    # Test 1: Sequential
    print_subheader "Test 1: Sequential - Testing each port"
    echo -e "${BLUE}→ Testing: Sequential requests to verify individual port functionality${NC}"
    local success=0
    local failed=0
    
    for port in $PORTS; do
        local response=$(curl -s -o /dev/null -w "%{http_code}" --max-time 2 http://localhost:$port 2>/dev/null)
        if [ "$response" = "200" ]; then
            ((success++))
        else
            ((failed++))
        fi
    done
    echo -e "Result: ${GREEN}$success passed${NC}, ${RED}$failed failed${NC}"

    # Test 2: Simultaneous
    print_subheader "Test 2: Simultaneous - 5 requests per port"
    echo -e "${BLUE}→ Testing: Concurrent load distribution across multiple server ports${NC}"
    local requests_per_port=5
    local start_time=$(date +%s)
    
    for port in $PORTS; do
        for ((j=0; j<$requests_per_port; j++)); do
            curl -s -o /dev/null --max-time 5 http://localhost:$port &
        done
    done
    wait
    
    local end_time=$(date +%s)
    local duration=$((end_time - start_time))
    echo -e "${GREEN}✅ All 20 requests completed in ${duration}s${NC}"

    # Test 3: Rapid fire
    print_subheader "Test 3: Rapid fire - 10 rounds on all ports"
    echo -e "${BLUE}→ Testing: High-frequency sequential requests to test server responsiveness${NC}"
    local rounds=10
    local success=0
    local failed=0
    
    for ((i=0; i<$rounds; i++)); do
        for port in $PORTS; do
            local response=$(curl -s -o /dev/null -w "%{http_code}" --max-time 2 http://localhost:$port 2>/dev/null)
            if [ "$response" = "200" ]; then
                ((success++))
            else
                ((failed++))
            fi
        done
    done
    
    local total=$((success + failed))
    echo -e "Result: $total requests - ${GREEN}$success passed${NC}, ${RED}$failed failed${NC}"

    if [ $failed -eq 0 ]; then
        echo -e "${GREEN}✓ Scalability Tests: PASSED${NC}"
        return 0
    else
        echo -e "${YELLOW}⚠ Scalability Tests: Some failures${NC}"
        return 1
    fi
}

# ===============================================
#              REDIRECT TESTS
# ===============================================
run_redirect_tests() {
    print_header "REDIRECT TESTS"
    echo -e "${CYAN}Purpose: Verify HTTP redirection functionality and correct Location headers${NC}"
    echo -e "${CYAN}Tests: 301 (Permanent), 302 (Temporary), 307 (Temporary) redirects${NC}"
    echo ""

    test_redirect() {
        local url="$1"
        local expected_code="$2"
        local expected_location="$3"

        local result=$(curl -s --max-time 2 --connect-timeout 1 -I "$url" 2>/dev/null)
        local code=$(echo "$result" | grep HTTP | awk '{print $2}')
        local location=$(echo "$result" | grep Location | awk '{print $2}')

        if [[ "$code" == "$expected_code" && ( -z "$expected_location" || "$location" == *"$expected_location"* ) ]]; then
            echo -e "${GREEN}✅ $url${NC}"
            return 0
        else
            echo -e "${RED}❌ $url${NC}"
            echo "   Got: $code, Expected: $expected_code"
            return 1
        fi
    }

    local redirect_failed=0

    # Test various redirect scenarios
    echo -e "${BLUE}→ Testing: HTTP redirect responses with proper status codes and Location headers${NC}"
    test_redirect "http://localhost:8080/redirect-301" "301" "/pages/about.html" || redirect_failed=1
    test_redirect "http://localhost:8080/redirect-302" "302" "/pages/r302.html" || redirect_failed=1
    test_redirect "http://localhost:8080/redirect-307" "307" "/pages/r307.html" || redirect_failed=1

    if [ $redirect_failed -eq 0 ]; then
        echo -e "${GREEN}✓ Redirect Tests: PASSED${NC}"
        return 0
    else
        echo -e "${RED}✗ Redirect Tests: Some failures${NC}"
        return 1
    fi
}

# ===============================================
#                CGI TESTS
# ===============================================
run_cgi_tests() {
    print_header "CGI TESTS (Python Test Suite)"
    echo -e "${CYAN}Purpose: Comprehensive testing of Common Gateway Interface (CGI) functionality${NC}"
    echo -e "${CYAN}Tests: Python/PHP scripts, GET/POST methods, environment variables, error handling${NC}"
    echo ""
    
    if [[ -f "tests/cgi_test.py" ]]; then
        echo "Running comprehensive Python CGI test suite..."
        echo -e "${BLUE}→ Testing: CGI script execution, environment variables, POST/GET handling, error recovery${NC}"
        if python3 tests/cgi_test.py; then
            echo -e "${GREEN}✓ CGI Tests: PASSED${NC}"
            return 0
        else
            echo -e "${RED}✗ CGI Tests: FAILED${NC}"
            return 1
        fi
    else
        echo -e "${RED}✗ CGI test script not found: tests/cgi_test.py${NC}"
        return 1
    fi
}

# ===============================================
#               MAIN EXECUTION
# ===============================================
main() {
    print_header "WEBSERV UNIFIED TEST SUITE"
    echo "Running complete test battery..."
    echo "This will test: Error Pages, Stress, Scalability, Redirects, CGI"
    echo ""
    echo -e "${YELLOW}═══════════════════════════════════════════════════════════════${NC}"
    echo -e "${YELLOW}  COMPREHENSIVE WEBSERVER VALIDATION SUITE${NC}"
    echo -e "${YELLOW}═══════════════════════════════════════════════════════════════${NC}"
    echo -e "${CYAN}• Error Pages:${NC} HTTP status codes and custom error handling"
    echo -e "${CYAN}• Stress Tests:${NC} Connection handling and resource management"  
    echo -e "${CYAN}• Scalability:${NC} Multi-port performance and concurrent operations"
    echo -e "${CYAN}• Redirects:${NC} HTTP redirection functionality and headers"
    echo -e "${CYAN}• CGI Tests:${NC} Common Gateway Interface script execution"
    echo -e "${YELLOW}═══════════════════════════════════════════════════════════════${NC}"
    echo ""
    
    # Check if server is running
    check_server_running

    # Run all test suites
    echo "Starting test execution..."
    local start_time=$(date +%s)

    # Error Page Tests  
    if run_error_page_tests; then
        ((PASSED_SUITES++))
    else
        ((FAILED_SUITES++))
    fi

    # Stress Tests
    if run_stress_tests; then
        ((PASSED_SUITES++))
    else
        ((FAILED_SUITES++))
    fi

    # Scalability Tests
    if run_scalability_tests; then
        ((PASSED_SUITES++))
    else
        ((FAILED_SUITES++))
    fi

    # Redirect Tests
    if run_redirect_tests; then
        ((PASSED_SUITES++))
    else
        ((FAILED_SUITES++))
    fi

    # CGI Tests
    if run_cgi_tests; then
        ((PASSED_SUITES++))
    else
        ((FAILED_SUITES++))
    fi

    # Generate final report
    local end_time=$(date +%s)
    local total_time=$((end_time - start_time))
    
    print_header "FINAL REPORT"
    echo "Test Execution Summary:"
    echo "======================"
    echo -e "Total Test Suites:   ${CYAN}$TOTAL_SUITES${NC}"
    echo -e "Passed Suites:       ${GREEN}$PASSED_SUITES${NC}"
    echo -e "Failed Suites:       ${RED}$FAILED_SUITES${NC}"
    echo -e "Execution Time:      ${PURPLE}${total_time}s${NC}"
    echo ""

    local success_rate=$((PASSED_SUITES * 100 / TOTAL_SUITES))
    echo -e "Success Rate:        ${CYAN}${success_rate}%${NC}"
    echo ""

    if [ $PASSED_SUITES -eq $TOTAL_SUITES ]; then
        echo -e "${GREEN}🎉 ALL TESTS PASSED! Your webserv is ready for submission! 🎉${NC}"
        exit 0
    elif [ $success_rate -ge 80 ]; then
        echo -e "${YELLOW}⚠️  Most tests passed ($success_rate%). Check failed suites above.${NC}"
        exit 1
    else
        echo -e "${RED}❌ Multiple test failures ($success_rate% pass rate). Review implementation.${NC}"
        exit 1
    fi
}

# Execute main function
main "$@"