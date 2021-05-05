#include "ServerManager.hpp"


ServerManager::ServerManager(void)
{
	FD_ZERO(&m_read_set);
	FD_ZERO(&m_write_set);
}

void		ServerManager::exitServer(const std::string& msg) const
{
	std::cerr << msg << std::endl;
	exit(1);
}

void		ServerManager::createServer(const std::string& configuration_file_path, char** envp)
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

void		ServerManager::runServer(void)
{
	// signal(SIGINT, changeSignal);

	struct timeval	timeout; memset(&timeout, 0, sizeof(struct timeval));

	// g_live = true;
	resetMaxFd(512);
	for (std::vector<Server>::iterator it = m_servers.begin() ; it != m_servers.end() ; ++it)
	{
		FD_SET(it->get_m_fd(), &m_read_set);
	}
	while (true	/* g_live */)
	{
		fdCopy(ALL_SET);
		int	cnt = select(m_max_fd + 1, &m_read_copy_set, &m_write_copy_set, NULL, &timeout);
		if (cnt > 0)
		{
			// writeServerHealthLog();
			for (std::vector<Server>::iterator it = m_servers.begin() ; it != m_servers.end() ; ++it)
			{
			// 	it->run();
			// 	closeOldConnection(it);
			}
			resetMaxFd(-1	/* cnt */);	// FIXME: cnt가 맞을지 -1이 맞을지 잘 모르겠음.
		}
		else if (cnt < 0)
		{
			// perror("Server select error: ");
			// ft::log(ServerManager::log_fd, "[Failed][Function]Select function failed(return -1)");
			throw std::runtime_error("select error");
		}
		else
		{
			continue ;
		}
	}
	exitServer("server exited.\n");
}

void		ServerManager::set_m_max_fd(const int& fd)
{
	m_max_fd = fd;
}

void		ServerManager::resetMaxFd(int new_max_fd)
{
	if (new_max_fd != -1)
	{
		set_m_max_fd(new_max_fd);
	}
	else
	{
		for (int i = 512; i >= 0; --i)
		{
			if (fdIsset(i, READ_SET) || fdIsset(i, WRITE_SET))
			{
				m_max_fd = i;
				break ;
			}
		}
	}
}

void		ServerManager::fdCopy(SetType fdset)
{
	if (fdset == WRITE_SET || fdset == ALL_SET)
	{
		FD_ZERO(&m_write_copy_set);
		m_write_copy_set = m_write_set;
	}
	if (fdset == READ_SET || fdset == ALL_SET)
	{
		FD_ZERO(&m_read_copy_set);
		m_read_copy_set = m_read_set;
	}
}

bool		ServerManager::fdIsset(int fd, SetType fdset)
{
	if (fdset == WRITE_SET)
	{
		return (FD_ISSET(fd, &m_write_copy_set));
	}
	else if (fdset == READ_SET)
	{
		return (FD_ISSET(fd, &m_read_copy_set));
	}
	else
	{
		return (false);
	}
}