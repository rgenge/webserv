SOURCES = Socket.cpp Server.cpp main.cpp ServerManager.cpp CgiHandler.cpp Request.cpp\
	Response.cpp Parser.cpp
NAME = webserv
SRCS_PATH = ./srcs
OBJS_PATH = ./objs
SRCS = ${addprefix ${SRCS_PATH}/ ${SOURCES}}
OBJS = ${addprefix ${OBJS_PATH}/, ${notdir ${SOURCES:.cpp=.o}}}

VPATH := ${SRCS_PATH} ${SRCS_PATH}/server ${SRCS_PATH}/socket ${SRCS_PATH}/ServerManager \ 
		${SRCS_PATH}/cgi ${SRCS_PATH}/Parser ${SRCS_PATH}/request ${SRCS_PATH}/response

CC = c++
FLAGS = -Werror -Wall -Wextra -std=c++98 -g
INCLUDE = -I srcs/socket -I srcs/server -I srcs/ServerManager -I srcs/cgi -I srcs/Parser -I . -I srcs/request \
-I srcs/response

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


# TESTS

# ***********************WILDCARD COMPILATION******************* # 
NAME_WILD = Webserv
NAME_ARCHIVE = webserv.a
SOURCES_W = $(wildcard $(SRCS_PATH)/**/*.cpp) $(wildcard $(SRCS_PATH)/*.cpp) \
			$(wildcard $(SRCS_PATH)/**/**/*.cpp)
OBJECTS_W = $(patsubst $(SRCS_PATH)/%.cpp, $(OBJS_PATH)/%.o, $(SOURCES_W))
OBJECTS_W_PATH = ${dir ${OBJECTS_W}}


${NAME_WILD}:
				${CC} ${INCLUDE} ${FLAGS} -o ${NAME_WILD}

${NAME_ARCHIVE}: ${OBJECTS_W}
				ar -rcs ${NAME_ARCHIVE} ${OBJECTS_W}

# ***********************TESTS******************* # 

TEST_PATH = ./tests
SOURCES_T = $(wildcard $(TEST_PATH)/*.cpp) $(wildcard ${TEST_PATH}/**/*.cpp)
TESTS = $(patsubst $(TEST_PATH)/%.cpp, $(TEST_PATH)/%.out, $(SOURCES_T))
VGTESTS = $(patsubst $(TEST_PATH)/%.cpp, $(TEST_PATH)/%vg.out, $(SOURCES_T))

test: test_clean ${TEST_PATH}/$t.out

tests: test_clean ${TESTS}

${TEST_PATH}/%.out: ${TEST_PATH}/%.cpp ${NAME_ARCHIVE}
			${CC} ${FLAGS} $< ${NAME_ARCHIVE} -g ${INCLUDE} -o $@
			-./$@ webserv.conf

${TEST_PATH}/%vg.out: ${TEST_PATH}/%.cpp ${NAME_ARCHIVE}
			${CC} ${FLAGS} $< -g ${NAME_ARCHIVE} ${INCLUDE} -o $@
			-valgrind --leak-check=full --show-leak-kinds=all ./$@ webserv.conf

test_clean: clean dirs
			rm -rf ${TESTS} ${VGTESTS}

vgtest: test_clean ${TEST_PATH}/$tvg.out

vgtests: test_clean ${VGTESTS}

dirs:
	mkdir -p ${OBJECTS_W_PATH} ${OBJECTS_T_PATH}
# ============================================================ #
