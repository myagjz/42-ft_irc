NAME = irc_server
CC = c++
CCFLAGS = -Wall -Wextra -std=c++98
INCLUDES = -Iincs
OBJDIR = obj

SRCS = srcs/Channel.cpp srcs/Executor.cpp srcs/main.cpp srcs/Server.cpp srcs/User.cpp \
        srcs/commands/INVITE.cpp srcs/commands/JOIN.cpp \
		srcs/commands/KICK.cpp srcs/commands/MODE.cpp \
		srcs/commands/NICK.cpp srcs/commands/PASS.cpp \
        srcs/commands/PRIVMSG.cpp srcs/commands/QUIT.cpp \
        srcs/commands/TOPIC.cpp srcs/commands/USER.cpp

OBJS = $(patsubst srcs/%.cpp,$(OBJDIR)/%.o,$(patsubst srcs/commands/%.cpp,$(OBJDIR)/commands/%.o,$(SRCS)))

all: $(NAME) 42

$(NAME): $(OBJS)
	@$(CC) $(CCFLAGS) -o $@ $^

$(OBJDIR)/%.o: srcs/%.cpp
	@mkdir -p $(dir $@)
	@$(CC) $(CCFLAGS) $(INCLUDES) -c $< -o $@

$(OBJDIR)/commands/%.o: srcs/commands/%.cpp
	@mkdir -p $(dir $@)
	@$(CC) $(CCFLAGS) $(INCLUDES) -c $< -o $@

clean: IRC42AFD
	@rm -rf $(OBJS) $(OBJDIR)
	@rm -rf bot/bot.o

fclean: clean IRC42AFD
	@rm -rf $(NAME)
	@rm -rf bot/bot

re: fclean all

bot:
	@$(MAKE) -C bot

42: IRC42
	@printf "\n\033[0;33m   Loading Server Properties... \033[0m\n  "
	@for i in {1..50}; do \
        printf "█"; \
        sleep 0.01; \
    done
	@echo "\033[1;92m IRC server ready ✅\n\n   \033[0m./$(NAME) <port> <password>"
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

.PHONY: all clean fclean re bot

