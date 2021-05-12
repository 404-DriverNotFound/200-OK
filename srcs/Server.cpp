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

Server::Server(void)
	: mport(8000)
	, mserverBlocks()
	, m_manager(NULL)
	, msocket(0)
	// , mPhase(READY)
{
}

Server::Server(ServerManager *serverManager)
	: mport(8000)
	, mserverBlocks()
	, m_manager(serverManager)
	// , mPhase(READY)
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
	// NOTE fcntl 부분 일단 넣어봄 왜 필요한지 여부는 아직 파악 못함
	// if (fcntl(this->msocket, F_SETFL, O_NONBLOCK) == -1)
	// {
	// 	throw std::runtime_error("FCNTL ERROR");
	// }
	return (-1);
}


const int&	Server::get_m_fd(void) const{ return (this->msocket); }

void	Server::run(void)
{
	std::map<int, Connection>::iterator	it = m_connections.begin();
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
				if (it2->second.get_m_request() == NULL)
				{
					it2->second.set_m_request(new Request());
				}
				runRecvAndSolve(it2->second);
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
	fcntl(client_socket, F_SETFL, O_NONBLOCK);
	FD_SET(client_socket, &(this->m_manager->GetReadSet()));
	FD_SET(client_socket, &(this->m_manager->GetWriteSet()));
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
			FD_CLR(client_fd, &(this->m_manager->GetWriteSet()));
			m_connections.erase(it2);
			return ;
		}
	}
}


bool			Server::runRecvAndSolve(Connection& connection)
{
	try
	{
		recvRequest(connection);
	}
	catch (int status_code)
	{
		// createResponse(connection, status_code);
		delete connection.get_m_request();
		connection.set_m_request(NULL);
		std::cout << "status code : " << status_code << std::endl;
		return (true);
	}
	// catch (const Server::IOError& e)
	// {
	// 	throw (e);
	// }
	catch (const std::exception& e)
	{
		// ft::log(ServerManager::log_fd, std::string("[Failed][Request] Failed to create request because ") + e.what());
		// createResponse(connection, 50001);
		delete connection.get_m_request();
		connection.set_m_request(NULL);
		return (true);
	}

	// const Request& request = connection.get_m_request();
	// if (request.get_m_phase() == Request::COMPLETE)
	// {
	// 	writeCreateNewRequestLog(request);
	// 	connection.set_m_status(Connection::ON_EXECUTE);
	// 	solveRequest(connection, connection.get_m_request());
	// 	return (true);
	// }
	return (false);
}

void						Server::recvRequest(Connection& connection)
{
	std::cout << "recvRequest() called" << std::endl;
	int			count = 0;
	char		buf[BUFFER_SIZE] = { 0, };
	Request*	request = connection.get_m_request();

	if (request->GetPhase() == Request::READY && hasRequest(connection) && (count = recvWithoutBody(connection, buf, sizeof(buf))) > 0)
	{
	// 	// FIXME 헤더까지 한번에 들어온다는 가정이라서 나중에 수정이 필요할 듯
	// 	// REVIEW 한번 읽을때 바디까지 들어온다는 가정이 아니면 read를 header들어올때까지 계속받는다는 이야기가되는데 그러면 select당 read 1번 룰에 위배됨.
	// 	// 별도함수로 작성한 것을 보면 header가 다읽힐때까지 read를 여러번한것이 아닐까 생각 듦.
		connection.addRbufFromClient(buf, count);
	}
	if (request->GetPhase() == Request::READY && parseStartLine(connection))
	{
		request->SetPhase(Request::ON_HEADER);
	}
	if (request->GetPhase() == Request::ON_HEADER && parseHeader(connection))
	{
		request->SetPhase(Request::ON_BODY);
	// 	if (isRequestHasBody(request))
	// 		return ;
	}
	if (request->GetPhase() == Request::ON_BODY && (count = recvBody(connection, buf, sizeof(buf))) > 0)
	{
		connection.addRbufFromClient(buf, count);
	}
	if (request->GetPhase() == Request::ON_BODY && parseBody(connection))
	{
		request->SetPhase(Request::COMPLETE);
	}

}

ssize_t						Server::recvWithoutBody(Connection& connection, void* buf, size_t nbyte)
{
	// REVIEW 한번 읽을때 바디까지 들어온다는 가정이 아니면 read를 header들어올때까지 계속받는다는 이야기가되는데 그러면 select당 read 1번 룰에 위배됨.
	// 별도함수로 작성한 것을 보면 header가 다읽힐때까지 read를 여러번한것이 아닐까 생각 듦.
	int	count = read(connection.get_m_fd(), buf, nbyte);
	return (count);
}

ssize_t						Server::recvBody(Connection& connection, void* buf, size_t nbyte)
{
	std::cout << "recvBody() called" << std::endl;
	int	count = read(connection.get_m_fd(), buf, nbyte);
	return (count);
}

bool						Server::parseStartLine(Connection& connection/*, Request& request*/)
{
	std::cout << "parseStartLine() called" << std::endl;
	Request*			request = connection.get_m_request();
	const std::string&	requestLine = connection.get_m_request()->get_m_origin();
	std::size_t			seek = 0;
	std::size_t			found;
	std::string			tmp;
	
	// method 파싱
	found = requestLine.find(' ');
	if (found == std::string::npos)
	{
		throw 400;
	}
	tmp = requestLine.substr(seek, found - seek);
	std::cout << "\t|" << tmp << "|" << std::endl;
	std::string			methodSet[8] = { "DEFAULT", "GET", "HEAD", "POST", "PUT", "DELETE", "OPTIONS", "TRACE" };

	int i;
	for (i = 0; i < 8; i++)
	{
		if (tmp.compare(methodSet[i]) == 0)
		{
			request->set_m_method(static_cast<Request::eMethod>(i));
			break ;
		}
	}
	if (i == 8)
	{
		throw 400;
	}
	// std::cout << "\tmethod : " << request->get_m_method() << std::endl;
	seek = found + 1;

	// URI 파싱
	found = requestLine.find(' ', seek);
	if (found == std::string::npos)
	{
		throw 414;
	}
	tmp = requestLine.substr(seek, found - seek);
	std::cout << "\t|" << tmp << "|" << std::endl;

	request->set_m_uri(tmp);
	// TODO URI 분석 (URI 구조를 몰라서 아직 못함)
	// TODO uri 타입도 설정해주어야함
	// std::cout << "\turi : " << request->get_m_uri() << std::endl;
	seek = found + 1;

	// http version 파싱
	found = requestLine.find("\r\n", seek);
	if (found == std::string::npos)
	{
		throw 505;
	}
	tmp = requestLine.substr(seek, found - seek);
	std::cout << "\t|" << tmp << "|" << std::endl;
	// TODO 지원하지 않는 버전 관련 부분 추가해야함
	// if (isUnsopportingVersion())
	// {
	// 	throw 505;
	// }
	seek = found + 2;

	request->SetOrigin(requestLine.substr(seek));
	return (true);
}

bool						Server::parseHeader(Connection& connection)
{
	std::cout << "parseHeader() called" << std::endl;
	Request*		request = connection.get_m_request();
	std::size_t		seek = 0;

	while (true)
	{
		std::size_t		found = request->get_m_origin().find("\r\n", seek);
		if (found == std::string::npos)
		{
			throw 400;
		}
		std::string	line = request->get_m_origin().substr(seek, found - seek);
		if (line.empty())
		{
			request->SetOrigin(request->get_m_origin().substr(found + 2));
			break ;
		}
		std::cout << "\t|" << line << "|" << std::endl;

		if (request->isValidHeader(line))
		{
			request->addHeader(line);
		}

		seek = found + 2;
	}
	return (true);
}

bool						Server::parseBody(Connection& connection)
{
	std::cout << "parseBody() called" << std::endl;
	Request*		request = connection.get_m_request();

	std::cout << "\t" << request->get_m_origin() << std::endl;
	// TODO chunked parsing

	return (true);
}