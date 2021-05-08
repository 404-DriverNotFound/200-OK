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
	// ANCHOR 1단계 parsing 전처리단계
	ConfigFiles configfiles(configuration_file_path.c_str());
	// ANCHOR 2단계 parsing
	this->SetServers(&configfiles); // NOTE 값을 확인하고싶으면, this->ShowServers(); 

	// ANCHOR 3단계 parsing -> port에 대해서 listen 함수까지 호출함.
	this->SetServers();

	// writeCreateServerLog();
}

void		ServerManager::runServer(void)
{
	// signal(SIGINT, changeSignal);

	cout << "Start runServer" << endl;
	struct timeval	timeout; memset(&timeout, 0, sizeof(struct timeval));
	// g_live = true;
	initMaxFd();
	for (std::vector<Server>::iterator it = m_servers.begin() ; it != m_servers.end() ; ++it)
	{
		FD_SET(it->get_m_fd(), &m_read_set);
	}
	while (true	/* g_live */)
	{
		timeout.tv_sec = 2;
		fdCopy(ALL_SET);
		resetMaxFd();
		int	cnt = select(m_max_fd + 1, &m_read_copy_set, &m_write_copy_set, NULL, &timeout);
		if (cnt < 0)
		{
			std::cout << "Select error\n";
			throw std::runtime_error("select error");
			break ;
		}
		else if (cnt == 0)
		{
			std::cout << "timeout\n";
			continue ;
		}
		else if (cnt > 0)
		{
			std::cout << "Received connection\n";
		}

		// writeServerHealthLog();
		for (size_t i = 0; i < this->m_servers.size(); i++) // NOTE yunslee port별로 ISSET 확인하기 위해서 반복문 씀
		{
			Server &server = this->m_servers[i];
			if (FD_ISSET(server.msocket, &m_read_copy_set))
			{
				// std::cerr << "ret is " << ret << std::endl;
				int client_socket;
				sockaddr_in sockaddr;
				socklen_t socketlen;
				socketlen = sizeof(struct sockaddr);
				client_socket = accept(server.msocket, (struct sockaddr*)&sockaddr, (socklen_t*)&socketlen);
				if (client_socket == -1)
					std::cerr << "Could not create socket." << std::endl;
				else
				{
					int bytesRead;
					bytesRead = BUFFER_SIZE - 1;
					std::cout << "connected client fd: " << client_socket << std::endl;
					char buffer[BUFFER_SIZE * 10];
					char *buffer_pointer = buffer;

					while (bytesRead == BUFFER_SIZE - 1)
					{
						bytesRead = read(client_socket, buffer_pointer, BUFFER_SIZE - 1); // request 를 여기서 받아서..
						/***************************************************
						 * 영환이가 버퍼를 받아 코드에서 적용시키는 영역
						 ***************************************************/
						if (bytesRead == -1)
							std::cerr << "Could not read request." << std::endl;
						buffer_pointer += bytesRead;
					}
					buffer_pointer[bytesRead] = '\0';
					cout << buffer << endl;

					// NOTE Http 파싱 파트
					// if (bytesRead != -1)
					// {
					// 	//	STUB : HttpMessageRequest
					// 	HttpMessageRequest	request(buffer);
					// 	request.Parser(); // request 를 parsing 한 후,

					// 	//	STUB : HttpMessageResponse
					// 	HttpMessageResponse	response(request); // reponse 를 정리한다.
					// 	response.SetMessage();

					// 	//	STUB : Send a message to the connection
					// 	int len = response.GetMessage().size();
					// 	int ret = send(client_socket, response.GetMessage().c_str(), len, 0);
					// }
				}
				// STUB Close the connections
				close(client_socket);
			}
		}


		for (std::vector<Server>::iterator it = m_servers.begin() ; it != m_servers.end() ; ++it)
		{
		// 	it->run();
		// 	closeOldConnection(it);
		}

	}
	exitServer("server exited.\n");
}

void		ServerManager::set_m_max_fd(const int& fd)
{
	m_max_fd = fd;
}

void		ServerManager::initMaxFd()
{
	set_m_max_fd(INIT_FD_MAX);
}

void		ServerManager::resetMaxFd()
{
	// STUB 하향식. 연결된 fd가 많으면, 이 방법이 더 효율적임
	// for (int i = 512; i >= 0; --i)
	// {
	// 	if (fdIsset(i, READ_SET) || fdIsset(i, WRITE_SET))
	// 	{
	// 		m_max_fd = i;
	// 		break ;
	// 	}
	// }

	// STUB 상향식. 연결된 fd가 적으면, 이 방법이 더 효율적임
	for (int i = 0; i <= 512; i++)
	{
		if (fdIsset(i, READ_SET) == false && fdIsset(i, WRITE_SET) == false)
		{
			m_max_fd = i;
			break ;
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



//ANCHOR yunslee line: 160~332 추가

int ServerManager::SetServers(ConfigFiles *configs)
{
	while (configs->mconfigs.size() != 0)
	{
		ConfigFile &config = configs->mconfigs[configs->mconfigs.size() - 1];
		int idxserver;
		if (-1 == (idxserver = this->GetIdxServer(config.mport)))
		{
			// NOTE port별로 서버도 없는 상황, 아예 새롭게 만들면 됨.
			Server server;
			server.mport = config.mport;
			
			ServerBlock temp;
			temp.mserverName = config.mserver_name;
			server.mserverBlocks.push_back(temp);

			LocationPath temp2;
			temp2.mlocationPath = config.mlocation_path;
			temp2.mroot = config.mroot;
			temp2.mindex_pages = config.mindex_pages;
			temp2.merror_page = config.merror_page;
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
			server.mserverBlocks.push_back(temp);
			
			LocationPath temp2;
			temp2.mlocationPath = config.mlocation_path;
			temp2.mroot = config.mroot;
			temp2.mindex_pages = config.mindex_pages;
			temp2.merror_page = config.merror_page;
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
		server.SetSocket("127.0.0.1", server.mport);
	}
	return (1);
}


// REVIEW Socket 클래스에서 단순히 int msocket(fd)를 쓰는 것으로 변경되었으므로 다시한번 확인이 필요함.
void ServerManager::SetFdMax()
{
	for (size_t i = 0; i < this->m_servers.size(); i++)
	{
		Server &server =this->m_servers[i];
		if (this->m_max_fd < server.msocket)
			this->m_max_fd = server.msocket;
	}
}

int ServerManager::GetFdMax()
{
	this->SetFdMax();
	return (this->m_max_fd);
}