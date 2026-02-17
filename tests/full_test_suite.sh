#!/bin/bash

# ===============================================
#         WEBSERV FULL TEST SUITE
# ===============================================
# Executa todos os testes disponíveis do webserv
# Testes: CGI, Error Pages, Stress, Scalability, Redirects

# Colors for output
GREEN='\033[0;32m'
RED='\033[0;31m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
PURPLE='\033[0;35m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# Test results tracking
TOTAL_TESTS=0
PASSED_TESTS=0
FAILED_TESTS=0

# Configuration
WEBSERV_CONFIG="config/default.conf"
WEBSERV_PORTS="8080 8081 8082"
LOG_FILE="test_results_$(date +%Y%m%d_%H%M%S).log"

# Get script directory and project root  
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

# Ensure we're in project root
cd "$PROJECT_ROOT"

print_header() {
    echo ""
    echo -e "${BLUE}============================================${NC}"
    echo -e "${BLUE}  $1  ${NC}"
    echo -e "${BLUE}============================================${NC}"
    echo ""
}

print_section() {
    echo ""
    echo -e "${CYAN}→ $1${NC}"
    echo "----------------------------------------"
}

check_requirements() {
    print_section "Checking Requirements"
    
    # Check if webserv is compiled
    if [[ ! -f "./webserv" ]]; then
        echo -e "${RED}✗ webserv executable not found${NC}"
        echo "  Run 'make' first"
        exit 1
    fi
    echo -e "${GREEN}✓ webserv executable found${NC}"
    
    # Check if config exists
    if [[ ! -f "$WEBSERV_CONFIG" ]]; then
        echo -e "${RED}✗ Config file not found: $WEBSERV_CONFIG${NC}"
        exit 1
    fi
    echo -e "${GREEN}✓ Config file found${NC}"
    
    # Check Python for CGI tests
    if ! command -v python3 &> /dev/null; then
        echo -e "${YELLOW}⚠ Python3 not found, CGI tests may fail${NC}"
    else
        echo -e "${GREEN}✓ Python3 available${NC}"
    fi
    
    # Check if requests module is available
    if python3 -c "import requests" 2>/dev/null; then
        echo -e "${GREEN}✓ Python requests module available${NC}"
    else
        echo -e "${YELLOW}⚠ Python requests not found, installing...${NC}"
        pip3 install requests
    fi
}

start_server() {
    print_section "Starting Webserv"
    
    # Kill any existing webserv
    pkill -f webserv 2>/dev/null && sleep 1
    
    # Start webserv in background
    ./webserv $WEBSERV_CONFIG &
    SERVER_PID=$!
    sleep 2
    
    # Check if server started successfully
    if kill -0 $SERVER_PID 2>/dev/null; then
        echo -e "${GREEN}✓ Webserv started (PID: $SERVER_PID)${NC}"
        
        # Test connectivity
        for port in $WEBSERV_PORTS; do
            if curl -s -o /dev/null -w "%{http_code}" --max-time 2 http://localhost:$port > /dev/null 2>&1; then
                echo -e "${GREEN}✓ Port $port responding${NC}"
            else
                echo -e "${YELLOW}⚠ Port $port not responding${NC}"
            fi
        done
    else
        echo -e "${RED}✗ Failed to start webserv${NC}"
        exit 1
    fi
}

run_cgi_tests() {
    print_header "CGI TESTS (Python Script)"
    ((TOTAL_TESTS++))
    
    if [[ -f "tests/5.test_cgi.py" ]]; then
        echo "Running comprehensive CGI test suite..."
        if python3 tests/5.test_cgi.py; then
            echo -e "${GREEN}✓ CGI Tests: PASSED${NC}"
            ((PASSED_TESTS++))
        else
            echo -e "${RED}✗ CGI Tests: FAILED${NC}"
            ((FAILED_TESTS++))
        fi
    else
        echo -e "${YELLOW}⚠ CGI test script not found (tests/5.test_cgi.py)${NC}"
        ((FAILED_TESTS++))
    fi
}

run_error_page_tests() {
    print_header "ERROR PAGE TESTS"
    ((TOTAL_TESTS++))
    
    if [[ -f "tests/1.test_error_pages.sh" ]]; then
        echo "Running error page tests..."
        if timeout 60 ./tests/1.test_error_pages.sh; then
            echo -e "${GREEN}✓ Error Page Tests: PASSED${NC}"
            ((PASSED_TESTS++))
        else
            echo -e "${RED}✗ Error Page Tests: FAILED or TIMEOUT${NC}"
            ((FAILED_TESTS++))
        fi
    else
        echo -e "${YELLOW}⚠ Error page test script not found (tests/1.test_error_pages.sh)${NC}"
        ((FAILED_TESTS++))
    fi
}

run_stress_tests() {
    print_header "STRESS TESTS"
    ((TOTAL_TESTS++))
    
    if [[ -f "tests/2.stress_test.sh" ]]; then
        echo "Running stress tests..."
        if timeout 120 ./tests/2.stress_test.sh; then
            echo -e "${GREEN}✓ Stress Tests: PASSED${NC}"
            ((PASSED_TESTS++))
        else
            echo -e "${RED}✗ Stress Tests: FAILED or TIMEOUT${NC}"
            ((FAILED_TESTS++))
        fi
    else
        echo -e "${YELLOW}⚠ Stress test script not found (tests/2.stress_test.sh)${NC}"
        ((FAILED_TESTS++))
    fi
}

run_scalability_tests() {
    print_header "SCALABILITY TESTS"
    ((TOTAL_TESTS++))
    
    if [[ -f "tests/3.test_scalability.sh" ]]; then
        echo "Running scalability tests..."
        if timeout 90 ./tests/3.test_scalability.sh; then
            echo -e "${GREEN}✓ Scalability Tests: PASSED${NC}"
            ((PASSED_TESTS++))
        else
            echo -e "${RED}✗ Scalability Tests: FAILED or TIMEOUT${NC}"
            ((FAILED_TESTS++))
        fi
    else
        echo -e "${YELLOW}⚠ Scalability test script not found (tests/3.test_scalability.sh)${NC}"
        ((FAILED_TESTS++))
    fi
}

run_redirect_tests() {
    print_header "REDIRECT TESTS"
    ((TOTAL_TESTS++))
    
    if [[ -f "tests/4.test_redirect.sh" ]]; then
        echo "Running redirect tests..."
        if timeout 30 ./tests/4.test_redirect.sh; then
            echo -e "${GREEN}✓ Redirect Tests: PASSED${NC}"
            ((PASSED_TESTS++))
        else
            echo -e "${RED}✗ Redirect Tests: FAILED or TIMEOUT${NC}"
            ((FAILED_TESTS++))
        fi
    else
        echo -e "${YELLOW}⚠ Redirect test script not found (tests/4.test_redirect.sh)${NC}"
        ((FAILED_TESTS++))
    fi
}

run_manual_spot_checks() {
    print_header "MANUAL SPOT CHECKS"
    
    print_section "Basic HTTP Methods"
    echo "Testing basic GET..."
    if curl -s http://localhost:8080/ | head -5 | grep -q "DOCTYPE"; then
        echo -e "${GREEN}✓ GET /: HTML response${NC}"
    else
        echo -e "${RED}✗ GET /: No HTML response${NC}"
    fi
    
    echo "Testing CGI Python..."
    if curl -s http://localhost:8080/cgi-bin/py/hello.py | grep -q "Hello"; then
        echo -e "${GREEN}✓ Python CGI: Working${NC}"
    else
        echo -e "${RED}✗ Python CGI: Failed${NC}"
    fi
    
    echo "Testing CGI PHP..."
    if curl -s http://localhost:8080/cgi-bin/php/hello.php | grep -q "PHP"; then
        echo -e "${GREEN}✓ PHP CGI: Working${NC}"
    else
        echo -e "${RED}✗ PHP CGI: Failed${NC}"
    fi
    
    echo "Testing 404 handling..."
    status=$(curl -s -o /dev/null -w "%{http_code}" http://localhost:8080/nonexistent.html)
    if [[ "$status" == "404" ]]; then
        echo -e "${GREEN}✓ 404 Error: Correct status${NC}"
    else
        echo -e "${RED}✗ 404 Error: Got $status${NC}"
    fi
}

cleanup() {
    print_section "Cleaning Up"
    
    if [[ -n "$SERVER_PID" ]]; then
        echo "Stopping webserv (PID: $SERVER_PID)..."
        kill $SERVER_PID 2>/dev/null
        sleep 1
        pkill -f webserv 2>/dev/null
        echo -e "${GREEN}✓ Server stopped${NC}"
    fi
}

generate_report() {
    print_header "TEST REPORT"
    
    local score_percent=$((PASSED_TESTS * 100 / TOTAL_TESTS))
    
    echo "Test Results Summary:"
    echo "===================="
    echo -e "Total Tests:     ${CYAN}$TOTAL_TESTS${NC}"
    echo -e "Passed:          ${GREEN}$PASSED_TESTS${NC}"
    echo -e "Failed:          ${RED}$FAILED_TESTS${NC}"
    echo -e "Success Rate:    ${PURPLE}${score_percent}%${NC}"
    echo ""
    
    if [[ $PASSED_TESTS == $TOTAL_TESTS ]]; then
        echo -e "${GREEN}🎉 ALL TESTS PASSED! Your webserv is ready for submission! 🎉${NC}"
    elif [[ $score_percent -ge 80 ]]; then
        echo -e "${YELLOW}⚠️  Most tests passed. Check failed tests and minor issues.${NC}"
    else
        echo -e "${RED}❌ Several tests failed. Review your implementation.${NC}"
    fi
    
    echo ""
    echo "Test completed at: $(date)"
    echo "Log saved to: $LOG_FILE"
}

# Trap to cleanup on exit
trap cleanup EXIT INT TERM

# Main execution
main() {
    print_header "WEBSERV FULL TEST SUITE"
    echo "Starting comprehensive testing of your webserv implementation..."
    echo "This may take 5-10 minutes to complete."
    echo ""
    echo "Test log: $LOG_FILE"
    
    # Redirect all output to both terminal and log file
    exec > >(tee -a "$LOG_FILE")
    exec 2>&1
    
    # Run all test phases
    check_requirements
    start_server
    
    # Core test suites
    run_cgi_tests
    run_error_page_tests
    run_redirect_tests
    run_stress_tests
    run_scalability_tests
    
    # Quick manual checks
    run_manual_spot_checks
    
    # Generate final report
    generate_report
}

# Execute if script is run directly
if [[ "${BASH_SOURCE[0]}" == "${0}" ]]; then
    main "$@"
fi