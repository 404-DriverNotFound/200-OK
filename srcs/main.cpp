#include "Webserver/Webserver.hpp"

/*******************************************
내부에 try catch 문을 최대한 제거하기 위한 main 코드
********************************************/
int		main(int argc, char** argv, char** env)
{
	try {
		Webserver	s(argc, argv, env);
		s.start_server();
	}
	catch(const std::exception &e) {
		std::cerr << e.what() << std::endl;
	}
}
