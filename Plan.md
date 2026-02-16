# Webserv - Next Steps Plan (2-3 Days)

## Day 1: CGI Core Implementation (6-8 hours)

### Morning: CGI Detection & Config
1. *Add CGI config options* (30 min)
   - Add cgi_extension to LocationConfig (.py, .php, .sh)
   - Add cgi_path for script interpreter location
   
2. *Detect CGI requests* (1 hour)
   - Check file extension in handleGet()
   - Route to CGI handler instead of static file

3. *Set up CGI environment variables* (2 hours)
   - REQUEST_METHOD, QUERY_STRING, CONTENT_TYPE
   - CONTENT_LENGTH, PATH_INFO, SCRIPT_NAME
   - SERVER_NAME, SERVER_PORT, HTTP_* headers

### Afternoon: CGI Execution
4. *Implement fork + exec* (2 hours)
   - fork() to create child process
   - Setup pipes for stdin/stdout
   - execve() to run script
   - Pass environment variables

5. *Capture output* (1-2 hours)
   - Read from pipe in parent process
   - Parse CGI response headers
   - Send to client

6. *Error handling* (1 hour)
   - Timeout for long-running scripts
   - 500 if script fails
   - Clean up zombie processes (waitpid)

## Day 2: CGI Testing & Demo Website (6-8 hours)

### Morning: Test CGI
7. *Create test CGI scripts* (1 hour)
   - hello.py - Simple "Hello World"
   - form.py - Handle POST data
   - env.py - Show environment variables

8. *Test & Debug* (2-3 hours)
   - Test GET with query strings
   - Test POST with form data
   - Fix bugs, handle edge cases

### Afternoon: Enhanced Website
9. *Create upload form page* (1 hour)
   html
   <form action="/upload" method="POST">
   

10. *File browser CGI script* (1-2 hours)
    - List uploaded files
    - Delete files via GET parameter

11. *Polish index.html* (1 hour)
    - Links to all features
    - Show CGI demos
    - Navigation menu

## Day 3: Final Testing & Polish (4-6 hours)

### Morning: Integration
12. *Update configs* (30 min)
    - Add CGI locations to default.conf
    - Test all config combinations

13. *Comprehensive testing* (2 hours)
    - Test all HTTP methods
    - Test all error codes
    - Test CGI scripts
    - Test file operations

### Afternoon: Defense Prep
14. *Create demo script* (1 hour)
    - Show all features working
    - CGI examples
    - Error handling

15. *Documentation* (1 hour)
    - Update README
    - Comment complex code
    - Prepare defense talking points

16. *Final checks* (1-2 hours)
    - Memory leaks (valgrind)
    - Handle SIGPIPE
    - Stress testing
    - Clean compilation warnings

## Priority Checklist

*Critical (Must Have):*
- [ ] CGI execution (fork/exec/pipes)
- [ ] CGI environment variables
- [ ] Handle GET with query string
- [ ] Handle POST to CGI
- [ ] Basic error handling (timeouts, failures)

*Important (Should Have):*
- [ ] Multiple CGI types (.py, .php, .sh)
- [ ] Upload form page
- [ ] File browser
- [ ] Updated configs

*Nice to Have:*
- [ ] Session/cookies
- [ ] Directory listing (autoindex)
- [ ] Chunked transfer encoding

## Quick Wins (Do These First)
1. Simple Python CGI that prints "Hello World"
2. Environment variables working
3. GET with query string → CGI

This gets you to "CGI works" fast, then refine from there

-> pkill -f ./webserv
After recompiling, the running server process still uses the old code in memory, so we must kill it (pkill) and start a new process to load the updated executable

pkill -f webserv
./webserv config/default.conf
