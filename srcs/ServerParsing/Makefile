SRCS = ./*.cpp ../../gnl/*.cpp ../../base_srcs/*.cpp -fsanitize=address

all :
	clang++ $(SRCS)

clean :
	rm -rf ./a.out
	
fclean : clean

re : clean all

