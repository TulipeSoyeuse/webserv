FILES		=	main.cpp Request.cpp Response.cpp Server.cpp config_string.cpp hm_popen.cpp string_utils.cpp\
				bytes_container.cpp
HEADERS		=	$(addprefix $(HEADERS_DIR)/, Request.hpp Response.hpp Server.hpp\
				 includes.hpp config_string.hpp hm_popen.hpp bytes_container.hpp)
HEADERS_DIR =	headers

SRC 		=	$(addprefix src/, $(FILES))
OBJ 		=	$(addprefix $(OBJ_DIR)/, $(FILES:.cpp=.o))
CPPFLAGS	=	-Wall -Werror -Wextra -std=c++98 -g3 -o3
NAME		=	webserv

OBJ_DIR		=	obj

all : $(NAME)

$(OBJ_DIR)/%.o: src/%.cpp | $(OBJ_DIR)
	c++ $(CPPFLAGS) -c $< -o $@ -I$(HEADERS_DIR)

$(NAME): $(OBJ) $(HEADERS)
	c++ $(OBJ) -o $@

$(OBJ_DIR):
	@mkdir obj

clean:
	@rm -rf $(OBJ_DIR)

fclean: clean
	@rm -rf $(NAME)

re: fclean all

.PHONY: all clean fclean re
