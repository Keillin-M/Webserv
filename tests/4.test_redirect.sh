#!/bin/bash

# Automated redirect tests for webserv
# Each test prints result with clear formatting

GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m'

print_result() {
    local url="$1"
    local code="$2"
    local location="$3"
    local expected_code="$4"
    local expected_location="$5"

    if [[ "$code" == "$expected_code" && "$location" == *"$expected_location"* ]]; then
        echo -e "${GREEN}✅ $url\n   Status: $code\n   Location: $location${NC}"
    else
        echo -e "${RED}❌ $url\n   Got: $code, Location: $location\n   Expected: $expected_code, $expected_location${NC}"
    fi
    echo ""
}

test_redirect() {
    local url="$1"
    local expected_code="$2"
    local expected_location="$3"

    result=$(curl -s -I "$url")
    code=$(echo "$result" | grep HTTP | awk '{print $2}')
    location=$(echo "$result" | grep Location | awk '{print $2}')

    print_result "$url" "$code" "$location" "$expected_code" "$expected_location"
}

echo -e "${YELLOW}=== Webserv Redirect Test Suite ===${NC}\n"

test_redirect "http://localhost:8080/old-blog/" "301" "http://localhost:8080/pages/"
test_redirect "http://localhost:8080/temp-redirect/" "302" "http://localhost:8080/"
test_redirect "http://localhost:8080/secure/" "301" "https://localhost:8443/"
test_redirect "http://localhost:8080/redirect/301" "301" "/pages/about.html"
test_redirect "http://localhost:8080/redirect/302" "302" "/pages/about.html"

echo -e "${YELLOW}=== End of Redirect Tests ===${NC}"
