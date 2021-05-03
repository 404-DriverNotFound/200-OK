#include "Webserver/Webserver.hpp"
// #include "ServerParsing/ServerConfigIdx.hpp"
// #include "Server"
// #include "Server/Server.hpp"
#include "Server/Servers.hpp"

// #include "Config/Config.hpp"
// #include "Config/Configs.hpp"
/*******************************************
내부에 try catch 문을 최대한 제거하기 위한 main 코드
********************************************/
int		main(int argc, char** argv, char** env)
{
	try {
		Webserver	s(argc, argv, env);
		Configs configs("./webserv.config");
		// configss.ShowConfigs();
		Servers servers;
		if (-1 == servers.SetServers(&configs)) // NOTE Configs -> Servers로 파싱하고 옮김
		{
			std::cout << "error" << std::endl;
			return (-1);
		}
		// NOTE Servers의 socket에 대한 정보를 세팅하는 것이 좋을 것 같음.
		servers.ShowServers(); // NOTE Servers에 담긴 값 확인


		s.mservers = servers; // REVIEW public으로 값을 할당하는데, soft 할지 deep할지
		s.start_server();
	}
	catch(const std::exception &e) {
		std::cerr << e.what() << std::endl;
	}
}
