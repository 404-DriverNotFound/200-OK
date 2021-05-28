SRCS	=	gnl/get_next_line_bonus.cpp gnl/get_next_line_utils_bonus.cpp \
			srcs/main.cpp\
			srcs/ServerManager.cpp\
			srcs/Server.cpp\
			srcs/Server_parsing.cpp\
			srcs/Server_run.cpp\
			srcs/Server_execute*.cpp\
			srcs/Connection.cpp \
			srcs/Response.cpp \
			srcs/Connection.cpp\
			srcs/Request.cpp\
			srcs/Response.cpp \
			\
			srcs/ConfigFile/ConfigFile.cpp \
			srcs/ConfigFile/ConfigFiles.cpp \
			srcs/Path/Path.cpp \
			srcs/ServerConfigIdx/ServerConfigIdx.cpp \
			srcs/Utils/utils.cpp
#			srcs/Config.cpp\


# $(wildcard ./srcs/*.cpp)



OBJS	=	$(SRCS:.cpp=.o)

NAME	=	webserv

CC		=	clang++

# CFLAGS	=	-Wall -Wextra -Werror -std=c++98 -g3 -fsanitize=address
# CFLAGS	=	-Wall -Wextra -Werror -std=c++98 -g3
# CFLAGS	=	-std=c++98 -g3 -fsanitize=address
CFLAGS	=	-std=c++98 -Wall -Wextra


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
# clang++ -g gnl/get_next_line_bonus.cpp gnl/get_next_line_utils_bonus.cpp srcs/main.cpp  srcs/ServerManager.cpp  srcs/Server.cpp  srcs/Config.cpp  srcs/Location.cpp   srcs/Connection.cpp     srcs/ConfigFile/ConfigFile.cpp   srcs/ConfigFile/ConfigFiles.cpp   srcs/Path/Path.cpp   srcs/ServerConfigIdx/ServerConfigIdx.cpp srcs/Utils/utils.cpp srcs/Response.cpp srcs/Request.cpp srcs/Server_parsing.cpp srcs/Server_run.cpp srcs/Server_execute*.cpp
