CXX=c++
CPPFLAGS=-g -Wall -Werror -Wextra
SRC=main.cpp \
	src/Server.cpp \
	src/Client.cpp \
	src/Channel.cpp
OBJ=$(SRC:.cpp=.o)
NAME=ft_irc

all: $(NAME)

$(NAME): $(OBJ)
	$(CXX) $(CPPFLAGS) $(OBJ) -o $(NAME)

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re