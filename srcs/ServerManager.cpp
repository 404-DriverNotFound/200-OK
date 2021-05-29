#include "ServerManager.hpp"

ServerManager::ServerManager(void)
	: mMaxFd(INIT_FD_MAX)
{
	FD_ZERO(&mReadFds);
	FD_ZERO(&mWriteFds);
}

void		ServerManager::CreateServers(const std::string& configurationFilePath)
{
	// ANCHOR 1단계 parsing 전처리단계
	ConfigFiles configFiles(configurationFilePath.c_str());
	configFiles.ShowConfigs(); // NOTE configfile의 값을 확인하고싶으면,

	// ANCHOR 2단계 parsing
	this->setServersValue(&configFiles);
	this->showServers(); // NOTE server로 구성된 값을 확인하고싶으면,

	// // ANCHOR 3단계 parsing -> port에 대해서 listen 함수까지 호출함.
	this->setServers();
}

void		ServerManager::RunServers(void)
{
	struct timeval	timeOut;
	for (std::vector<Server>::iterator it = mServers.begin(); it != mServers.end(); ++it)
	{
		FT_FD_SET(it->getSocket(), &mReadFds);
	}
	while (true)
	{
		timeOut.tv_sec = SELECT_TIMEOUT_SEC; timeOut.tv_usec = SELECT_TIMEOUT_USEC;

		FD_COPY(&mReadFds, &mReadCopyFds);
		FD_COPY(&mWriteFds, &mWriteCopyFds);
		updateMaxFd();
		errno = 0;
		int	cnt = select(mMaxFd + 1, &mReadCopyFds, &mWriteCopyFds, NULL, &timeOut);
		perror("errno: ");
		if (cnt < 0)
		{
			std::cout << "Select error\n";
			throw std::exception();
		}
		else if (cnt == 0)
		{
			for (std::vector<Server>::iterator it = mServers.begin(); it != mServers.end(); ++it)
			{
				closeOldConnection(it);
			}
		}
		else if (cnt > 0)
		{
			// std::vector<int> read_set;
			// read_set = ft::getVector_changedFD(&mReadCopyFds);
			// std::vector<int> write_set;
			// write_set = ft::getVector_changedFD(&mWriteCopyFds);
		}
		std::cout << "select : " << cnt << std::endl;
		std::cout << "-------------------------------" << std::endl;

		for (std::vector<Server>::iterator it = mServers.begin() ; it != mServers.end() ; ++it)
		{
			it->run();
			// closeOldConnection(it);
		}
	}
}
void		ServerManager::updateMaxFd(void) // REVIEW mMaxFd에 대해서 +- 증감 연산으로도 충분히 계산할 수 있을 것 같아서, while문을 도는 것이 비효율적이라는 생각이 듦
{
	// STUB 하향식. 연결된 fd가 많으면, 이 방법이 더 효율적임
	for (int i = 1023; i >= 0; --i)
	{
		// if (fdIsset(i, READ_SET) || fdIsset(i, WRITE_SET))
		if (FD_ISSET(i, &mReadFds) || FD_ISSET(i, &mWriteFds))
		{
			mMaxFd = i;
			break ;
		}
	}

	// FIXME 상향식으로는 체크할 수 없음... from yunslee
	// STUB 상향식. 연결된 fd가 적으면, 이 방법이 더 효율적임
	// for (int i = 3; i <= 512; i++)
	// {
	// 	if (fdIsset(i, READ_SET) == false && fdIsset(i, WRITE_SET) == false)
	// 	{
	// 		mMaxFd = i - 1;
	// 		break ;
	// 	}
	// }
}

//ANCHOR yunslee line: 160~332 추가

int ServerManager::setServersValue(ConfigFiles* configs)
{
	while (configs->mconfigs.size() != 0)
	{
		ConfigFile &config = configs->mconfigs[configs->mconfigs.size() - 1];
		int idxserver;
		if (-1 == (idxserver = this->getIdxServer(config.mport)))
		{
			// NOTE port별로 서버도 없는 상황, 아예 새롭게 만들면 됨.
			Server	server(this); // FIXME ServerManager * 가 들어갈 것으로 예상했으나 들어가지 않음. 그래서 Setservers에서 처리해줌
			server.mPort = config.mport;
			
			ServerBlock		temp;
			temp.mServerName = config.mserver_name;
			temp.mTimeOut = config.mtimeout;
			server.mServerBlocks.push_back(temp);

			LocationPath	temp2;
			temp2.mLocationPath = config.mlocation_path;
			temp2.mRoot = config.mroot;
			temp2.mIndexPages = config.mIndexPages;
			temp2.mErrorPage = config.mErrorPage;
			temp2.mAutoIndex = config.mauto_index;
			temp2.mClientMaxBodySize = config.mclient_max_body_size;

			temp2.mCgiExtensions = config.mcgi_extension;
			temp2.mMethods = config.m_method;
			server.mServerBlocks[0].mLocationPaths.push_back(temp2);

			this->mServers.push_back(server);
			configs->mconfigs.pop_back();
			continue;
		}
		Server&	server = this->mServers[idxserver];
		int		idxserverblock;
		if (-1 == (idxserverblock = this->getIdxServerBlock(server.mServerBlocks, config.mserver_name)))
		{
			// NOTE port는 있으나 다른 서버네임을 가지고 있음
			ServerBlock		temp;
			temp.mServerName = config.mserver_name;
			temp.mTimeOut = config.mtimeout;
			server.mServerBlocks.push_back(temp);
			
			LocationPath	temp2;
			temp2.mLocationPath = config.mlocation_path;
			temp2.mRoot = config.mroot;
			temp2.mIndexPages = config.mIndexPages;
			temp2.mErrorPage = config.mErrorPage;
			temp2.mCgiExtensions = config.mcgi_extension;
			temp2.mMethods = config.m_method;
			temp2.mAutoIndex = config.mauto_index;
			temp2.mClientMaxBodySize = config.mclient_max_body_size;
			server.mServerBlocks[server.mServerBlocks.size() - 1].mLocationPaths.push_back(temp2);
			configs->mconfigs.pop_back();
			continue;
		}
		ServerBlock&	temp = server.mServerBlocks[idxserverblock];
		int				idxlocationblock;
		if (-1 == (idxlocationblock = this->getIdxLocationPath(temp.mLocationPaths, config.mlocation_path)))
		{
			// NOTE port도 있고, 서버네임도 있으나, location이 다른 경우
			LocationPath	temp2;
			temp2.mLocationPath = config.mlocation_path;
			temp2.mRoot = config.mroot;
			temp2.mIndexPages = config.mIndexPages;
			temp2.mErrorPage = config.mErrorPage;
			temp2.mCgiExtensions = config.mcgi_extension;
			temp2.mMethods = config.m_method;
			temp2.mAutoIndex = config.mauto_index;
			temp2.mClientMaxBodySize = config.mclient_max_body_size;
			server.mServerBlocks[server.mServerBlocks.size() - 1].mLocationPaths.push_back(temp2);
			configs->mconfigs.pop_back();
			continue;
		}
		// NOTE 세개가 모두 같은 경우는 에러처리한다.
		configs->mconfigs.pop_back();
		return (-1);
	}
	return (1);
}

int ServerManager::getIdxServer(int port)
{
	if (this->mServers.size() == 0)
		return (-1);
	for (size_t i = 0; i < mServers.size(); i++)
	{
		if (this->mServers[i].mPort == port)
			return (i);
	}
	return (-1);
}

int ServerManager::getIdxServerBlock(std::vector<ServerBlock> &serverBlocks, std::string serverName)
{
	if (serverBlocks.size() == 0)
		return (-1);
	for (size_t i = 0; i < serverBlocks.size(); i++)
	{
		ServerBlock temp = serverBlocks[i];
		if (temp.mServerName.compare(serverName) == 0)
			return (i);
	}
	return (-1);
}

int	ServerManager::getIdxLocationPath(std::vector<LocationPath>& locationPaths, Path locationPath)
{
	if (locationPaths.size() == 0)
		return (-1);
	for (size_t i = 0; i < locationPaths.size(); i++)
	{
		LocationPath &temp = locationPaths[i];
		if (temp.mLocationPath == locationPath)
			return (i);
	}
	return (-1);
}

int	ServerManager::showServers(void)
{
	for (size_t i = 0; i < this->mServers.size(); i++)
	{
		Server &temp = this->mServers[i];
		std::cout << "port: " << temp.mPort << std::endl;
		for (size_t j = 0; j < temp.mServerBlocks.size(); j++)
		{
			ServerBlock temp2 = temp.mServerBlocks[j];
			std::cout << "server_name: " << temp2.mServerName << std::endl;
			std::cout << "timeout: " << temp2.mTimeOut << std::endl;

			std::cout << temp2.mServerName << ": " << "locationPathsize: " << temp2.mLocationPaths.size() << std::endl;
			for (size_t k = 0; k < temp2.mLocationPaths.size(); k++)
			{
				LocationPath temp3 = temp2.mLocationPaths[k];
				std::cout << "mlocationpath: " << temp3.mLocationPath.getPath() << std::endl;
				std::cout << "error_page: " << temp3.mErrorPage.getPath() << std::endl;
				std::cout << "root: " << temp3.mRoot.getPath() << std::endl;
				std::cout << "auto_index: " << temp3.mAutoIndex << std::endl;
				std::cout << "client_max_body_size: " << temp3.mClientMaxBodySize << std::endl;
				
				for (size_t l = 0; l < temp3.mIndexPages.size(); l++)
				{
					std::cout << "index_pages: " << temp3.mIndexPages[l].getPath() << std::endl;	/* code */
				}
				std::cout << "method: ";
				for (size_t j = 0; j < temp3.mMethods.size(); j++)
				{
					std::cout << temp3.mMethods[j] << " ";
				}
				std::cout << std::endl;
				std::cout << "cgi_extension: ";
				for (size_t j = 0; j < temp3.mCgiExtensions.size(); j++)
				{
					std::cout << temp3.mCgiExtensions[j] << " ";
				}
				std::cout << std::endl;
				std::cout << "------------------------------ locationPath" << std::endl;
			}
			std::cout << "============================== server name" << std::endl;
		}
		std::cout << "=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- port" << std::endl;	
	}
	return (1);
}

int						ServerManager::setServers(void)
{
	for (size_t i = 0; i < this->mServers.size(); i++)
	{
		Server&	server = this->mServers[i];
		server.SetSocket("0.0.0.0", server.mPort);
		server.mManager = this; 
		server.mConnections[server.mSocket] = Connection(server.mSocket, "0.0.0.0", server.mPort);
	}
	return (1);
}

std::vector<Server>&	ServerManager::GetServers(void)
{
	return (this->mServers);
}

void					ServerManager::closeOldConnection(std::vector<Server>::iterator server_it)
{
	std::map<int, Connection>::iterator	it = server_it->mConnections.begin();
	while (it != server_it->mConnections.end())
	{
		std::map<int, Connection>::iterator	it2 = it++;
		int fd = it2->first;
		if (it2->second.getSocket() == server_it->mSocket)
		{
			continue ;
		}
		if (it2->second.isKeepConnection() == false && (FD_ISSET(fd, &this->mReadCopyFds) == 0))
		{
			std::cout << "closeOldconnection: " << fd << std::endl;
			server_it->closeConnection(it2->second.getSocket());
			return ;
		}
	}
	return ;
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