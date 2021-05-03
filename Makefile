SRCS	=
# $(wildcard ./srcs/*.cpp)\




OBJS	=	$(SRCS:.cpp=.o)

NAME	=	webserv

CC		=	clang++

# CFLAG	=	-Wall -Wextra -Werror -std=c++98 -g3 -fsanitize=address
CFLAG	=	-Wall -Wextra -Werror -std=c++98 -g3

RM		=	-rm -rf

all		:	$(NAME)

clean	:
			$(RM) *.o
			$(RM) $(OBJS)

fclean	:	clean
			$(RM) $(NAME)

re		:	fclean all

$(NAME)	:	$(OBJS)
# $(CC) -c $(SRCS)
			# $(CC) $(CFLAG) $(OBJS) -o $(NAME)
			$(CC) $(SRCS) -o $(NAME)
			./webserv
# $(CC) $(CFLAG) -o $(NAME) $(OBJS)

.PHONY	:	all clean fclean re


# yunslee 쓰는 복사본
# srcs/main.cpp srcs/Server/Socket.cpp srcs/HttpMessage/HttpMessageRequest.cpp srcs/HttpMessage/HttpMessageResponse.cpp srcs/Webserver/Webserver.cpp srcs/Utils/utils.cpp srcs/ServerParsing/ServerConfigIdx.cpp srcs/Server/Server.cpp srcs/Server/Servers.cpp srcs/Path/Path.cpp srcs/Config/Config.cpp srcs/Config/Configs.cpp gnl/get_next_line_bonus.cpp gnl/get_next_line_utils_bonus.cpp