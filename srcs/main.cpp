#include "ServerManager.hpp"

std::map<int, std::string> Response::mStatusMap;
char** gEnv;
int gTotalClients;

int		main(int argc, char* argv[], char* envp[])
{
	gEnv = envp;

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
					throw (static_cast<const std::string>("No such a file"));
				}
			}
			else
			{
				manager.CreateServers(std::string(DEFAULT_CONFIG_FILE_PATH));
			}
			manager.RunServers();
		}
		catch (const std::string e)
		{
			manager.ExitServer(e);
		}
		return (0);
	}
}