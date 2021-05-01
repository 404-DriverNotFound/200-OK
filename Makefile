SRCS	=	srcs/main.cpp\
			srcs/Server/Socket.cpp\
			srcs/HttpMessage/HttpMessageRequest.cpp\
			srcs/HttpMessage/HttpMessageResponse.cpp\
			srcs/Webserver/Webserver.cpp\
			srcs/Utils/utils.cpp
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
			$(CC) $(CFLAG) -o $(NAME) $(OBJS)
			./webserv
# $(CC) $(CFLAG) -c $(SRCS)
# $(CC) $(CFLAG) -o $(NAME) $(OBJS)

.PHONY	:	all clean fclean re