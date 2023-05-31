SOURCES = srcs/socket/Socket.cpp main.cpp
NAME = webserv
OBJS = $(SOURCES:.cpp=.o)
CC = c++
FLAGS = -std=c++98 -g
INCLUDE = -I srcs/socket

all: ${NAME}

%.o: %.cpp
		${CC} ${INCLUDE} ${FLAGS} -c $< -o $@

${NAME}: ${OBJS}
		${CC} ${FLAGS} ${OBJS} -o ${NAME}

clean:
		rm -f ${OBJS}

fclean: clean
		rm -f ${NAME}

re: fclean all

teste:
		echo ${OBJ}