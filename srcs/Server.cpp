#include "Server.hpp"
#include "ServerManager.hpp" // NOTE 상호참조 문제를 해결하기 위해서!
#include "Response.hpp"


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

ServerBlock::ServerBlock() : mserverName("localhost"), mlocationPaths(), mauto_index(false), mtimeout(0)
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
	this->mtimeout = ref.mtimeout;
	this->mauto_index = ref.mauto_index;
	return (*this);
}

Server::Server(void)
	: mport(8000)
	// , mserverBlocks()
	, m_manager(NULL)
	, msocket(0)
	// , mPhase(READY)
{
}

Server::Server(ServerManager *serverManager)
	: mport(8000)
	// , mserverBlocks()
	, m_manager(serverManager)
	, msocket(0)
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
	this->msocket = ref.msocket;
	this->m_connections = ref.m_connections;
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

std::vector<ServerBlock>&	Server::get_m_serverBlocks(void){ return (this->mserverBlocks);}

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
		create_statuspage_Response(connection, status_code);
		connection.SetStatus(Connection::SEND_READY);
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
	}

	try
	{
		Request& request = *connection.get_m_request();
		if (request.GetPhase() == Request::COMPLETE)
		{
		// 	writeCreateNewRequestLog(request);
		// 	connection.set_m_status(Connection::ON_EXECUTE); //REVIEW 이게 맞나?
			solveRequest(connection, *connection.get_m_request());
			return (true);
		}
		return (false);
	}
	catch (int status_code)
	{
		std::cout << "status code : " << status_code << std::endl;
		create_statuspage_Response(connection, status_code);
		connection.SetStatus(Connection::SEND_READY);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
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
	value = connection.get_m_request()->GetPhase();
	
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
		// cout << connection.get_m_response()->getResponse() << endl;
		send_complete = true;
	}
	closeConnection(connection.get_m_fd());
	return (send_complete);
}

void		Server::create_statuspage_Response(Connection &connection, int status_code)
{
		// NOTE connection의 response에 갖다 붙이기.
		if (connection.get_m_response() != NULL)
		{
			cout << "말이 안되는 상황" << endl;
		}
		else
		{
			if (status_code != 0)
			{
				connection.set_m_response(new Response(&connection, status_code));
				Response *response = connection.get_m_response();
				response->set_m_headers("Server", "YKK_webserv");
				response->set_m_headers("Date", ft::getCurrentTime());
				response->set_m_headers("Content-Type", "text/html");
				std::string errorpage_body = Response::makeErrorPage(status_code);
				response->set_m_headers("Content-Length", std::to_string(errorpage_body.size()));
				response->set_m_body(errorpage_body);
				return ;
			}
		}
}

void		Server::get_htmlpage_Response(Connection &connection, std::string uri_plus_file, TYPE_HTML type)
{
	connection.set_m_response(new Response(&connection, 0));
	Response *response = connection.get_m_response();
	response->set_m_headers("Server", "webserv");
	response->set_m_headers("Content-Type", "text/html");
	response->set_m_headers("Connection", "keep-alive");
	response->set_m_headers("Date", ft::getCurrentTime().c_str());

	char buffer[BUFIZE_HTMLFILE];
	int fd;
	if (type == ERROR_HTML)
	{
		cout << "error_html :" << uri_plus_file << endl;
		int start = uri_plus_file.rfind('/') + 1;
		uri_plus_file = uri_plus_file.substr(start, uri_plus_file.size());
		fd = open(uri_plus_file.c_str(), O_RDONLY);
		if (fd == -1)
		{
			fd = open("./error.html", O_RDONLY);
		}
	}
	else if (type == INDEX_HTML)
	{
		cout << "index_html :" << uri_plus_file << endl;
		fd = open(uri_plus_file.c_str(), O_RDONLY);
		if (fd == -1)
		{
			fd = open("./index.html", O_RDONLY);
		}

	}
	int ret = read(fd, buffer, BUFIZE_HTMLFILE);
	buffer[ret] = '\0';
	std::string body = buffer;
	response->set_m_body(body);
	if (fd != -1)
		close(fd);
}


void	Server::solveRequest(Connection& connection, Request& request)
{
	cout << "solveRequest()" << endl;
	char absolute_path[255];
	getcwd(absolute_path, 255);
	std::string target_uri(absolute_path);
	
	std::string hostname;
	std::map<std::string, std::string>::iterator it;
	it = request.get_m_headers().find("host");
	if (it == request.get_m_headers().end())
		hostname = "localhost";
	else
		hostname = it->second;
	// FIXME @ykoh 이런 뉘앙스가 들어가야함.
	// hostname = request.get_m_host();

	// NOTE 무작위 값이 들어감
	std::vector<ServerBlock>::iterator serverblock = return_iterator_serverblock(this->get_m_serverBlocks(), hostname);
	std::vector<LocationPath>::iterator locationPath = return_iterator_locationpathlocationPath(serverblock->mlocationPaths, request.get_m_uri());

	target_uri += locationPath->mroot.getPath();
	target_uri += request.get_m_uri();
	// 여기까지 왔으면, 내가 요청하고자하는 자원의 위치는 정해짐. -> 폴더이면, autoindex와 index_page를 찾거나, 파일이면, Method를 적용함.
	// target_uri.pop_back();
	cout << "target_uri: " << target_uri << endl;
	
	// DIR *dir = opendir(target_uri.c_str()); closedir(dir);
	if (ft::isFilePath(target_uri))
	{
		if (ft::access(target_uri) == false)
		{
			throw 404;
		}
		else
		{
			// NOTE 파일 경로이고, 존재함.
			connection.set_m_response(new Response(&connection, 200));
		}
	}
	else
	{
		if (ft::access(target_uri) == true) // NOTE 있는 폴더 경로에 접근 했을 때, index,html or autoindex
		{
			for (int i = 0; i < locationPath->mindex_pages.size(); i++)
			{
				std::string uri_indexhtml(target_uri);
				uri_indexhtml += locationPath->mindex_pages[i].getPath();
				if (ft::isFilePath(uri_indexhtml) == true && ft::access(uri_indexhtml) == true)
				{
					get_htmlpage_Response(connection, uri_indexhtml, INDEX_HTML);
					connection.SetStatus(Connection::SEND_READY);
					return ;
				}
			}

			// NOTE index_pages 으로 찾아봐도 해당 페이지가 없음. 에러페이지 혹은 오토인덱스 페이지를 보여줘야함.
			if (serverblock->mauto_index == true)
			{
				cout << "serverblock autoindex: " << serverblock->mauto_index << endl;
				executeAutoindex(connection, *connection.get_m_request(), target_uri);
				return ;
			}
			else
			{
				throw 403;
			}
		}
		else // NOTE 없는 폴더 경로에 접근 했을 때, error.html 보여주기
		{
			std::string uri_errorhtml(target_uri);
			uri_errorhtml += locationPath->merror_page.getPath();
			// uri_errorhtml.clear();
			// uri_errorhtml = "/Users/yunslee/webserv_200/flabc/error.html";
			get_htmlpage_Response(connection, uri_errorhtml, ERROR_HTML);
			connection.SetStatus(Connection::SEND_READY);
			return ;
		}
	}
}

std::vector<ServerBlock>::iterator Server::return_iterator_serverblock(std::vector<ServerBlock> &serverblocks, std::string hostname)
{
	std::vector<ServerBlock>::iterator it = serverblocks.begin();
	while (it != serverblocks.end())
	{
		if (hostname == it->mserverName)
		{
			return (it);
		}
		it++;
	}
	it--; // NOTE 맨 뒤에 있는 serverBlock을 default로 잡음
	return (it);
}

std::vector<LocationPath>::iterator Server::return_iterator_locationpathlocationPath(std::vector<LocationPath> &locationpaths, std::string locationpath_str)
{
	std::vector<LocationPath>::iterator it = locationpaths.begin();
	while (it != locationpaths.end())
	{
		if (locationpath_str == it->mlocationPath.getPath())
		{
			return (it);
		}
		it++;
	}
	it--; // NOTE 맨 뒤에 있는 locationPath을 default로 잡음
	return (it);
}

// bool Server::isHostname_IN_server_name(std::vector<ServerBlock> &serverblocks, std::string hostname)
// {
// 	std::vector<ServerBlock>::iterator it = serverblocks.begin();
// 	while (it != serverblocks.end())
// 	{
// 		if (hostname == it->mserverName)
// 			return (true);
// 		it++;
// 	}
// 	return (false);
// }



void	Server::executeAutoindex(Connection& connection, const Request& request, std::string uri_copy)
{
	connection.set_m_response(new Response(&connection, 200, ft::makeAutoindexHTML(uri_copy)));
	Response *response = connection.get_m_response();
	response->set_m_headers("Server", "webserv");
	response->set_m_headers("Content-Type", "text/html");
	response->set_m_headers("Connection", "keep-alive");
	response->set_m_headers("Date", ft::getCurrentTime().c_str());

	connection.SetStatus(Connection::SEND_READY);
}


void		Server::executeGet(Connection& connection, const Request& request)
{

}

void		Server::executeHead(Connection& connection, const Request& request)
{

}

void		Server::executePost(Connection& connection, const Request& request)
{

}

void		Server::executePut(Connection& connection, const Request& request)
{

}

void		Server::executeDelete(Connection& connection, const Request& request)
{

}

void		Server::executeOptions(Connection& connection, const Request& request)
{

}

void		Server::executeTrace(Connection& connection, const Request& request)
{

}



const char* Server::ClientServerClose::what() const throw(){ return ("Client close Server!"); }

