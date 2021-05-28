#include <iostream>
#include "ServerManager.hpp"

// #define DEFAULT_CONFIG_FILE_PATH "default.config"

std::map<int, std::string> Response::m_status_map; // NOTE static 변수도 전역변수라도 한번 선언을 해줘야함.

char** g_env;

int		main(int argc, char* argv[], char* envp[])
{
	g_env = envp; // FIXME env 임시로 전역변수로

	Response::init_status_map();
	ServerManager	manager;

	if (argc > 2)
	{
		std::cerr << "There are many arguments." << std::endl;
		return (1);
	}
	else
	{
		try
		{
			// NOTE configuration file 경로 설정
			std::string	configurationFilePath(DEFAULT_CONFIG_FILE_PATH);
			if (argc == 2)
			{
				configurationFilePath = argv[1];
			}
			// NOTE 서버 생성
			manager.CreateServers(configurationFilePath.c_str(), envp);
			
			// NOTE 서버 실행
			manager.RunServers();
		}
		catch (const std::exception& e)
		{
			std::cerr << "Exception occurred. Because of " << e.what() << std::endl;
			return (1);
		}
		return (0);
	}
}