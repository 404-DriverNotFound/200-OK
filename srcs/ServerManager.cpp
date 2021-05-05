#include "ServerManager.hpp"

void	ServerManager::createServer(const std::string& configuration_file_path, char** envp)
{
	std::string					config_string = ft::getStringFromFile(configuration_file_path);
	std::string					config_block;
	std::vector<std::string>	server_strings;

	if (!splitConfigString(config_string, config_block, server_strings))
	{
		throw (std::invalid_argument("Failed to split configuration string"));
	}
	if (!isValidConfigBlock(config_block))
	{
		throw (std::invalid_argument("Config block is not valid."));
	}
	m_config = Config(config_block, envp);

	for (size_t i = 0; i < server_strings.size(); ++i)
	{
		std::string					server_block;
		std::vector<std::string>	location_blocks;
		if (!splitServerString(server_strings[i], server_block, location_blocks))
		{
			throw (std::invalid_argument("Failed to split Sever string(" + ft::to_string(i) + ")"));
		}
		if (!isValidServerBlock(server_block))
		{
			throw (std::invalid_argument("Server block(" + ft::to_string(i) + ") is not valid."));
		}
		for (size_t j = 0; j < location_blocks.size(); ++j)
		{
			if (!isValidLocationBlock(location_blocks[j]))
			{
				throw (std::invalid_argument("Location block(" + ft::to_string(i) + "-" + ft::to_string(j) + ") is not valid."));
			}
		}
		m_servers.push_back(Server(this, server_block, location_blocks, &this->m_config));
		m_server_fdset.insert(m_servers.back().get_m_fd());
	}
	writeCreateServerLog();
}

void	ServerManager::runServer()
{
	signal(SIGINT, changeSignal);

	timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 0;

	g_live = true;
	resetMaxFd();
	while (g_live)
	{
		int cnt;
		fdCopy(ALL_SET);

		if ((cnt = select(this->m_max_fd + 1, &this->m_read_copy_set, &this->m_write_copy_set, NULL, &timeout)) == -1)
		{
			perror("Server select error: ");
			ft::log(ServerManager::log_fd, "[Failed][Function]Select function failed(return -1)");
			throw std::runtime_error("select error");
		}
		else if (cnt == 0)
		{
			continue ;
		}
		writeServerHealthLog();
		for (std::vector<Server>::iterator it = m_servers.begin() ; it != m_servers.end() ; ++it)
		{
			it->run();
			closeOldConnection(it);
		}
		resetMaxFd();
	}
	exitServer("server exited.\n");
}

// WRITE_SET, WRITE_COPY_SET, READ_SET, READ_COPY_SET, ERROR_SET, ERROR_COPY_SET
void			fdSet(int fd, int SetType) // REVIEW: SetType 이 enum 이라서 int 로 두었음
{
	if (SetType == 0)
		FD_SET(fd, &m_write_set);
	else if (SetType == 1)
		FD_SET(fd, &m_write_copy_set);
	else if (SetType == 2)
		FD_SET(fd, &m_read_set);
	else if (SetType == 3)
		FD_SET(fd, &m_read_copy_set);
	else if (SetType == 4)
		FD_SET(fd, &m_error_set);
	else if (SetType == 5)
		FD_SET(fd, &m_error_copy_set);
}

void			fdZero(int SetType) // REVIEW: SetType 이 enum 이라서 int 로 두었음
{
	if (SetType == 0)
		FD_ZERO(fd, &m_write_set);
	else if (SetType == 1)
		FD_ZERO(fd, &m_write_copy_set);
	else if (SetType == 2)
		FD_ZERO(fd, &m_read_set);
	else if (SetType == 3)
		FD_ZERO(fd, &m_read_copy_set);
	else if (SetType == 4)
		FD_ZERO(fd, &m_error_set);
	else if (SetType == 5)
		FD_ZERO(fd, &m_error_copy_set);
}

void			fdClear(int fd, int SetType) // REVIEW: SetType 이 enum 이라서 int 로 두었음
{
	if (SetType == 0)
		FD_CLR(fd, &m_write_set);
	else if (SetType == 1)
		FD_CLR(fd, &m_write_copy_set);
	else if (SetType == 2)
		FD_CLR(fd, &m_read_set);
	else if (SetType == 3)
		FD_CLR(fd, &m_read_copy_set);
	else if (SetType == 4)
		FD_CLR(fd, &m_error_set);
	else if (SetType == 5)
		FD_CLR(fd, &m_error_copy_set);
}

void			fdIsset(int fd, int SetType) // REVIEW: SetType 이 enum 이라서 int 로 두었음
{
	if (SetType == 0)
		FD_ISSET(fd, &m_write_set);
	else if (SetType == 1)
		FD_ISSET(fd, &m_write_copy_set);
	else if (SetType == 2)
		FD_ISSET(fd, &m_read_set);
	else if (SetType == 3)
		FD_ISSET(fd, &m_read_copy_set);
	else if (SetType == 4)
		FD_ISSET(fd, &m_error_set);
	else if (SetType == 5)
		FD_ISSET(fd, &m_error_copy_set);
}

void			fdCopy(int SetType) // REVIEW: SetType 이 enum 이라서 int 로 두었음
{
	if (SetType == 0)
		FD_COPY(fd, &m_write_set);
	else if (SetType == 1)
		FD_COPY(fd, &m_write_copy_set);
	else if (SetType == 2)
		FD_COPY(fd, &m_read_set);
	else if (SetType == 3)
		FD_COPY(fd, &m_read_copy_set);
	else if (SetType == 4)
		FD_COPY(fd, &m_error_set);
	else if (SetType == 5)
		FD_COPY(fd, &m_error_copy_set);
}