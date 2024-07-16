NAME	=	ircserv
CC		=	c++
CFLAGS	=	-Wall -Wextra -Werror -std=c++98

EXEC	=	main	\
			Server	\
			Client	\
			Channel

SRC_DIR	=	./srcs/
SRC_SUF	=	.cpp
SRCS	=	$(addprefix $(SRC_DIR), $(addsuffix $(SRC_SUF), $(EXEC)))

OBJ_DIR	=	./objs/
OBJS	=	$(addprefix $(OBJ_DIR), $(notdir $(SRCS:.cpp=.o)))

all:		$(NAME) 42

$(NAME):	$(OBJS)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJS)

$(OBJ_DIR)%.o: $(SRC_DIR)%.cpp
	mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean: IRC42AFD
	rm -rf $(OBJS)
	rm -rf ./objs

fclean: clean IRC42AFD
	rm -rf $(NAME)

re: fclean all

42: IRC42
	
	@printf "\n\033[0;33m   Loading Server Properties... \033[0m\n  "
	
	@for i in {1..50}; do \
        printf "█"; \
        sleep 0.01; \
    done
	@echo "\033[1;92m IRC server ready ✅\n\n   \033[3;92m./$(NAME) <port> <password>"
	@echo "\n"

IRC42:
	
	@echo "\n"
	@echo "              ,---------------------------,            "
	@echo "              |  /---------------------\  |            "
	@echo "              | |                       | |            "
	@echo "              | |     Internet          | |            "
	@echo "              | |    	 Relay          | |            "
	@echo "              | |           Chat        | |            "
	@echo "              | |                       | |            "
	@echo "              |  \_____________________/  |            "
	@echo "              |___________________________|            "
	@echo "            ,---\_____     []     _______/------,      "
	@echo "          /         /______________\           /|      "
	@echo "        /___________________________________ /  |      "
	@echo "        |                                   |   |      "
	@echo "        |  _ _ _                 [-------]  |   |      "
	@echo "        |  o o o                 [-------]  |  /       "
	@echo "        |__________________________________ |/         "

IRC42AFD:

	@echo "\n"
	@echo "              ,---------------------------,            "
	@echo "              |  /---------------------\  |            "
	@echo "              | |                       | |            "
	@echo "              | |       ALL             | |            "
	@echo "              | |        FILES          | |            "
	@echo "              | |          DELETED      | |            "
	@echo "              | |                       | |            "
	@echo "              |  \_____________________/  |            "
	@echo "              |___________________________|            "
	@echo "            ,---\_____     []     _______/------,      "
	@echo "          /         /______________\           /|      "
	@echo "        /___________________________________ /  |      "
	@echo "        |                                   |   |      "
	@echo "        |  _ _ _                 [-------]  |   |      "
	@echo "        |  o o o                 [-------]  |  /       "
	@echo "        |__________________________________ |/         "
	@echo "\n"
.PHONY: all clean fclean re
