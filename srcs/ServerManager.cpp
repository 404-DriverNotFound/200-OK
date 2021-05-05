#include "ServerManager.hpp"


ServerManager::ServerManager(void)
{
}

void					ServerManager::exitServer(const std::string& msg) const
{
	std::cerr << msg << std::endl;
	exit(1);
}

void	ServerManager::createServer(const std::string& configuration_file_path, char** envp)
{
	// // std::string					config_string = ft::getStringFromFile(configuration_file_path);
	std::string					config_block;
	std::vector<std::string>	server_strings;

	// // if (!splitConfigString(config_string, config_block, server_strings))
	// // {
	// 	// throw (std::invalid_argument("Failed to split configuration string"));
	// // }
	// // if (!isValidConfigBlock(config_block))
	// // {
	// 	// throw (std::invalid_argument("Config block is not valid."));
	// // }
	m_config = Config(config_block, envp);

	// NOTE : config 파싱에서 서버블록이 1개 있었다고 가정하고 작업
	for (size_t i = 0; i < 1/*server_strings.size()*/; ++i)
	{
		std::string					server_block;
		std::vector<std::string>	location_blocks;
		// if (!splitServerString(server_strings[i], server_block, location_blocks))
	// 	// {
	// 	// 	throw (std::invalid_argument("Failed to split Sever string(" + ft::to_string(i) + ")"));
	// 	// }
	// 	// if (!isValidServerBlock(server_block))
	// 	// {
	// 	// 	throw (std::invalid_argument("Server block(" + ft::to_string(i) + ") is not valid."));
	// 	// }
	// 	// for (size_t j = 0; j < location_blocks.size(); ++j)
	// 	// {
	// 	// 	if (!isValidLocationBlock(location_blocks[j]))
	// 	// 	{
	// 	// 		throw (std::invalid_argument("Location block(" + ft::to_string(i) + "-" + ft::to_string(j) + ") is not valid."));
	// 	// 	}
	// 	// }
		m_servers.push_back(Server(this, server_block, location_blocks, &this->m_config));
		m_server_fdset.insert(m_servers.back().get_m_fd()); // REVIEW: 이유가 있으니 이런 코드가 있겠지만 의미하는 바를 파악하지 못했음
	}
	// // writeCreateServerLog();
}

void					ServerManager::runServer(void)
{

}