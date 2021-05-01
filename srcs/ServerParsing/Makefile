SRCS = ./*.cpp ../../gnl/*.cpp ../Utils/*.cpp ../Path/*.cpp ../Server/Server.cpp \
			../Config/*.cpp -fsanitize=address

all :
	clang++ $(SRCS)

clean :
	rm -rf ./a.out
	
fclean : clean

re : clean all

