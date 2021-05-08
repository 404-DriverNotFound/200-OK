SRCS	=	gnl/get_next_line_bonus.cpp gnl/get_next_line_utils_bonus.cpp \
			srcs/main.cpp\
			srcs/ServerManager.cpp\
			srcs/Server.cpp\
			srcs/Config.cpp\
			srcs/Location.cpp \
			\
			srcs/ConfigFile/ConfigFile.cpp \
			srcs/ConfigFile/ConfigFiles.cpp \
			srcs/Path/Path.cpp \
			srcs/ServerConfigIdx/ServerConfigIdx.cpp \
			srcs/Utils/utils.cpp


# $(wildcard ./srcs/*.cpp)



OBJS	=	$(SRCS:.cpp=.o)

NAME	=	webserv

CC		=	clang++

# CFLAG	=	-Wall -Wextra -Werror -std=c++98 -g3 -fsanitize=address
# CFLAG	=	-Wall -Wextra -Werror -std=c++98 -g3
CFLAG	=	-std=c++98 -v


RM		=	-rm -rf

all		:	$(NAME)

clean	:
			$(RM) $(OBJS)

fclean	:	clean
			$(RM) $(NAME)

re		:	fclean all

$(NAME)		:	$(SRCS)
				$(CC) $(CFLAGS) $^ -o $@
				# ./webserv

.PHONY	:	all clean fclean re



# yunslee가 쓰는 소스코드모음
# clang++ gnl/get_next_line_bonus.cpp gnl/get_next_line_utils_bonus.cpp srcs/main.cpp srcs/ServerManager.cpp srcs/Server.cpp srcs/Config.cpp srcs/Location.cpp   srcs/ConfigFile/ConfigFile.cpp  srcs/ConfigFile/ConfigFiles.cpp  srcs/Path/Path.cpp  srcs/ServerConfigIdx/ServerConfigIdx.cpp srcs/Utils/utils.cpp