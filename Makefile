NAME = webserv
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98
INCLUDES = -I include

SRC = src/main.cpp \
	src/cgi/CGI.cpp \
	src/config/ConfigParser.cpp \
	src/config/LocationConfig.cpp \
	src/config/ServerConfig.cpp \
	src/core/Client.cpp \
	src/core/Server.cpp \
	src/core/run_fts.cpp \
	src/core/ServerManager.cpp \
	src/http/Request.cpp \
	src/http/Response.cpp \
	src/http/handle_client_read.cpp \
	src/http/handle_methods.cpp \
	src/http/handle_CGI.cpp 

OBJ = $(SRC:src/%.cpp=obj/%.o)

GREEN		= \033[0;32m
GRAY		= \033[0;90m
RED			= \033[0;31m
RESET		= \033[0m

all: $(NAME)

$(NAME): $(OBJ)
	@$(CXX) $(CXXFLAGS) $(INCLUDES) $(OBJ) -o $(NAME)
	@echo "$(GREEN)Executable created: $(NAME)$(RESET)"
	@echo "$(GRAY)Usage: $(RESET)./$(NAME) + <config file>"

obj/%.o: src/%.cpp
	@mkdir -p $(dir $@)
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	@$(RM) $(OBJ)
	@$(RM) -r obj
	@echo "$(RED)Cleaned object files.$(RESET)"

fclean: clean
	@$(RM) $(NAME)
	@echo "$(RED)Fully cleaned everything.$(RESET)"

re: fclean all

test:
	@echo "$(GREEN)Running full test suite...$(RESET)"
	@echo "$(GRAY)Starting webserv server...$(RESET)"
	@./webserv config/default.conf & echo $$! > .webserv.pid
	@sleep 3
	@echo "$(GRAY)Server started, running tests...$(RESET)"
	@./tests/full_test.sh || true
	@echo "$(GRAY)Tests completed, stopping server...$(RESET)"
	@if [ -f .webserv.pid ]; then \
		kill `cat .webserv.pid` 2>/dev/null || true; \
		rm -f .webserv.pid; \
	fi