#include "Server.hpp"
#include "ServerManager.hpp" // NOTE 상호참조 문제를 해결하기 위해서!
#include "Response.hpp"

extern char**	g_env;

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
	sockaddr.sin_addr.s_addr = ft::ft_inet_addr(ip.c_str()); // REVIEW 위 아래 어떤 것으로 쓸지
	// sockaddr.sin_addr.s_addr = INADDR_ANY;
	sockaddr.sin_port = ft::ft_htons(this->mport); // htons is necessary to convert a number to

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
			if (hasSendWork(it2->second))
			{
				runSend(it2->second);
				continue ; // FIXME 어떻게 처리할지...
			}
			if (hasExecuteWork(it2->second))
			{
				runExecute(it2->second);
				continue ;
			}
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
			cout << e.what() << endl;
			// ft::log(ServerManager::log_fd, ft::getTimestamp() + "detected some error" + std::string("\n"));
			closeConnection(it2->second.get_m_fd());

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
	// cout << this->mport << "'s connection_size: "<< m_connections.size() << endl; 
}

bool					Server::hasRequest(const Connection& connection)
{
	if (FD_ISSET(connection.get_m_fd(), &(this->m_manager->GetReadCopySet()))) // REVIEW	request의 phase도 함께 확인해야할 수도 있을 듯
	{
		// std::cout << "client(" << connection.get_m_fd() << ") : has request" << std::endl;
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
		if ( (FD_ISSET(it2->second.get_m_fd(), &(this->m_manager->GetReadCopySet())) <= 0) &&
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
		std::cout << "status code : " << status_code << std::endl;
		create_errorpage_Response(connection, status_code);
		connection.SetStatus(Connection::SEND_READY);
		// REVIEW 합의가 필요한 부분
		// delete connection.get_m_request();
		// connection.set_m_request(NULL);
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

	// const Request* request = connection.get_m_request();
	// if (request->GetPhase() == Request::COMPLETE)
	// {
	// 	writeCreateNewRequestLog(request);
	// 	connection.set_m_status(Connection::ON_EXECUTE);
	// 	solveRequest(connection, connection.get_m_request());
		// std::cout << request->get_m_content() << std::endl;
		// return (true);
	// }
	return (false);
}

void						Server::recvRequest(Connection& connection)
{
	Request*	request = connection.get_m_request();
	char		buf[BUFFER_SIZE] = { 0, };
	
	ssize_t		count = read(connection.get_m_fd(), buf, sizeof(buf));
	if (count > 0)
	{
		// REVIEW 파싱단계에서 count 변수를 사용해서 탐색 범위를 좁힐 수 있을까?
		connection.addRbufFromClient(buf, count);
		if (request->GetPhase() == Request::READY)
		{
			if (parseStartLine(connection))
			{
				request->SetPhase(Request::ON_HEADER);
			}
		}
		if (request->GetPhase() == Request::ON_HEADER)
		{
			if (parseHeader(connection))
			{
				request->SetPhase(Request::ON_BODY);
			}
		}
		if (request->GetPhase() == Request::ON_BODY)
		{
			if (isRequestHasBody(request))
			{
				if (parseBody(connection))
				{
					request->SetPhase(Request::COMPLETE);
					connection.SetStatus(Connection::SEND_READY);
					std::cout << "|" << request->getBody() << "|" << std::endl;
				}
			}
			else
			{
				request->SetPhase(Request::COMPLETE);
				connection.SetStatus(Connection::SEND_READY);
			}
		}
	}
	else
	{
		throw Server::ClientServerClose();
	}
}

bool Server::isRequestHasBody(Request* request)
{
	if (request->get_m_method() == Request::POST || request->get_m_method() == Request::PUT)
		return (true);
	else
		return (false);
}

bool						Server::parseStartLine(Connection& connection)
{
	Request*			request = connection.get_m_request();
	const std::string&	requestLine = request->get_m_origin();
	std::size_t			found;
	std::string			tmp;
	

	if (requestLine.find("\r\n") == std::string::npos)
	{
		return (false);
	}
	std::cout << "parseStartLine() called" << std::endl;
	// method 파싱
	found = requestLine.find(' ');
	if (found == std::string::npos)
	{
		throw 400;
	}
	tmp = requestLine.substr(request->GetSeek(), found - request->GetSeek());
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
	request->SetSeek(found + 1);

	// URI 파싱
	found = requestLine.find(' ', request->GetSeek());
	if (found == std::string::npos)
	{
		throw 414;
	}
	tmp = requestLine.substr(request->GetSeek(), found - request->GetSeek());
	std::cout << "\t|" << tmp << "|" << std::endl;

	request->set_m_uri(tmp);
	// TODO URI 분석 (URI 구조를 몰라서 아직 못함)
	// TODO uri 타입도 설정해주어야함
	// std::cout << "\turi : " << request->get_m_uri() << std::endl;
	request->SetSeek(found + 1);

	// http version 파싱
	found = requestLine.find("\r\n", request->GetSeek());
	if (found == std::string::npos)
	{
		throw 505;
	}
	tmp = requestLine.substr(request->GetSeek(), found - request->GetSeek());
	std::cout << "\t|" << tmp << "|" << std::endl;
	// TODO 지원하지 않는 버전 관련 부분 추가해야함
	// if (isUnsopportingVersion())
	// {
	// 	throw 505;
	// }
	request->SetSeek(found + 2);
	return (true);
}

bool						Server::parseHeader(Connection& connection)
{
	Request*		request = connection.get_m_request();
	std::size_t		found = request->get_m_origin().find("\r\n\r\n", request->GetSeek());
	if (found == std::string::npos)
	{
		return (false);
	}

	std::cout << "parseHeader() called" << std::endl;
	while (true)
	{
		std::size_t		found = request->get_m_origin().find("\r\n", request->GetSeek());
		if (found == std::string::npos)
		{
			throw 400;
		}
		std::string	line = request->get_m_origin().substr(request->GetSeek(), found - request->GetSeek());
		if (line.empty())
		{
			request->SetSeek(found + 2);
			break ;
		}
		std::cout << "\t|" << line << "|" << std::endl;

		if (request->isValidHeader(line))
		{
			request->addHeader(line);
		}
		request->SetSeek(found + 2);
	}
	return (true);
}

bool						Server::parseBody(Connection& connection)
{
	std::cout << "parseBody() called" << std::endl;
	Request*		request = connection.get_m_request();

	// NOTE chunked parsing
	if (request->get_m_transfer_type() == Request::CHUNKED)
	{
		while (true)
		{
			// hex 기다림
			std::size_t		foundHex = request->get_m_origin().find("\r\n", request->GetSeek());
			if (foundHex == std::string::npos)
			{
				return (false);
			}
			else
			{
				// 바디 기다림
				std::size_t	foundBody = request->get_m_origin().find("\r\n", foundHex + 2);
				if (foundBody == std::string::npos)
				{
					return (false);
				}
				else
				{
					std::string		hex = request->get_m_origin().substr(request->GetSeek(), foundHex - request->GetSeek());
					unsigned long	hexValue = ft::stohex(hex);
					request->SetSeek(foundHex + 2);
					std::string		body = request->get_m_origin().substr(request->GetSeek(), foundBody - request->GetSeek());
					if (hexValue != body.length())
					{
						throw 413; // REVIEW payload too large 이거 맞는지 모르겠음
					}
					else
					{
						request->addBody(body);
						if (hexValue == 0)
						{
							return (true);
						}
					}
					request->SetSeek(request->GetSeek() + hexValue + 2);
				}
			}
		}
	}
	else
	{
		// NOTE contents-length parsing
		std::map<std::string, std::string>::iterator	it = request->get_m_headers().find("content-length");
		int												contentLength = std::atoi(it->second.c_str());
		int												bodyLength = request->get_m_origin().length() - request->GetSeek();
		std::cout << contentLength << " " << bodyLength << std::endl;
		if (contentLength > bodyLength)
		{
			return (false);
		}
		else if (contentLength < bodyLength)
		{
			throw 414;
		}
		else
		{
			request->SetBody(request->get_m_origin().substr(request->GetSeek()));
			return (true);
		}
	}
}

bool						Server::hasSendWork(Connection& connection)
{
	// value = connection.get_m_request()->GetPhase();
	// TODO COMPLETE로 가정하였으나, 실제로는 request의 진행상황에 따라서 { READY, ON_HEADER, ON_BODY, COMPLETE }; 단계로 나뉨.
	if (connection.get_m_request() == NULL)
		return (false);
	Request::ePhase value;
	// value = connection.get_m_request()->COMPLETE;
	
	// char buffer[200];
	// int ret = read(connection.get_m_fd(), buffer, 200); // FIXME read는 단 한번...
	// cout <<  connection.get_m_fd() << endl;
	// cout << "ret: " << ret << endl; 
	// if (ret == 0) // NOTE 클라이언트에서 서버를 끊는 경우에만 생기는 케이스
	// {
	// 	throw Server::ClientServerClose();
	// }
	//
	if (value == connection.get_m_request()->COMPLETE)
	{
		if (FD_ISSET(connection.get_m_fd(), &(this->m_manager->GetWriteCopySet())) <= 0)
		{
			closeConnection(connection.get_m_fd());
			return (false);
		}
		return (true);
	}
	else
		return (false);
}

bool						Server::runSend(Connection& connection)
{
	int clinet_socket = connection.get_m_fd();
	Request *request = connection.get_m_request();
	char buffer[100];

	bool send_complete = false;
	if (connection.GetStatus() == Connection::SEND_READY)
	{
		write(connection.get_m_fd(), connection.get_m_response()->getResponse().c_str(), connection.get_m_response()->getResponse().size());
		cout << connection.get_m_response()->getResponse() << endl;
		send_complete = true;
	}
	// TODO solveRequest에서 진행되어야하는 부분
	// bool auto_index = true;
	// if (ft::isDirPath(request->get_m_uri()) && auto_index == true)
	// {
	// 	executeAutoindex(connection, *connection.get_m_request());
	// 	return (true);
	// }
	closeConnection(connection.get_m_fd());
	return (send_complete);
}


void	Server::executeAutoindex(Connection& connection, const Request& request)
{
	int clinet_socket = connection.get_m_fd();
	std::string total;
	std::string header;
	std::string body;

	//STUB header는 파싱된 결과로 구성할 수 있어야한다. 우선은 nginx의 autoindex request header를 보고 베낌
	header.append("HTTP/1.1 200 OK\r\n");
	header.append("Server: webserv\r\n");
	header.append("Content-Type: text/html\r\n");
	header.append("Connection: keep-alive\r\n");
	// header.append("Transfer-Encoding: chunked\r\n"); // REVIEW 시도하면 postman에서 body를 못 받음
	header.append("Date: " + ft::getCurrentTime() + "\r\n");
	header.append("\r\n");
	body = ft::makeAutoindexHTML(request.get_m_uri());
	total = header + body;
	int ret = write(clinet_socket, total.c_str(), total.size());
	cout << "write return: " << ret << endl;
	closeConnection(clinet_socket);
}


void		Server::create_errorpage_Response(Connection &connection, int status_code)
{
		// NOTE connection의 response에 갖다 붙이기.
		if (connection.get_m_response() != NULL)
		{
			cout << "말이 안되는 상황" << endl;
		}
		else
		{
			connection.set_m_response(new Response(&connection, status_code));
			Response *response = connection.get_m_response();
			response->set_m_headers("Server", "YKK_webserv");
			response->set_m_headers("Date", ft::getCurrentTime());
			response->set_m_headers("Content-Type", "text/html");
			std::string errorpage_body = Response::makeErrorPage(status_code);
			response->set_m_headers("Content-Length", std::to_string(errorpage_body.size()));
			response->set_m_body(errorpage_body);
		}
}


const char* Server::ClientServerClose::what() const throw(){ return ("Client close Server!"); }

bool		Server::hasExecuteWork(const Connection& connection) const
{
	if (connection.GetStatus() == Connection::CGI_READY ||
		connection.GetStatus() == Connection::SEND_READY || // STUB	나중에 지워야함
		connection.GetStatus() == Connection::CGI_ING)
	{
		return (true);
	}
	else
	{
		return (false);
	}
}

bool		Server::runExecute(Connection& connection)
{
	if (connection.GetStatus() == Connection::CGI_READY)
	{
		createCGIEnv(connection);
	}
	else	// CGI_ING
	{
		// executeCGI();
	}
	return (false);
}

char**		Server::createCGIEnv(const Connection& connection) const
{
	char**											ret = NULL;
	std::map<std::string, std::string>				cgiEnv;
	
	Request*										request = connection.get_m_request();
	std::map<std::string, std::string>::iterator	it = request->get_m_headers().find("authorization");
	if (it != request->get_m_headers().end())
	{
		std::size_t	found = it->first.find(" ");
		if (found != std::string::npos)
		{
			cgiEnv["AUTH_TYPE"] = it->second.substr(0, found);		// NOTE 불확실 검증필요
			cgiEnv["REMOTE_IDENT"] = it->second.substr(found + 1);	// NOTE 불확실 검증필요
			cgiEnv["REMOTE_USER"] = it->second.substr(found + 1);	// NOTE 불확실 검증필요
			
		}
	}

	it = request->get_m_headers().find("content-length");
	if (it != request->get_m_headers().end())
	{
		cgiEnv["CONTENT_LENGTH"] = it->second;
	}

	it = request->get_m_headers().find("content-type");
	if (it != request->get_m_headers().end())
	{
		cgiEnv["CONTENT_TYPE"] = it->second;
	}

	cgiEnv["GATEWAY_INTERFACE"] = "이건 Config클래스에서 가져오는데 쓸지말지 정해야함";


	cgiEnv["PATH_INFO"] = "path suffix, if appended to URL after program name and a slash.";	// NOTE /asdf 인지 asdf 인지 모르겠음
	cgiEnv["PATH_TRANSLATED"] = "corresponding full path as supposed by server, if PATH_INFO is present.";


	cgiEnv["QUERY_STRING"] = "the part of URL after ? character. The query string may be composed of *name=value pairs separated with ampersands (such as var1=val1&var2=val2...) when used to submit form data transferred via GET method as defined by HTML application/x-www-form-urlencoded.";
	cgiEnv["REMOTE_ADDR"] = "IP address of the client (dot-decimal).";
	cgiEnv["REQUEST_METHOD"] = "name of HTTP method (see above).";
	cgiEnv["REQUEST_URI"] = "requset uri name of HTTP method (see above).";
	cgiEnv["SCRIPT_NAME"] = "relative path to the program, like /cgi-bin/script.cgi.";
	cgiEnv["SERVER_NAME"] = "host name of the server, may be dot-decimal IP address.";
	cgiEnv["SERVER_PORT"] = "TCP port (decimal).";
	cgiEnv["SERVER_PROTOCOL"] = "HTTP/version.";
	cgiEnv["SERVER_SOFTWARE"] = "name/version of HTTP server.";

	try
	{
		ret = new char*[cgiEnv.size() + 1];
		int	i = 0;
		for (std::map<std::string,std::string>::iterator it = cgiEnv.begin(); it != cgiEnv.end(); ++it)
		{
			ret[i++] = strdup((it->first + "=" + it->second).c_str());	// iostream 했더니 strdup됨
		}
		ret[i] = 0;
	}
	catch(const std::exception& e)
	{
		throw 500; // NOTE 뭘 날려야할까요 500번대 에러는 맞는데...
	}
	return (ret);
}