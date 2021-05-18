#include "ServerManager.hpp"

// bool	g_live; // REVIEW: g_live 를 쓰길래, 전역변수 느낌인 것같아서 여기에 선언함.
std::map<int, std::string> Response::m_status_map; // NOTE static 변수도 전역변수라도 한번 선언을 해줘야함.
std::map<std::string, std::string> Response::m_MIME_map; // NOTE static 변수도 전역변수라도 한번 선언을 해줘야함.

char** g_env;

int		main(int argc, char* argv[], char* envp[])
{
	g_env = envp; // FIXME env 임시로 전역변수로

	Response::init_status_map();
	Response::init_MIME_map();
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
				manager.createServer(std::string(argv[1]), envp);
			}
			else
			{
				manager.createServer(std::string(DEFAULT_CONFIG_FILE_PATH), envp);
			}
		}
		catch (std::exception& e)
		{
			manager.exitServer(e.what());
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