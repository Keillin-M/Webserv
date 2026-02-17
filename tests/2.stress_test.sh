#!/bin/bash

echo "=== Test 1: Rapid connect/disconnect ==="
for i in {1..50}; do
    (echo "" | nc -w 1 localhost 8080) &
done
sleep 2
echo "Completed 50 rapid connections"

echo ""
echo "=== Test 2: Disconnect mid-headers ==="
for i in {1..10}; do
    (printf "GET / H" | nc -w 1 localhost 8080) &
done
sleep 2
echo "Completed 10 incomplete header tests"

echo ""
echo "=== Test 3: Disconnect mid-body ==="
for i in {1..10}; do
    (printf "POST /upload HTTP/1.1\r\nContent-Length: 10000\r\n\r\npartial_body" | nc -w 1 localhost 8080) &
done
sleep 2
echo "Completed 10 truncated body tests"

echo ""
echo "=== Test 4: Concurrent valid requests ==="
seq 1 100 | xargs -P 50 -I{} curl -s -o /dev/null -w "." http://localhost:8080/
echo ""
echo "Completed 100 concurrent requests"

echo ""
echo "=== Test 5: Check for fd leaks ==="
PID=$(pgrep webserv | head -1)
if [ -n "$PID" ]; then
    FD_COUNT=$(lsof -p $PID 2>/dev/null | wc -l)
    echo "Server PID: $PID"
    echo "Open fds: $FD_COUNT (should be ~10-15 for 3 listen sockets + stdin/stdout/stderr)"
else
    echo "Server not running"
fi
