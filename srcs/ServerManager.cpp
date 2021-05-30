#include "ServerManager.hpp"

extern int gTotalClients;

ServerManager::ServerManager(void)
	: mMaxFd(INIT_FD_MAX), mTotalClients(0)
{
	FD_ZERO(&mReadFds);
	FD_ZERO(&mWriteFds);
}

void		ServerManager::exitServer(const std::string& msg) const
{
	std::cerr << REDB "[" << ft::getCurrentTime() << "] [error] " << msg << NC << std::endl;
	exit(1);
}

void		ServerManager::CreateServers(const std::string& configurationFilePath, char** envp)
{
	(void)envp;
	// ANCHOR 1단계 parsing 전처리단계
	ConfigFiles configfiles(configurationFilePath.c_str());
	if (this->isOverlapedServer(&configfiles) == true)
		throw (static_cast<const string>("Overlaped port and host"));
	configfiles.ShowConfigs(); // NOTE configfile의 값을 확인하고싶으면,
	// ANCHOR 2단계 parsing
	this->SetServers_value(&configfiles);
	this->ShowServers(); // NOTE server로 구성된 값을 확인하고싶으면,
	// // ANCHOR 3단계 parsing -> port에 대해서 listen 함수까지 호출함.
	this->SetServers();


	// writeCreateServerLog();
}

void		ServerManager::RunServers(void)
{
	struct timeval	timeout;
	for (std::vector<Server>::iterator it = m_servers.begin(); it != m_servers.end(); ++it)
	{
		FT_FD_SET(it->get_m_fd(), &mReadFds);
	}
	while (true)
	{
		timeout.tv_sec = SELECT_TIMEOUT_SEC; timeout.tv_usec = SELECT_TIMEOUT_USEC;

		FD_COPY(&mReadFds, &mReadCopyFds);
		FD_COPY(&mWriteFds, &mWriteCopyFds);
		updateMaxFd();
		errno = 0;
		int	cnt = select(mMaxFd + 1, &mReadCopyFds, &mWriteCopyFds, NULL, &timeout);
		if (cnt < 0)
		{
			perror("errno: ");
			throw (static_cast<const string>("Select Error"));
		}
		else if (cnt == 0)
		{
			for (std::vector<Server>::iterator it = m_servers.begin() ; it != m_servers.end() ; ++it)
			{
				closeOldConnection(it);
			}
			std::cout << "timeout\n";
		}
		else if (cnt > 0)
		{
			// std::vector<int> read_set;
			// read_set = ft::getVector_changedFD(&mReadCopyFds, mMaxFd + 1);
			// std::vector<int> write_set;
			// write_set = ft::getVector_changedFD(&mWriteCopyFds, mMaxFd + 1);
		}
		std::cout << "select : " << cnt << endl;
		std::cout << "-------------------------------" << std::endl;

		// ANCHOR 참고코드
		// writeServerHealthLog();
		for (std::vector<Server>::iterator it = m_servers.begin() ; it != m_servers.end() ; ++it)
		{
			// cout << "loop_run?" << endl;
			it->run();
			// closeOldConnection(it);
		}
		// updateMaxFd();
		// cout << "-------------------------------" << endl;
	}
}

// const int&	ServerManager::get_mMaxFd(void) const{return (this->mMaxFd);}

// void		ServerManager::set_mMaxFd(const int& fd)
// {
// 	mMaxFd = fd;
// }

// void		ServerManager::initMaxFd()
// {
// 	set_mMaxFd(1023);
// 	memset((void *)&mReadFds, 0, 4*32);
// 	memset((void *)&mWriteFds, 0, 4*32);
// }

void		ServerManager::updateMaxFd() // REVIEW mMaxFd에 대해서 +- 증감 연산으로도 충분히 계산할 수 있을 것 같아서, while문을 도는 것이 비효율적이라는 생각이 듦
{
	// REVIEW 하향식으로 전체 탐색하는 것으로 MaxFd를 찾아냈었음. (+상향식으로는 체크할 수 없음)
	// for (int i = INIT_FD_MAX; i >= 0; --i)
	// {
	// 	// if (fdIsset(i, READ_SET) || fdIsset(i, WRITE_SET))
	// 	if (FD_ISSET(i, &mReadFds) || FD_ISSET(i, &mWriteFds))
	// 	{
	// 		mMaxFd = i;
	// 		break ;
	// 	}
	// }

	if (gTotalClients == 0)
	{
		mMaxFd = 2 + this->m_servers.size();
	}
}

//ANCHOR yunslee line: 160~332 추가

int ServerManager::SetServers_value(ConfigFiles *configs)
{
	while (configs->mconfigs.size() != 0)
	{
		ConfigFile &config = configs->mconfigs[configs->mconfigs.size() - 1];
		int idxserver;
		if (-1 == (idxserver = this->GetIdxServer(config.mport, config.mhost)))
		{
			// NOTE port별로 서버도 없는 상황, 아예 새롭게 만들면 됨.
			Server server(this); // FIXME ServerManager * 가 들어갈 것으로 예상했으나 들어가지 않음. 그래서 Setservers에서 처리해줌
			server.mport = config.mport;
			server.mhost = config.mhost;
			
			ServerBlock temp;
			temp.mserverName = config.mserver_name;
			temp.mtimeout = config.mtimeout;
			server.mserverBlocks.push_back(temp);

			LocationPath temp2;
			temp2.mlocationPath = config.mlocation_path;
			temp2.mroot = config.mroot;
			temp2.mindex_pages = config.mindex_pages;
			temp2.merror_page = config.merror_page;
			temp2.mauto_index = config.mauto_index;
			temp2.mclient_max_body_size = config.mclient_max_body_size;

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
			server.mserverBlocks.push_back(temp);
			
			LocationPath temp2;
			temp2.mlocationPath = config.mlocation_path;
			temp2.mroot = config.mroot;
			temp2.mindex_pages = config.mindex_pages;
			temp2.merror_page = config.merror_page;
			temp2.mcgi_extension = config.mcgi_extension;
			temp2.m_method = config.m_method;
			temp2.mauto_index = config.mauto_index;
			temp2.mclient_max_body_size = config.mclient_max_body_size;
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
			temp2.mauto_index = config.mauto_index;
			temp2.mclient_max_body_size = config.mclient_max_body_size;
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

int ServerManager::GetIdxServer(int port, std::string host)
{
	if (this->m_servers.size() == 0)
		return (-1);
	for (size_t i = 0; i < m_servers.size(); i++)
	{
		if (this->m_servers[i].mport == port && this->m_servers[i].mhost == host)
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
		std::cout << "host: " << temp.mhost << std::endl;
		for (size_t j = 0; j < temp.mserverBlocks.size(); j++)
		{
			ServerBlock temp2 = temp.mserverBlocks[j];
			std::cout << "server_name: " << temp2.mserverName << std::endl;
			std::cout << "timeout: " << temp2.mtimeout << std::endl;

			std::cout << temp2.mserverName << ": " << "locationPathsize: " << temp2.mlocationPaths.size() << std::endl;
			for (size_t k = 0; k < temp2.mlocationPaths.size(); k++)
			{
				LocationPath temp3 = temp2.mlocationPaths[k];
				std::cout << "mlocationpath: " << temp3.mlocationPath.getPath() << std::endl;
				std::cout << "error_page: " << temp3.merror_page.getPath() << std::endl;
				std::cout << "root: " << temp3.mroot.getPath() << std::endl;
				std::cout << "auto_index: " << temp3.mauto_index << std::endl;
				std::cout << "client_max_body_size: " << temp3.mclient_max_body_size << std::endl;
				
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
		server.SetSocket();
		server.m_manager = this; 
		server.m_connections[server.msocket] = Connection(server.msocket, server.mhost, server.mport);
	}
	return (1);
}


// REVIEW Socket 클래스에서 단순히 int msocket(fd)를 쓰는 것으로 변경되었으므로 다시한번 확인이 필요함.
// void ServerManager::SetFdMax(int maxfd){this->mMaxFd = maxfd;}

// int ServerManager::GetFdMax(){return (this->mMaxFd);}

// fd_set& ServerManager::GetReadFds() {return (this->mReadCopyFds);}

// fd_set & ServerManager::GetReadSet() {return (this->mReadFds);}

// fd_set & ServerManager::GetWriteCopySet() {return (this->mWriteCopyFds);}

// fd_set & ServerManager::GetWriteSet() {return (this->mWriteFds);}

// fd_set & ServerManager::GetErrorCopySet() {return (this->m_error_copy_set);}

// fd_set & ServerManager::GetErrorSet() {return (this->m_error_set);}



std::vector<Server> &ServerManager::GetServers() {return (this->m_servers);}

void	ServerManager::closeOldConnection(std::vector<Server>::iterator server_it)
{
	std::map<int, Connection>::iterator it = server_it->m_connections.begin();
	while (it != server_it->m_connections.end())
	{
		std::map<int, Connection>::iterator it2 = it++;
		int fd = it2->first;
		if (it2->second.get_m_fd() == server_it->msocket)
		{
			continue ;
		}
		if (it2->second.isKeepConnection() == false && (FD_ISSET(fd, &this->mReadCopyFds) == 0))
		{
			std::cout << "closeOldconnection: " << fd << std::endl;
			server_it->closeConnection(it2->second.get_m_fd());
			return ;
		}
	}
	return ;
}

class overlaped_value
{
	public:
		std::string mhost;
		uint16_t mport;
		std::string mserverName;
		overlaped_value(std::string a, uint16_t b, std::string c)
		:
			mhost(a),
			mport(b),
			mserverName(c)
		{};

		bool operator < (const overlaped_value& t) const
		{
			if (mhost < t.mhost || mport < t.mport || mserverName < t.mserverName)
				return true;
			else
				return false;
		}
		bool operator > (const overlaped_value& t) const
		{
			if (mhost > t.mhost || mport > t.mport || mserverName > t.mserverName)
				return true;
			else
				return false;
		}
		bool operator == (const overlaped_value& t) const
		{
			if (mhost == t.mhost && mport == t.mport && mserverName == t.mserverName)
				return true;
			else
				return false;
		}
};

bool	ServerManager::isOverlapedServer(ConfigFiles* configfiles)
{
	std::vector<overlaped_value> server_list;
	for (size_t i = 0; i < configfiles->mconfigs.size(); i++)
	{
		ConfigFile &config = configfiles->mconfigs[i];
		overlaped_value temp(config.mhost, config.mport, config.mserver_name);
		for (size_t j = 0; j < server_list.size(); j++)
		{
			if (server_list[j] == temp)
			{
				if (config.mlocation_path.getSize() == 0)
					return (true);
			}
		}
		if (config.mlocation_path.getSize() == 0)
			server_list.push_back(temp);
	}
	return (false);
}

const fd_set&				ServerManager::GetReadCopyFds(void) const
{
	return (mReadCopyFds);
}

void						ServerManager::SetReadCopyFds(const int& fd)
{
	FD_SET(fd, &mReadCopyFds);
}

void						ServerManager::ClrReadCopyFds(const int& fd)
{
	FD_CLR(fd, &mReadCopyFds);
}

const fd_set&				ServerManager::GetReadFds(void) const
{
	return (mReadFds);
}

void						ServerManager::SetReadFds(const int& fd)
{
	FD_SET(fd, &mReadFds);
}

void						ServerManager::ClrReadFds(const int& fd)
{
	FD_CLR(fd, &mReadFds);
}

const fd_set&				ServerManager::GetWriteCopyFds(void) const
{
	return (mWriteCopyFds);
}

void						ServerManager::SetWriteCopyFds(const int& fd)
{
	FD_SET(fd, &mWriteCopyFds);
}

void						ServerManager::ClrWriteCopyFds(const int& fd)
{
	FD_CLR(fd, &mWriteCopyFds);
}

const fd_set&				ServerManager::GetWriteFds(void) const
{
	return (mWriteFds);
}

void						ServerManager::SetWriteFds(const int& fd)
{
	FD_SET(fd, &mWriteFds);
}

void						ServerManager::ClrWriteFds(const int& fd)
{
	FD_CLR(fd, &mWriteFds);
}

const int&					ServerManager::GetMaxFd(void) const{ return (this->mMaxFd);}
void						ServerManager::SetMaxFd(const int &fd)
{
	this->mMaxFd = fd;
}

const uint16_t&					ServerManager::GetTotalClients(void) const{ return (this->mTotalClients);}
void							ServerManager::SetTotalClients(const uint16_t &totalClient)
{
	this->mTotalClients = totalClient;
}