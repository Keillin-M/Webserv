NAME = webserv
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98
INCLUDES = -I include

SRC = src/main.cpp \
	src/Client.cpp \
	src/Server.cpp \
	src/Request.cpp \
	src/Response.cpp \
	src/ServerManager.cpp \
	src/CGI.cpp

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

.PHONY: all clean fclean re

