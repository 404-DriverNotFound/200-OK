#include "Server.hpp"
#include "ServerManager.hpp" // NOTE 상호참조 문제를 해결하기 위해서!

LocationPath::LocationPath() : mlocationPath(), mroot(), merror_page("error.html")
{
	Path temp("index.html");
	this->mindex_pages.push_back(temp);
}

LocationPath::~LocationPath()
{

}

LocationPath::LocationPath(const LocationPath &ref)
{
	*this = ref;
}

LocationPath&	LocationPath::operator=(const LocationPath &ref)
{
	if (this == &ref)
		return (*this);
	this->merror_page = ref.merror_page;
	this->mindex_pages = ref.mindex_pages;
	this->mlocationPath = ref.mlocationPath;
	this->mroot = ref.mroot;
	return (*this);
}

ServerBlock::ServerBlock() : mserverName("localhost"), mlocationPaths()
{

}

ServerBlock::~ServerBlock()
{

}

ServerBlock::ServerBlock(const ServerBlock &ref)
{
	*this = ref;
}

ServerBlock&	ServerBlock::operator=(const ServerBlock &ref)
{
	if (this == &ref)
		return (*this);
	this->mlocationPaths = ref.mlocationPaths;
	this->mserverName = ref.mserverName;
	return (*this);
}

Server::Server() : mport(8000), mserverBlocks(), m_manager(NULL), msocket(0)
{
}

Server::Server(ServerManager *servermanager) : mport(8000), mserverBlocks(), m_manager(servermanager)
{

}

Server::~Server()
{
	
}

Server::Server(const Server &ref)
{
	*this = ref;
}

Server&	Server::operator=(const Server &ref)
{
	if (this == &ref)
		return (*this);
	this->mport = ref.mport;
	this->mserverBlocks = ref.mserverBlocks;
	return (*this);
}


int 	Server::SetSocket(std::string ip, uint16_t port)
{
	if ((this->msocket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
			return (-1);
	sockaddr_in sockaddr;
	ft::memset((sockaddr_in *)&sockaddr, 0, sizeof(sockaddr_in));
	sockaddr.sin_family = AF_INET;
	// sockaddr.sin_addr.s_addr = inet_addr(ip.c_str()); // REVIEW 위 아래 어떤 것으로 쓸지
	sockaddr.sin_addr.s_addr = INADDR_ANY;
	sockaddr.sin_port = htons(this->mport); // htons is necessary to convert a number to

	int opt = 1; // 소켓을 재사용하려면 희한하게도 1로 설정해야한다.
	setsockopt(this->msocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	if (bind(this->msocket, (struct sockaddr*)&sockaddr, sizeof(sockaddr)) < 0)
		return (-1);
	if (listen(this->msocket, 10) < 0)
	{
		cout << "this->msocket: " << this->msocket << endl;
		return (-1);
	}
	return (-1);
}

const int&	Server::get_m_fd(void) const{ return (this->msocket); }

void	Server::run()
{
	std::map<int, Connection>::iterator it = m_connections.begin();
	while (it != m_connections.end())
	{
		std::map<int, Connection>::iterator it2 = it++;
		int fd = this->msocket;

		if (it2->second.get_m_fd() == fd)
		{
			continue ;
		}
		try
		{
			// if (hasSendWork(it2->second))
			// {
			// 	runSend(it2->second);
			// 	continue ;
			// }
			// if (hasExecuteWork(it2->second))
			// {
			// 	runExecute(it2->second);
			// 	continue ;
			// }
			if (hasRequest(it2->second))
			{
				// runRecvAndSolve(it2->second);
			}
		}
		// catch (Server::IOError& e)
		// {
		// 	ft::log(ServerManager::log_fd, ft::getTimestamp() + e.location() + std::string("\n"));
		// 	closeConnection(fd);
		// }
		catch (const std::exception& e)
		{
			// ft::log(ServerManager::log_fd, ft::getTimestamp() + "detected some error" + std::string("\n"));
			closeConnection(fd);				
		}
	}
	if (hasNewConnection())
	{
		if (m_connections.size() >= (INIT_FD_MAX / m_manager->GetServers().size()))
		{
			int fd = getUnuseConnectionFd();
			if (fd == -1)
				return ;
			closeConnection(fd);
		}
		
		if (false == acceptNewConnection())
		{
			cout << "accpetNewconnection(): Fail" << endl;
			// reportCreateNewConnectionLog();
		}
		else
		{
			cout << "accpetNewconnection(): Success" << endl;
		}
	}
	cout << this->mport << "'s connection_size: "<< m_connections.size() << endl; 
}

bool					Server::hasRequest(const Connection& connection)
{
	if (FD_ISSET(connection.get_m_fd(), &(this->m_manager->GetReadCopySet()))) // REVIEW	request의 phase도 함께 확인해야할 수도 있을 듯
	{
		std::cout << "client(" << connection.get_m_fd() << ") : has request" << std::endl;
		return (true);
	}
	else
	{
		return (false);
	}
}

bool						Server::hasNewConnection()
{
	if (FD_ISSET(this->msocket, &(this->m_manager->GetReadCopySet())))
	{
		cout << "this->msocket: " << this->msocket << endl;
		return (true);
	}
	else
	{
		return (false);
	}
}

bool						Server::acceptNewConnection()
{
	int client_socket;
	sockaddr_in sockaddr;
	socklen_t socketlen;
	socketlen = sizeof(struct sockaddr);
	client_socket = accept(this->msocket, (struct sockaddr*)&sockaddr, (socklen_t*)&socketlen);
	if (client_socket == -1)
	{
		std::cerr << "Could not create socket." << std::endl;
		return (false);
	}
	else
	{
		return (true);
		// // STUB nc localhost 8000 BLOCK 처리 방지
		// int flags = 0;
		// // flags = fcntl(client_socket, F_GETFL, 0);
		// // flags |= O_NONBLOCK;
		// // fcntl(client_socket, F_SETFL, flags);

		// int bytesRead;
		// bytesRead = BUFFER_SIZE - 1;
		// std::cout << "connected client fd: " << client_socket << std::endl;
		// char buffer[BUFFER_SIZE * 10];
		// char *buffer_pointer = buffer;

		// while (bytesRead == BUFFER_SIZE - 1)
		// {
		// 	bytesRead = read(client_socket, buffer_pointer, BUFFER_SIZE - 1); // request 를 여기서 받아서..
		// 	/***************************************************
		// 	 * 영환이가 버퍼를 받아 코드에서 적용시키는 영역
		// 	 ***************************************************/
		// 	if (bytesRead == -1)
		// 		std::cerr << "Could not read request." << std::endl;
		// 	buffer_pointer += bytesRead;
		// }
		// if ((flags & O_NONBLOCK) != O_NONBLOCK)
		// {
		// 	buffer_pointer[bytesRead] = '\0';
		// 	cout << buffer << endl;
		// }

		// // NOTE Http 파싱 파트
		// // if (bytesRead != -1)
		// // {
		// // 	//	STUB : HttpMessageRequest
		// // 	HttpMessageRequest	request(buffer);
		// // 	request.Parser(); // request 를 parsing 한 후,

		// // 	//	STUB : HttpMessageResponse
		// // 	HttpMessageResponse	response(request); // reponse 를 정리한다.
		// // 	response.SetMessage();

		// // 	//	STUB : Send a message to the connection
		// // 	int len = response.GetMessage().size();
		// // 	int ret = send(client_socket, response.GetMessage().c_str(), len, 0);
		// // }
	}
	FD_SET(client_socket, &(this->m_manager->GetReadSet()));
	this->m_connections[client_socket] = Connection(client_socket, "", this->mport);
	// close(client_socket); // NOTE 이제 keep-alive로 관리
	return (true);
}


int				Server::getUnuseConnectionFd()
{
	std::map<int, Connection>::iterator it = m_connections.begin();
	cout << "getUnuseConnectionFd_connection size: "<< m_connections.size() << endl; 
	while (it != m_connections.end())
	{
		std::map<int, Connection>::iterator it2 = it++;
		int fd = it2->first;
		if (it2->second.get_m_fd() == fd)
			continue ;
		if ( (FD_ISSET(it2->second.get_m_fd(), &(this->m_manager->GetReadCopySet())) == false) &&
				it2->second.isKeepConnection() == false)
		{
			cout << "it2->second.get_m_fd() " << it2->second.get_m_fd() << endl;
			return (it2->second.get_m_fd());
		}
	}
	return (-1);
}

void			Server::closeConnection(int client_fd)
{
	close(client_fd);

	std::map<int, Connection>::iterator it = m_connections.begin();
	while (it != m_connections.end())
	{
		std::map<int, Connection>::iterator it2 = it++;
		if (it2->second.get_m_fd() == client_fd)
		{
			FD_CLR(client_fd, &(this->m_manager->GetReadSet()));
			m_connections.erase(it2);
			return ;
		}
	}
}
