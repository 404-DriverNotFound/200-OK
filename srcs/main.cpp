#include "ServerManager.hpp"

// bool	g_live; // REVIEW: g_live 를 쓰길래, 전역변수 느낌인 것같아서 여기에 선언함.
std::map<int, std::string> Response::m_status_map; // NOTE static 변수도 전역변수라도 한번 선언을 해줘야함.

char** g_env;

int		main(int argc, char* argv[], char* envp[])
{
	g_env = envp; // FIXME env 임시로 전역변수로

	Response::init_status_map();
	ServerManager	manager;
	// manager.openLog();
	if (argc > 2)
	{
		manager.exitServer("There are many arguments.");
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
					manager.createServer(std::string(argv[1]), envp);
				}
				else
				{
					throw (static_cast<const string>("No such a file"));
				}
			}
			else
			{
				manager.createServer(std::string(DEFAULT_CONFIG_FILE_PATH), envp);
			}
		}
		catch (const std::string e)
		{
			manager.exitServer(e);
		}
	}
	
	try
	{
		manager.runServer();
	}
	catch (std::exception& e)
	{
		manager.exitServer(e.what());
	}
	return (0);
}