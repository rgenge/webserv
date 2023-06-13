SOURCES = Socket.cpp Server.cpp main.cpp ServerManager.cpp CgiHandler.cpp
NAME = webserv
SRCS_PATH = srcs
OBJS_PATH = objs
SRCS = ${addprefix ${SRCS_PATH}/ ${SOURCES}}
OBJS = ${addprefix ${OBJS_PATH}/, ${notdir ${SOURCES:.cpp=.o}}}

VPATH := ${SRCS_PATH} ${SRCS_PATH}/server ${SRCS_PATH}/socket ${SRCS_PATH}/ServerManager ${SRCS_PATH}/cgi

CC = c++
FLAGS = -Werror -Wall -Wextra -std=c++98 -g
INCLUDE = -I srcs/socket -I srcs/server -I srcs/ServerManager -I srcs/cgi

# Colors
GREEN = \33[1;32m
L_CYAN = \33[1;36m
L_BLUE = \33[1;34m
L_GREEN = \33[1;92m
L_PURPLE = \33[1;35m
NC = \033[0m

all: ${NAME}


${OBJS_PATH}/%.o:	%.cpp
					@ mkdir -p ${OBJS_PATH}
					@ printf "Compiling: $< %20s\r"
					@${CC} ${INCLUDE} ${FLAGS} -c $< -o $@

${NAME}:	${OBJS}
			@ ${CC} ${FLAGS} ${OBJS} -o ${NAME}
			@ echo "\12${GREEN}${NAME} successfully compiled!${NC}"

clean:
		@ rm -rf ${OBJS_PATH}
		@ echo "${L_CYAN}Objects deleted!${NC}"

fclean:	clean
		@ rm -rf ${NAME}
		@ echo "${L_BLUE}${NAME} deleted!${NC}"

re:	fclean all

val:	${NAME}
		@ valgrind --leak-check=full --show-leak-kinds=all ./webserv