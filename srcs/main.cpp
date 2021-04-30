#include "Webserver/Webserver.hpp"

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
