#include "ServerManager.hpp"


ServerManager::ServerManager(void)
{
}

void					ServerManager::exitServer(const std::string& msg) const
{
	std::cerr << msg << std::endl;
	exit(1);
}

void					ServerManager::createServer(const std::string& configuration_file_path, char** envp)
{

}

void					ServerManager::runServer(void)
{

}