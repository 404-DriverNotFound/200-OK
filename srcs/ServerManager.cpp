#include "ServerManager.hpp"


ServerManager::ServerManager(void)
{
	FT_FD_ZERO(&m_read_set);
	FT_FD_ZERO(&m_write_set);
}

void		ServerManager::exitServer(const std::string& msg) const
{
	std::cerr << REDB "[" << ft::getCurrentTime() << "] [error] " << msg << NC << std::endl;
	exit(1);
}

void		ServerManager::createServer(const std::string& configuration_file_path, char** envp)
{
	// ANCHOR 1단계 parsing 전처리단계
	ConfigFiles configfiles(configuration_file_path.c_str());
	// configfiles.ShowConfigs(); // NOTE configfile의 값을 확인하고싶으면,
	// ANCHOR 2단계 parsing
	this->SetServers_value(&configfiles); // NOTE server로 구성된 값을 확인하고싶으면,this->ShowServers();
	// // ANCHOR 3단계 parsing -> port에 대해서 listen 함수까지 호출함.
	this->SetServers();


	// writeCreateServerLog();
}

void		ServerManager::runServer(void)
{
	// signal(SIGINT, changeSignal);
	struct timeval	timeout; memset(&timeout, 0, sizeof(struct timeval));
	// g_live = true;
	initMaxFd();
	for (std::vector<Server>::iterator it = m_servers.begin() ; it != m_servers.end() ; ++it)
	{
		FD_SET(it->get_m_fd(), &m_read_set);
	}
	while (true	/* g_live */)
	{
		timeout.tv_sec = SELECT_TIMEOUT_SEC; timeout.tv_usec = SELECT_TIMEOUT_USEC;
		// fdCopy(ALL_SET);
		FT_FD_COPY(&m_read_set, &m_read_copy_set);
		FT_FD_COPY(&m_write_set, &m_write_copy_set);
		resetMaxFd();
		// cout << "m_max_fd: " << m_max_fd << endl;
		int	cnt = select(m_max_fd + 1, &m_read_copy_set, &m_write_copy_set, NULL, &timeout);
		if (cnt < 0)
		{
			// std::cout << "Select error\n";
			throw std::runtime_error("select error");
			break ;
		}
		else if (cnt == 0)
		{
			// std::cout << "timeout\n";
		}
		else if (cnt > 0)
		{
			std::vector<int> read_set;
			read_set = ft::getVector_changedFD(&m_read_copy_set, m_max_fd + 1);
			std::vector<int> write_set;
			write_set = ft::getVector_changedFD(&m_write_copy_set, m_max_fd + 1);
			std::cout << "select :" << cnt << endl;
		}

		// ANCHOR 참고코드
		// writeServerHealthLog();
		for (std::vector<Server>::iterator it = m_servers.begin() ; it != m_servers.end() ; ++it)
		{
			// cout << "loop_run?" << endl;
			it->run();
			// closeOldConnection(it);
		}
		// resetMaxFd();
		// cout << "-------------------------------" << endl;
	}
	exitServer("server exited.\n");
}

const int&	ServerManager::get_m_max_fd(void) const{return (this->m_max_fd);}

void		ServerManager::set_m_max_fd(const int& fd)
{
	m_max_fd = fd;
}

void		ServerManager::initMaxFd()
{
	set_m_max_fd(INIT_FD_MAX);
}

void		ServerManager::resetMaxFd() // REVIEW m_max_fd에 대해서 +- 증감 연산으로도 충분히 계산할 수 있을 것 같아서, while문을 도는 것이 비효율적이라는 생각이 듦
{
	// STUB 하향식. 연결된 fd가 많으면, 이 방법이 더 효율적임
	for (int i = 512; i >= 0; --i)
	{
		// if (fdIsset(i, READ_SET) || fdIsset(i, WRITE_SET))
		if (FT_FD_ISSET(i, &m_read_set) || FT_FD_ISSET(i, &m_write_set))
		{
			m_max_fd = i;
			break ;
		}
	}

	// FIXME 상향식으로는 체크할 수 없음... from yunslee
	// STUB 상향식. 연결된 fd가 적으면, 이 방법이 더 효율적임
	// for (int i = 3; i <= 512; i++)
	// {
	// 	if (fdIsset(i, READ_SET) == false && fdIsset(i, WRITE_SET) == false)
	// 	{
	// 		m_max_fd = i - 1;
	// 		break ;
	// 	}
	// }
}

// void		ServerManager::fdCopy(SetType fdset)
// {
// 	if (fdset == WRITE_SET || fdset == ALL_SET)
// 	{
// 		FT_FD_ZERO(&m_write_copy_set);
// 		m_write_copy_set = m_write_set;
// 	}
// 	if (fdset == READ_SET || fdset == ALL_SET)
// 	{
// 		FT_FD_ZERO(&m_read_copy_set);
// 		m_read_copy_set = m_read_set;
// 	}
// }

// bool		ServerManager::fdIsset(int fd, SetType fdset)
// {
// 	if (fdset == WRITE_SET)
// 	{
// 		if (FT_FD_ISSET(fd, &m_write_copy_set))
// 	{
// 			return (true);
// 	}
// 		else
// 	{
// 			return (false);
// 		}
// 	}
// 	else if (fdset == READ_SET)
// 	{
// 		if (FT_FD_ISSET(fd, &m_read_copy_set))
// 		{
// 			return (true);
// 		}
// 		else
// 		{
// 			return (false);
// 		}
// 	}
// 	return (false);
// }



//ANCHOR yunslee line: 160~332 추가

int ServerManager::SetServers_value(ConfigFiles *configs)
{
	while (configs->mconfigs.size() != 0)
	{
		ConfigFile &config = configs->mconfigs[configs->mconfigs.size() - 1];
		int idxserver;
		if (-1 == (idxserver = this->GetIdxServer(config.mport)))
		{
			// NOTE port별로 서버도 없는 상황, 아예 새롭게 만들면 됨.
			Server server(this); // FIXME ServerManager * 가 들어갈 것으로 예상했으나 들어가지 않음. 그래서 Setservers에서 처리해줌
			server.mport = config.mport;
			
			ServerBlock temp;
			temp.mserverName = config.mserver_name;
			temp.mtimeout = config.mtimeout;
			temp.mauto_index = config.mauto_index;
			server.mserverBlocks.push_back(temp);

			LocationPath temp2;
			temp2.mlocationPath = config.mlocation_path;
			temp2.mroot = config.mroot;
			temp2.mindex_pages = config.mindex_pages;
			temp2.merror_page = config.merror_page;

			temp2.mcgi_extension = config.mcgi_extension;
			temp2.m_method = config.m_method;
			server.mserverBlocks[0].mlocationPaths.push_back(temp2);

			this->m_servers.push_back(server);
			configs->mconfigs.pop_back();
			continue;
		}
		Server &server = this->m_servers[idxserver];
		int idxserverblock;
		if (-1 == (idxserverblock = this->GetIdxServerBlock(server.mserverBlocks, config.mserver_name)))
		{
			// NOTE port는 있으나 다른 서버네임을 가지고 있음
			ServerBlock temp;
			temp.mserverName = config.mserver_name;
			temp.mtimeout = config.mtimeout;
			temp.mauto_index = config.mauto_index;
			server.mserverBlocks.push_back(temp);
			
			LocationPath temp2;
			temp2.mlocationPath = config.mlocation_path;
			temp2.mroot = config.mroot;
			temp2.mindex_pages = config.mindex_pages;
			temp2.merror_page = config.merror_page;
			temp2.mcgi_extension = config.mcgi_extension;
			temp2.m_method = config.m_method;
			server.mserverBlocks[server.mserverBlocks.size() - 1].mlocationPaths.push_back(temp2);
			configs->mconfigs.pop_back();
			continue;
		}
		ServerBlock &temp = server.mserverBlocks[idxserverblock];
		int idxlocationblock;
		if (-1 == (idxlocationblock = this->GetIdxLocationPath(temp.mlocationPaths, config.mlocation_path)))
		{
			// NOTE port도 있고, 서버네임도 있으나, location이 다른 경우
			LocationPath temp2;
			temp2.mlocationPath = config.mlocation_path;
			temp2.mroot = config.mroot;
			temp2.mindex_pages = config.mindex_pages;
			temp2.merror_page = config.merror_page;
			temp2.mcgi_extension = config.mcgi_extension;
			temp2.m_method = config.m_method;
			server.mserverBlocks[server.mserverBlocks.size() - 1].mlocationPaths.push_back(temp2);
			configs->mconfigs.pop_back();
			continue;
		}
		// NOTE 세개가 모두 같은 경우는 에러처리한다.
		configs->mconfigs.pop_back();
		return (-1);
	}
	return (1);
}

int ServerManager::GetIdxServer(int port)
{
	if (this->m_servers.size() == 0)
		return (-1);
	for (size_t i = 0; i < m_servers.size(); i++)
	{
		if (this->m_servers[i].mport == port)
			return (i);
	}
	return (-1);
}

int ServerManager::GetIdxServerBlock(std::vector<ServerBlock> &serverBlocks, std::string serverName)
{
	if (serverBlocks.size() == 0)
		return (-1);
	for (size_t i = 0; i < serverBlocks.size(); i++)
	{
		ServerBlock temp = serverBlocks[i];
		if (temp.mserverName.compare(serverName) == 0)
			return (i);
	}
	return (-1);
}

int ServerManager::GetIdxLocationPath(std::vector<LocationPath> &locationPaths, Path locationPath)
{
	if (locationPaths.size() == 0)
		return (-1);
	for (size_t i = 0; i < locationPaths.size(); i++)
	{
		LocationPath &temp = locationPaths[i];
		if (temp.mlocationPath == locationPath)
			return (i);
	}
	return (-1);
}

int ServerManager::ShowServers()
{
	for (size_t i = 0; i < this->m_servers.size(); i++)
	{
		Server &temp = this->m_servers[i];
		std::cout << "port: " << temp.mport << std::endl;
		for (size_t j = 0; j < temp.mserverBlocks.size(); j++)
		{
			ServerBlock temp2 = temp.mserverBlocks[j];
			std::cout << "server_name: " << temp2.mserverName << std::endl;
			std::cout << "timeout: " << temp2.mtimeout << std::endl;
			std::cout << "auto_index: " << temp2.mauto_index << std::endl;

			std::cout << temp2.mserverName << ": " << "locationPathsize: " << temp2.mlocationPaths.size() << std::endl;
			for (size_t k = 0; k < temp2.mlocationPaths.size(); k++)
			{
				LocationPath temp3 = temp2.mlocationPaths[k];
				std::cout << "mlocationpath: " << temp3.mlocationPath.getPath() << std::endl;
				std::cout << "error_page: " << temp3.merror_page.getPath() << std::endl;
				std::cout << "root: " << temp3.mroot.getPath() << std::endl;
				for (size_t l = 0; l < temp3.mindex_pages.size(); l++)
				{
					std::cout << "index_pages: " << temp3.mindex_pages[l].getPath() << std::endl;	/* code */
				}
				std::cout << "method: ";
				for (size_t j = 0; j < temp3.m_method.size(); j++)
				{
					std::cout << temp3.m_method[j] << " ";
				}
				std::cout << endl;
				std::cout << "cgi_extension: ";
				for (size_t j = 0; j < temp3.mcgi_extension.size(); j++)
				{
					std::cout << temp3.mcgi_extension[j] << " ";
				}
				std::cout << endl;
				std::cout << "------------------------------ locationPath" << std::endl;
			}
			std::cout << "============================== server name" << std::endl;
		}
		std::cout << "=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- port" << std::endl;	
	}
	return (1);
}

int ServerManager::SetServers()
{
	for (size_t i = 0; i < this->m_servers.size(); i++)
	{
		Server &server = this->m_servers[i];
		server.SetSocket("0.0.0.0", server.mport);
		server.m_manager = this; 
		server.m_connections[server.msocket] = Connection(server.msocket, "0.0.0.0", server.mport);
	}
	return (1);
}


// REVIEW Socket 클래스에서 단순히 int msocket(fd)를 쓰는 것으로 변경되었으므로 다시한번 확인이 필요함.
void ServerManager::SetFdMax(int maxfd){this->m_max_fd = maxfd;}

int ServerManager::GetFdMax(){return (this->m_max_fd);}

fd_set& ServerManager::GetReadCopySet() {return (this->m_read_copy_set);}

fd_set & ServerManager::GetReadSet() {return (this->m_read_set);}

fd_set & ServerManager::GetWriteCopySet() {return (this->m_write_copy_set);}

fd_set & ServerManager::GetWriteSet() {return (this->m_write_set);}

fd_set & ServerManager::GetErrorCopySet() {return (this->m_error_copy_set);}

fd_set & ServerManager::GetErrorSet() {return (this->m_error_set);}



std::vector<Server> &ServerManager::GetServers() {return (this->m_servers);}

void	ServerManager::closeOldConnection(std::vector<Server>::iterator server_it)
{
	std::map<int, Connection>::iterator it = server_it->m_connections.begin();
	while (it != server_it->m_connections.end())
	{
		std::map<int, Connection>::iterator it2 = it++;
		int fd = it2->first;
		if (it2->second.get_m_fd() == server_it->msocket)
			continue ;
		// cout << "loop_closeOldConnection?" << endl;
		if (it2->second.isKeepConnection() == false) // ANCHOR
		{
			cout << "closeOldConnection: " << it2->second.get_m_fd() << endl;
			close (it2->second.get_m_fd());
			FT_FD_CLR(it2->second.get_m_fd(), &(server_it->m_manager->GetWriteSet()));
			FT_FD_CLR(it2->second.get_m_fd(), &(server_it->m_manager->GetReadSet()));
			server_it->m_connections.erase(it2);
			return ;
		}
	}
	return ;
}