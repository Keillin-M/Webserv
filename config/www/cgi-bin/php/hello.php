#!/usr/bin/php
<?php
header("Content-Type: text/html; charset=UTF-8");

echo "<!DOCTYPE html>\n";
echo "<html>\n";
echo "<head><title>Hello PHP - httpain</title></head>\n";
echo "<body style='font-family: Arial, sans-serif; padding: 20px;'>\n";
echo "<h1 style='color: #007acc;'>🐘 Hello from PHP!</h1>\n";
echo "<div style='background: #f0f8ff; padding: 15px; border-radius: 5px;'>\n";
echo "<p><strong>Status:</strong> ✅ PHP CGI is working!</p>\n";
echo "<p><strong>PHP Version:</strong> " . phpversion() . "</p>\n";
echo "<p><strong>Current Time:</strong> " . date('Y-m-d H:i:s T') . "</p>\n";
echo "<p><strong>Request Method:</strong> " . ($_SERVER['REQUEST_METHOD'] ?? getenv('REQUEST_METHOD')) . "</p>\n";

if (getenv('QUERY_STRING')) {
    echo "<p><strong>Query String:</strong> " . htmlspecialchars(getenv('QUERY_STRING')) . "</p>\n";
}

echo "</div>\n";
echo "<br>\n";
echo "<p>🔗 <a href='/cgi-bin/py/hello.py'>Try Python version</a> | ";
echo "<a href='/cgi-bin/php/info.php'>Detailed PHP info</a></p>\n";
echo "</body>\n";
echo "</html>\n";
?>