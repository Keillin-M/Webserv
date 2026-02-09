#!/bin/bash

# Colors
GREEN='\033[0;32m'
RED='\033[0;31m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
NC='\033[0m'

echo "========================================"
echo "   WEBSERV SCALABILITY TEST SUITE"
echo "========================================"
echo ""

# Configured ports (edit this to match your main.cpp)
PORTS="8080 8081 8082"

check_server() {
    if ! pgrep -x "webserv" > /dev/null; then
        echo -e "${RED}Error: webserv is not running!${NC}"
        echo "Start the server first: ./webserv"
        exit 1
    fi
    echo -e "${GREEN}Server is running (PID: $(pgrep -x webserv))${NC}"
    echo ""
}

test_port() {
    local port=$1
    local response=$(curl -s -o /dev/null -w "%{http_code}" --max-time 2 http://localhost:$port 2>/dev/null)
    
    if [ "$response" = "200" ]; then
        echo -e "${GREEN}  ✅ Port $port: OK (HTTP $response)${NC}"
        return 0
    else
        echo -e "${RED}  ❌ Port $port: FAILED (HTTP $response)${NC}"
        return 1
    fi
}

# Test 1: Sequential (one by one)
test_sequential() {
    local success=0
    local failed=0
    
    echo -e "${BLUE}[Test 1] Sequential - Testing each port${NC}"
    
    for port in $PORTS; do
        if test_port $port; then
            ((success++))
        else
            ((failed++))
        fi
    done
    
    echo ""
    echo -e "  Result: ${GREEN}$success passed${NC}, ${RED}$failed failed${NC}"
    echo ""
}

# Test 2: Simultaneous (all at once)
test_simultaneous() {
    local requests_per_port=$1
    local total=0
    
    for port in $PORTS; do
        total=$((total + requests_per_port))
    done
    
    echo -e "${BLUE}[Test 2] Simultaneous - $requests_per_port requests per port ($total total)${NC}"
    
    local start_time=$(date +%s)
    
    for port in $PORTS; do
        for ((j=0; j<$requests_per_port; j++)); do
            curl -s -o /dev/null --max-time 5 http://localhost:$port &
        done
    done
    
    wait
    
    local end_time=$(date +%s)
    local duration=$((end_time - start_time))
    
    echo -e "  ${GREEN}✅ All $total requests completed in ${duration}s${NC}"
    echo ""
}

# Test 3: Rapid fire (fast sequential)
test_rapid() {
    local rounds=$1
    local success=0
    local failed=0
    
    echo -e "${BLUE}[Test 3] Rapid fire - $rounds rounds on all ports${NC}"
    
    for ((i=0; i<$rounds; i++)); do
        for port in $PORTS; do
            response=$(curl -s -o /dev/null -w "%{http_code}" --max-time 2 http://localhost:$port 2>/dev/null)
            if [ "$response" = "200" ]; then
                ((success++))
            else
                ((failed++))
            fi
        done
    done
    
    local total=$((success + failed))
    echo -e "  Result: $total requests - ${GREEN}$success passed${NC}, ${RED}$failed failed${NC}"
    echo ""
}

# System info
show_system_info() {
    echo -e "${BLUE}[System Info]${NC}"
    local pid=$(pgrep -x webserv)
    if [ ! -z "$pid" ]; then
        local mem=$(ps -o rss= -p $pid 2>/dev/null | tr -d ' ')
        local cpu=$(ps -o %cpu= -p $pid 2>/dev/null | tr -d ' ')
        echo "  Memory: $((mem / 1024)) MB"
        echo "  CPU: ${cpu}%"
        echo "  FD limit: $(ulimit -n)"
        if [[ "$OSTYPE" == "darwin"* ]]; then
            local fds=$(lsof -p $pid 2>/dev/null | wc -l | tr -d ' ')
            echo "  FDs in use: $fds"
        fi
    fi
    echo ""
}

# Menu
show_menu() {
    echo "========================================"
    echo "Configured ports: $PORTS"
    echo "========================================"
    echo "  1) Quick test (sequential + 5 simultaneous)"
    echo "  2) Standard test (sequential + 20 simultaneous)"
    echo "  3) Stress test (sequential + 50 simultaneous + rapid fire)"
    echo "  4) System info"
    echo "  5) Change ports"
    echo "  0) Exit"
    echo "========================================"
    echo -n "Choice: "
}

# Main
main() {
    check_server
    
    while true; do
        show_menu
        read choice
        echo ""
        
        case $choice in
            1)
                test_sequential
                test_simultaneous 5
                show_system_info
                ;;
            2)
                test_sequential
                test_simultaneous 20
                test_rapid 5
                show_system_info
                ;;
            3)
                echo -e "${YELLOW}Heavy load test...${NC}"
                echo ""
                test_sequential
                test_simultaneous 50
                test_rapid 20
                show_system_info
                ;;
            4)
                show_system_info
                ;;
            5)
                echo -n "Enter ports (space separated, e.g. 8080 9090 7070): "
                read PORTS
                echo -e "Ports updated: $PORTS"
                echo ""
                ;;
            0)
                echo "Bye!"
                exit 0
                ;;
            *)
                echo -e "${RED}Invalid choice${NC}"
                ;;
        esac
        
        echo "Press Enter to continue..."
        read
        clear
    done
}

clear
main
