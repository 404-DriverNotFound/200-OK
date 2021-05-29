#include <iostream>
#include "ServerManager.hpp"

// #define DEFAULT_CONFIG_FILE_PATH "tester.config"

std::map<int, std::string> Response::m_status_map; // NOTE static 변수도 전역변수라도 한번 선언을 해줘야함.
char**	g_envp;


int		main(int argc, char* argv[], char* envp[])
{
	Response::init_status_map();
	g_envp = envp;

	if (argc > 2)
	{
		std::cerr << REDB "[" << ft::getCurrentTime() << "][error]" << "[There are many arguments.]" << NC << std::endl;
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
			ServerManager	manager;

			// NOTE 서버 생성
			manager.CreateServers(configurationFilePath.c_str());
			
			// NOTE 서버 실행
			manager.RunServers();
		}
		catch (const std::exception& e)
		{
			std::cerr << REDB "[" << ft::getCurrentTime() << "][error]" << "[Server exited unexpected reasons.]" << NC << std::endl;
			return (1);
		}
		return (0);
	}
}