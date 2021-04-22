
SRCS	=	$(wildcard ./srcs/*.cpp)

OBJS	=	$(SRCS:.cpp=.o)

NAME	=	Webserv

CC		=	clang++

# CFLAG	=	-Wall -Wextra -Werror

RM		=	-rm -rf

all		:	$(NAME)

clean	:
			$(RM) *.o
			$(RM) $(OBJS)

fclean	:	clean
			$(RM) $(NAME)

re		:	fclean all

$(NAME)	:	$(OBJS)
			$(CC) -c $(SRCS)
			$(CC) -o $(NAME) $(OBJS)

# $(CC) $(CFLAG) -c $(SRCS)
# $(CC) $(CFLAG) -o $(NAME) $(OBJS)

.PHONY	:	all clean fclean re