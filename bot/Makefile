NAME = bot
CC = c++
CCFLAGS = -Wall -Wextra -std=c++98
INCLUDES = -Iincs

SRCS = bot.cpp
OBJS = $(SRCS:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJS)
	@$(CC) $(CCFLAGS) -o $@ $^

%.o: %.cpp
	@$(CC) $(CCFLAGS) $(INCLUDES) -c $< -o $@

clean:
	@rm -f $(OBJS)

fclean: clean
	@rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
