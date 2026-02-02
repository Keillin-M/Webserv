*This project has been created as part of the 42 curriculum by kmaeda, gabrsouz, tmarcos*

This repository implements a minimal HTTP server for the 42 School curriculum.

Sections included:
- **Description** presenting the project and goals.
- **Instructions** for compilation and execution.
- **Resources** and AI usage disclosure.
- Additional sections: features, project structure, development notes.

**webserv**

**About**
`webserv` is a small, standards-oriented HTTP server implemented in C++ as part of the 42 School curriculum. The goal is to implement a robust server that correctly parses HTTP requests and serves responses, supporting core features expected in the project evaluation: configuration parsing, multiple virtual hosts, static file serving, CGI execution, proper error handling, and concurrent client handling using non-blocking I/O.

**Key goals**
- Correct HTTP request parsing and response generation according to relevant RFCs.
- Support for common HTTP methods (GET, POST, DELETE) as required by the subject.
- Configuration-driven virtual hosts and locations.
- Static file serving, directory listing (autoindex) optional by config.
- CGI support for dynamic responses.
- Concurrency via `poll`/`select`/`kqueue` or similar non-blocking approach.
- Proper handling of persistent connections and chunked transfer when needed.

**Installation / Build**
Prerequisites: a POSIX system (Linux/macOS), a C++ compiler supporting C++98 and standard build tools.

Build steps:
```bash
# clone repository (if not already)
git clone <repo-url>
cd webserv

# compile with the provided Makefile
make
```

**Run**
```bash
# run server with a config
./webserv config/default.conf

# test with curl
curl -v http://localhost:8080/
```

**Development / Debugging**
- Rebuild: `make re`
- Run under Valgrind (Linux) to check for leaks:
```bash
valgrind --leak-check=full ./webserv config/default.conf
```

**Project Structure (example)**
```
webserv/
├── include/           # headers (e.g., Client.hpp, Server.hpp, Request.hpp, Response.hpp)
├── srcs/              # implementation files
├── config/            # example configuration files
├── Makefile
└── README.md
```

**Features & Architecture**
- Configuration Parser: reads server blocks, listen directives, server_name, locations, root, index, cgi, etc.
- Networking: non-blocking sockets, socket setup, binding, listening, and I/O multiplexing (poll/select/kqueue).
- Request Processing: parsing request line, headers, body; handling chunked bodies when required.
- Response Generation: status codes, headers, content-length or chunked transfer, and error pages.
- Static File Handling: read and send files from disk with proper MIME types.
- CGI Support: spawn CGI processes and pipe input/output to produce dynamic responses.
- Concurrency: handle multiple clients without blocking the main loop.

**Error Handling & Validation**
- Validate configuration files and report clear error messages.
- Provide HTTP-compliant error responses for malformed requests or server errors.

**Resources**
- HTTP/RFC references: RFC 7230–7235 (message syntax and routing)
- POSIX sockets documentation
- Common tutorials for writing HTTP servers in C/C++

**AI Usage Disclosure**
AI assistance was used to draft and format parts of this README (structure and wording). Implementation and code were written and reviewed by the project authors.

**Contributors**
- kmaeda
- gabrsouz
- tmarcos

**License & Academic Notice**
This project is part of the 42 School curriculum and follows the institution's academic guidelines.

Created with ❤️ by 42 students