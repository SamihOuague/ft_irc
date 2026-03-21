CXX=c++
CXXFLAGS=-g -Wall -Werror -Wextra -I./cmd -I./src
SRC=main.cpp \
	src/Server.cpp \
	src/Client.cpp \
	src/Channel.cpp \
	src/Routes.cpp \
	cmd/join.cpp \
	cmd/invite.cpp \
	cmd/kick.cpp \
	cmd/mode.cpp \
	cmd/topic.cpp \
	cmd/others.cpp \
	cmd/nick.cpp \
	cmd/part.cpp \
	cmd/privmsg.cpp \
	cmd/welcome.cpp \
	cmd/pass.cpp
HEADERS=src/Channel.hpp \
		src/Server.hpp \
		src/Routes.hpp \
		cmd/commands.hpp
OBJ=$(SRC:.cpp=.o)
NAME=ircserv

all: $(NAME)

$(NAME): $(OBJ) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME)

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re