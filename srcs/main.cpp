#include "ServerManager.hpp"

int	main(int argc, char* argv[], char* envp[])
{
	ServerManager	manager;

	manager.openLog();
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