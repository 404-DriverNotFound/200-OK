#include <iostream>
#include "ServerManager.hpp"

// #define DEFAULT_CONFIG_FILE_PATH "default.config"

std::map<int, std::string> Response::mStatusMap; // NOTE static 변수도 전역변수라도 한번 선언을 해줘야함.

char** gEnv;
int gTotalClients;

int		main(int argc, char* argv[], char* envp[])
{
	gEnv = envp; // FIXME env 임시로 전역변수로

	Response::initStatusMap();
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
			if (argc == 2)
			{
				int fd = 0; fd = open(argv[1], O_RDONLY);
				if (fd > 2)
				{
					close(fd);
					manager.CreateServers(std::string(argv[1]));
				}
				else
				{
					throw (static_cast<const string>("No such a file"));
				}
			}
			else
			{
				manager.CreateServers(std::string(DEFAULT_CONFIG_FILE_PATH));
			}
			// NOTE 서버 실행
			manager.RunServers();
		}
		catch (const std::string e)
		{
			manager.ExitServer(e);
		}
		return (0);
	}
}