#include "Webserver/Webserver.hpp"
#include "ServerParsing/ServerConfigIdx.hpp"
// #include "Server"
#include "Server/Server.hpp"
/*******************************************
내부에 try catch 문을 최대한 제거하기 위한 main 코드
********************************************/
int		main(int argc, char** argv, char** env)
{
	try {
		Webserver	s(argc, argv, env);
		Server server;
		server.ReadConfigFile("srcs/ServerParsing/webserv.config");
		server.SetGnl(server.config_fd);
		SetServer(server, server.gnl);
		server.ShowServerConfigs();
		s.start_server();
	}
	catch(const std::exception &e) {
		std::cerr << e.what() << std::endl;
	}
}
