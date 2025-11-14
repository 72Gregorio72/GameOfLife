NAME = gameoflife
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -I./include
LDFLAGS = -lsfml-graphics -lsfml-window -lsfml-system
SRCS = main.cpp revive.cpp game.cpp
INCLUDE = ./include

OBJS = $(SRCS:.cpp=.o)
all: $(NAME)
$(NAME): $(OBJS)
	$(CXX) $(OBJS) -o $(NAME) $(LDFLAGS)
clean:
	rm -f $(OBJS) $(NAME)
fclean: clean
	rm -f $(NAME)
vale:
	valgrind --suppressions=valgrind.supp --leak-check=full ./$(NAME)
re: fclean all
.PHONY: all clean