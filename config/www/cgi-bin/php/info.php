#!/usr/bin/php
<?php
header("Content-Type: text/html");

echo "<html>\n";
echo "<head>\n";
echo "  <title>PHP CGI Test - httpain</title>\n";
echo "  <style>\n";
echo "    body { font-family: Arial, sans-serif; margin: 40px; background: #f4f4f4; }\n";
echo "    .container { background: white; padding: 30px; border-radius: 8px; box-shadow: 0 2px 10px rgba(0,0,0,0.1); }\n";
echo "    h1 { color: #333; border-bottom: 2px solid #007acc; }\n";
echo "    .info { background: #e7f3ff; padding: 15px; margin: 10px 0; border-radius: 5px; }\n";
echo "    .env-var { background: #f0f0f0; padding: 8px; margin: 5px 0; border-left: 4px solid #007acc; }\n";
echo "    .success { color: #28a745; font-weight: bold; }\n";
echo "  </style>\n";
echo "</head>\n";
echo "<body>\n";
echo "  <div class='container'>\n";
echo "    <h1>🐘 PHP CGI Test</h1>\n";
echo "    <div class='info success'>✅ PHP CGI is working correctly!</div>\n";

echo "    <div class='info'>\n";
echo "      <strong>PHP Version:</strong> " . phpversion() . "<br>\n";
echo "      <strong>Current Time:</strong> " . date('Y-m-d H:i:s') . "<br>\n";
echo "      <strong>Server:</strong> httpain Web Server<br>\n";
echo "      <strong>Script:</strong> " . basename(__FILE__) . "\n";
echo "    </div>\n";

echo "    <h2>🌍 Environment Variables</h2>\n";

$important_vars = [
    'REQUEST_METHOD',
    'QUERY_STRING', 
    'SCRIPT_NAME',
    'SERVER_NAME',
    'SERVER_PORT',
    'SERVER_PROTOCOL',
    'GATEWAY_INTERFACE',
    'HTTP_USER_AGENT',
    'HTTP_HOST'
];

foreach ($important_vars as $var) {
    $value = getenv($var);
    if ($value !== false) {
        echo "    <div class='env-var'><strong>$var:</strong> " . htmlspecialchars($value) . "</div>\n";
    }
}

// Show query parameters if any
if (!empty($_GET)) {
    echo "    <h2>📋 Query Parameters</h2>\n";
    foreach ($_GET as $key => $value) {
        echo "    <div class='env-var'><strong>" . htmlspecialchars($key) . ":</strong> " . htmlspecialchars($value) . "</div>\n";
    }
}

echo "    <div style='margin-top: 30px; padding: 15px; background: #d1ecf1; border-radius: 5px;'>\n";
echo "      <strong>💡 Test URLs:</strong><br>\n";
echo "      • <a href='/cgi-bin/php/info.php'>Basic Info</a><br>\n";
echo "      • <a href='/cgi-bin/php/info.php?name=httpain&version=1.0'>With Parameters</a><br>\n";
echo "      • <a href='/cgi-bin/py/hello.py'>Python Script</a><br>\n";
echo "      • <a href='/cgi-bin/py/form.py'>Form Example</a>\n";
echo "    </div>\n";

echo "  </div>\n";
echo "</body>\n";
echo "</html>\n";
?>