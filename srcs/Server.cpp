#include "Server.hpp"
#include "ServerManager.hpp" // NOTE 상호참조 문제를 해결하기 위해서!
#include "Response.hpp"

extern char**	gEnv;
extern int		gTotalClients;

int				Server::SetSocket()
{
	if ((this->mSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		return (-1);
	}
	sockaddr_in	sockaddr;
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_addr.s_addr = inet_addr(this->mHost.c_str()); // REVIEW 위 아래 어떤 것으로 쓸지
	sockaddr.sin_port = htons(this->mPort); // htons is necessary to convert a number to

	int opt = 1; // 소켓을 재사용하려면 희한하게도 1로 설정해야한다.
	setsockopt(this->mSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	if (bind(this->mSocket, reinterpret_cast<struct sockaddr*>(&sockaddr), sizeof(sockaddr)) < 0)
	{
		throw (static_cast<const std::string>("bind() error"));
	}
	if (listen(this->mSocket, 10000) < 0)
	{
		throw (static_cast<const std::string>("listen() error"));
	}
	// NOTE fcntl 부분 일단 넣어봄 왜 필요한지 여부는 아직 파악 못함
	// if (fcntl(this->mSocket, F_SETFL, O_NONBLOCK) == -1)
	// {
	// 	throw std::runtime_error("FCNTL ERROR");
	// }
	return (-1);
}

int				Server::getUnuseConnectionFd()
{
	std::map<int, Connection>::iterator it = mConnections.begin();
	// cout << "getUnuseConnectionFd_connection size: "<< mConnections.size() << endl; 
	while (it != mConnections.end())
	{
		std::map<int, Connection>::iterator it2 = it++;
		int fd = it2->first;
		if (it2->second.getSocket() == fd)
			continue ;
		if ((FD_ISSET(it2->second.getSocket(), &(this->mManager->GetReadCopyFds())) == 0) &&
				it2->second.isKeepConnection() == false)
		{
			// cout << "it2->second.getSocket() " << it2->second.getSocket() << endl;
			return (it2->second.getSocket());
		}
	}
	return (-1);
}

void			Server::closeConnection(int clientFd)
{
	close(clientFd);
	mManager->ClrReadFds(clientFd);
	mManager->ClrReadCopyFds(clientFd);
	mManager->ClrWriteFds(clientFd);
	mManager->ClrWriteCopyFds(clientFd);
	// mManager->SetTotalClients(mManager->GetTotalClients() - 1);
	gTotalClients--;

	// FD_CLR(clientFd, &(this->mManager->GetReadFds()));
	// FD_CLR(clientFd, &(this->mManager->GetWriteFds()));
	// FD_CLR(clientFd, &(this->mManager->GetReadFds()));
	// FD_CLR(clientFd, &(this->mManager->GetWriteCopyFds()));

	std::map<int, Connection>::iterator it = mConnections.begin();
	while (it != mConnections.end())
	{
		std::map<int, Connection>::iterator it2 = it++;
		if (it2->second.getSocket() == clientFd)
		{
			std::cerr << REDB "[" << ft::getCurrentTime() << "][connection]" << "[DISCONNECTED]" << "[" << clientFd << "]" << NC << std::endl;
			mConnections.erase(it2);
			return ;
		}
	}
}

void			Server::recvRequest(Connection& connection)
{
	Request*	request = connection.getRequest();
	char		buf[BUFFER_SIZE] = { 0, };
	
	ssize_t		count = read(connection.getSocket(), buf, sizeof(buf));
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
					// FD_SET(connection.getSocket(), &(this->mManager->GetWriteFds()));
					// FD_SET(connection.getSocket(), &(this->mManager->GetWriteCopyFds()));
					mManager->SetWriteFds(connection.getSocket());
					mManager->SetWriteCopyFds(connection.getSocket());
				}
			}
			else
			{
				request->SetPhase(Request::COMPLETE);
				mManager->SetWriteFds(connection.getSocket());
				mManager->SetWriteCopyFds(connection.getSocket());
				// FD_SET(connection.getSocket(), &(this->mManager->GetWriteFds()));
				// FD_SET(connection.getSocket(), &(this->mManager->GetWriteCopyFds()));
			}
		}
	}
	else
	{
		throw Server::IOError();
	}
}

bool			Server::isRequestHasBody(Request* request)
{
	if (request->GetMethod().compare("POST") == 0 || request->GetMethod().compare("PUT") == 0)
	{
		return (true);
	}
	else
	{
		return (false);
	}
}

bool			Server::parseStartLine(Connection& connection)
{
	Request*			request = connection.getRequest();
	const std::string&	requestLine = request->get_m_origin();
	std::size_t			found;
	std::string			tmp;
	

	if (requestLine.find("\r\n") == std::string::npos)
	{
		return (false);
	}
	// method 파싱
	found = requestLine.find(' ');
	if (found == std::string::npos)
	{
		throw 400;
	}
	tmp = requestLine.substr(request->GetSeek(), found - request->GetSeek());
	request->SetMethod(tmp);
	request->SetSeek(found + 1);

	// URI 파싱
	found = requestLine.find(' ', request->GetSeek());
	if (found == std::string::npos)
	{
		throw 414;
	}
	tmp = requestLine.substr(request->GetSeek(), found - request->GetSeek());

	// ANCHOR URI 분석 (URI 구조를 몰라서 아직 못함) 작업중
	request->ParseURI(tmp);
	request->SetSeek(found + 1);


	// http version 파싱
	found = requestLine.find("\r\n", request->GetSeek());
	if (found == std::string::npos)
	{
		throw 505;
	}
	tmp = requestLine.substr(request->GetSeek(), found - request->GetSeek());
	request->ParseVersion(tmp);
	request->SetSeek(found + 2);
	return (true);
}

bool			Server::parseHeader(Connection& connection)
{
	Request*		request = connection.getRequest();
	std::size_t		found = request->get_m_origin().find("\r\n\r\n", request->GetSeek());
	if (found == std::string::npos)
	{
		return (false);
	}

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

		request->addHeader(line);
		request->SetSeek(found + 2);
	}
	return (true);
}

bool			Server::parseBody(Connection& connection)
{
	Request*		request = connection.getRequest();

	// NOTE chunked parsing
	if (request->GetTransferType() == Request::CHUNKED)
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
		// NOTE content-length parsing
		std::map<std::string, std::string>::iterator	it = request->GetHeaders().find("content-length");
		if (it == request->GetHeaders().end())
		{
			throw 411;
		}
		int												contentLength = std::atoi(it->second.c_str());
		int												bodyLength = request->get_m_origin().length() - request->GetSeek();
		// std::cout << contentLength << " " << bodyLength << std::endl;
		if (contentLength > bodyLength)
		{
			return (false);
		}
		else if (contentLength < bodyLength)
		{
			throw 413;
		}
		else
		{
			request->SetBody(request->get_m_origin().substr(request->GetSeek()));
			return (true);
		}
	}
}

void			Server::createResponseStatusCode(Connection &connection, int statusCode)
{
	if (connection.getResponse() != NULL)
	{
		delete connection.getResponse();
		connection.setResponse(NULL);
	}
	// createResponse_statuscode
	std::map<int, std::string> &status_map = Response::mStatusMap;
	std::map<int, std::string>::iterator it;
	if ((it = status_map.find(statusCode)) == status_map.end())
	{
		statusCode = 0;
	}
	connection.setResponse(new Response(&connection, statusCode));
	Response *response = connection.getResponse();
	response->setHeaders("Server", "YKK_webserv");
	response->setHeaders("Date", ft::getCurrentTime());
	std::string errorpage_body = Response::makeStatusPage(statusCode, connection.getRequest()->GetMethod());
	response->setHeaders("Content-Type", "text/html");
	response->setHeaders("Content-Length", ft::itos(errorpage_body.size()));
	response->setBody(errorpage_body);
	if (statusCode == 301)
	{
		std::string url;
		// url += "http://" + this->mHost + connection.getRequest()->GetDirectory() + "/" + connection.getRequest()->GetFileName() + "/";
		// NOTE 상대 경로만 적어주어도 됨
		response->setHeaders("Location", connection.getRequest()->GetDirectory() + "/" + connection.getRequest()->GetFileName() + "/");
	}
}

void			Server::createResponse0(Connection &connection, std::string uriPlusFile)
{
	connection.setResponse(new Response(&connection, 0));
	Response *response = connection.getResponse();
	response->setHeaders("Server", "webserv");
	response->setHeaders("Date", ft::getCurrentTime().c_str());
	response->setHeaders("Content-Type", "text/html");
	response->setHeaders("Content-Language", "en-US");

	std::string body;
	int fd = -1;
	body = ft::getBodyFromFile(uriPlusFile);
	if (body.size() == 0)
	{
		fd = open("./error.html", O_RDONLY);
		body = ft::getBodyFromFd(fd);
	}
	response->setBody(body);
	response->setHeaders("Content-Length", ft::itos(response->getBody().length()));
	if (fd > 2)
		close(fd);
	return ;
}

void			Server::createResponse200(Connection &connection, std::string uriPlusFile)
{
	connection.setResponse(new Response(&connection, 200));
	Response *response = connection.getResponse();
	response->setHeaders("Server", "webserv");
	response->setHeaders("Date", ft::getCurrentTime().c_str());
	response->setHeaders("Content-Type", "text/html");
	response->setHeaders("Content-Language", "en-US");

	std::string body;
	body = ft::getBodyFromFile(uriPlusFile);
	response->setBody(body);
	response->setHeaders("Content-Length", ft::itos(response->getBody().length()));
	return ;
}

void			Server::solveRequest(Connection& connection, Request& request)
{
	char temp[500];
	getcwd(temp, 500);
	std::string absolute_path(temp);
	
	std::string serverName;
	std::map<std::string, std::string>::iterator it;
	it = request.GetHeaders().find("host");
	if (it == request.GetHeaders().end())
		serverName = "localhost";
	else
		serverName = it->second;

	configIterator configIterator; // NOTE configfile에 있는 내용을 전달하기위해서 구조체를 이용함
	std::vector<serverBlock>::iterator serverBlock = returnIteratorServerBlock(this->getServerBlocks(), serverName);
	configIterator.serverBlock = serverBlock;
	std::string relative_uri = request.GetDirectory() + "/" + request.GetFileName();
	std::vector<LocationPath>::iterator locationPath = returnIteratorLocationPath(serverBlock->mlocationPaths, relative_uri);
	configIterator.locationPath = locationPath;

	std::string root = locationPath->mRoot.getPath();
	std::string relative_path = request.GetDirectory() + "/" + request.GetFileName();
	std::string targetUri = absolute_path + root + relative_path;
	// cout << "targetUri: " << targetUri << endl;

	if (isValidMethod(request, configIterator) == false)
	{
		throw 405;
		return ;
	}
	if (request.GetURItype() == Request::FILE_TO_CREATE)
	{

		if (locationPath->mClientMaxBodySize < request.getBody().length() && locationPath->mClientMaxBodySize != 0)
			throw 413;
		executePut(connection, request, targetUri);
		connection.SetStatus(Connection::SEND_READY);

	}
	else if (request.GetURItype() == Request::DIRECTORY)
	{
		if (locationPath->mClientMaxBodySize < request.getBody().length() && locationPath->mClientMaxBodySize != 0)
			throw 413;
		if (ft::access(absolute_path + root + relative_path, 0) == 0) // NOTE 있는 폴더 경로에 접근 했을 때, index,html or autoindex
		{
			for (std::size_t i = 0; i < locationPath->mIndexPages.size(); i++)
			{
				std::string uri_indexhtml(absolute_path + root + request.GetDirectory());
				uri_indexhtml += "/" + locationPath->mIndexPages[i].getPath();
				if (ft::isFilePath(uri_indexhtml) == true && ft::access(uri_indexhtml, 0) == 0)
				{
					createResponse200(connection, uri_indexhtml);
					connection.SetStatus(Connection::SEND_READY);
					return ;
				}
			}

			// NOTE index_pages 으로 찾아봐도 해당 페이지가 없음. 에러페이지 혹은 오토인덱스 페이지를 보여줘야함.
			if (locationPath->mAutoIndex == true)
			{
				// cout << "serverBlock autoindex: " << locationPath->mAutoIndex << endl;
				std::string temp = root + relative_path;
				executeAutoindex(connection, ft::ReplaceAllModified(temp, "//", "/"));
				connection.SetStatus(Connection::SEND_READY);
				return ;
			}
			else
			{
				throw 404;
			}
		}
		else // NOTE 없는 "폴더경로"로 접근 했을 때, 404 page. 파일이건 폴더이건 신경쓰지 않음
		{
			throw 404;
		}
	}
	else if (request.GetURItype() == Request::FILE || request.GetURItype() == Request::CGI_PROGRAM)
	{
		if (locationPath->mClientMaxBodySize < request.getBody().length() && locationPath->mClientMaxBodySize != 0)
			throw 413;
		errno = 0;
		if (ft::access(absolute_path + root + relative_path, 0) == 0)
		{
			if (ft::isDirPath(absolute_path + root + relative_path) == true && ft::isFilePath(absolute_path + root + relative_path) == false)
			{
				throw 301;
			}
			if (request.GetMethod().compare("GET") == 0)
			{
				executeGet(connection, targetUri);
				if (request.GetURItype() == Request::FILE)
					connection.SetStatus(Connection::SEND_READY);
				else
					connection.SetStatus(Connection::CGI_READY);
			}
			else if (request.GetMethod().compare("HEAD") == 0)
			{
				executeHead(connection, targetUri);
				if (request.GetURItype() == Request::FILE)
					connection.SetStatus(Connection::SEND_READY);
				else
					connection.SetStatus(Connection::CGI_READY);
			}
			else if (request.GetMethod().compare("POST") == 0)
			{
				if (locationPath->mClientMaxBodySize < request.getBody().length() && locationPath->mClientMaxBodySize != 0)
					throw 413;
				executePost(connection, request);
				if (request.GetURItype() == Request::FILE)
					connection.SetStatus(Connection::SEND_READY);
				else
					connection.SetStatus(Connection::CGI_READY);
			}
			else if (request.GetMethod().compare("DELETE") == 0)
			{
				executeDelete(connection, request, targetUri);
				connection.SetStatus(Connection::SEND_READY);
			}
			else if (request.GetMethod().compare("OPTIONS") == 0)
			{
				executeOptions(connection, targetUri, configIterator);
				connection.SetStatus(Connection::SEND_READY);
			}
			// else if (request.GetMethod().compare("TRACE") == 0)
			// {
			// 	// executeOptions(connection, request);
			// 	connection.SetStatus(Connection::SEND_READY);
			// }
			else
			{
				throw 405; // NOTE Method NOTE Allowed
			}
		}
		else
		{
			if (errno == 2)
				throw 404;
		}
	}
	else
	{
		throw 0;
	}
}

const char* Server::IOError::what() const throw(){ return ("I/O error occurred."); }

char**			Server::createCGIEnv(const Connection& connection) const
{
	std::map<std::string, std::string>	cgiEnv;
	
	int	i = 0;
	while (gEnv[i])
	{
		std::string	env = std::string(gEnv[i]);
		std::size_t	found = env.find("=");
		if (found != std::string::npos)
		{
			cgiEnv[env.substr(0, found)] = env.substr(found + 1);
		}
		i++;
	}
	
	Request*	request = connection.getRequest();

	std::map<std::string, std::string>::iterator	it = request->GetHeaders().find("authorization");
	if (it != request->GetHeaders().end())
	{
		std::size_t	found = it->first.find(" ");
		if (found != std::string::npos)
		{
			cgiEnv["AUTH_TYPE"] = it->second.substr(0, found);		// NOTE 불확실 검증필요
			cgiEnv["REMOTE_IDENT"] = it->second.substr(found + 1);	// NOTE 불확실 검증필요
			cgiEnv["REMOTE_USER"] = it->second.substr(found + 1);	// NOTE 불확실 검증필요
			
		}
	}

	it = request->GetHeaders().find("content-length");
	if (it != request->GetHeaders().end())
	{
		cgiEnv["CONTENT_LENGTH"] = it->second;
	}

	it = request->GetHeaders().find("content-type");
	if (it != request->GetHeaders().end())
	{
		cgiEnv["CONTENT_TYPE"] = it->second;
	}

	cgiEnv["GATEWAY_INTERFACE"] = "CGI/1.1";												// STUB config 클래스에서 가져와야함

	cgiEnv["PATH_INFO"] = request->GetURI();

	cgiEnv["PATH_TRANSLATED"] = request->GetFileName(); 									// STUB 잘 모르겠음 _file_path

	cgiEnv["QUERY_STRING"] = request->GetQuery();

	cgiEnv["REMOTE_ADDR"] = connection.getClientIp();									// STUB client ip 주소 필요함 IP address of the client (dot-decimal).
	cgiEnv["REQUEST_METHOD"] = request->GetMethod();
	cgiEnv["REQUEST_URI"] = request->GetURI();
	cgiEnv["SCRIPT_NAME"] = "ft_tester/cgi_tester";											// STUB "relative path to the program, like /cgi-bin/script.cgi.";

	cgiEnv["SERVER_NAME"] = "FIXME";														// FIXME server_name을 가져다 쓰지 못하고 있다..
	cgiEnv["SERVER_PORT"] = ft::itos(mPort);
	cgiEnv["SERVER_PROTOCOL"] = "HTTP/1.1";													// STUB 서버의 버전을 지정해줘야하는데 우선 문자열로 박아넣음 "HTTP/version.";
	cgiEnv["SERVER_SOFTWARE"] = cgiEnv["SERVER_NAME"] + "/" + cgiEnv["SERVER_PROTOCOL"];	// STUB "name/version of HTTP server.";

	std::map<std::string, std::string> headers = request->GetHeaders();
	std::map<string, string>::iterator it_http;
	for (it_http = headers.begin(); it_http != headers.end(); it_http++)
	{
		std::string http_cgi = "HTTP_";
		std::string temp = it_http->first;
		for(std::string::size_type i = 0; i < temp.length(); i++)
		{
			if (temp[i] == '-')
				temp[i] = '_';
			temp[i] = std::toupper(temp[i]);
		}
		http_cgi += temp;
		// cout << "http_cgi: " << http_cgi << " | " << "value: " << it_http->second << endl;
		cgiEnv[http_cgi] = it_http->second;
	}
	


	try
	{
		char**	ret = new char*[cgiEnv.size() + 1];
		int	i = 0;
		for (std::map<std::string,std::string>::iterator it = cgiEnv.begin(); it != cgiEnv.end(); ++it)
		{
			ret[i++] = strdup((it->first + "=" + it->second).c_str());	// iostream 했더니 strdup됨
		}
		ret[i] = 0;

		// ANCHOR cgiENV debug block
		// {
		// 	i = 0;
		// 	while (ret[i])
		// 	{
		// 		std::cout << ret[i++] << std::endl;
		// 	}
		// 	exit(1);
		// }

		return (ret);
	}
	catch(const std::exception& e)
	{
		throw 500; // NOTE 뭘 날려야할까요 500번대 에러는 맞는데...
	}
}

bool Server::isValidMethod(Request &request, configIterator configIterator)
{
	// std::vector<serverBlock>::iterator serverBlock = configIterator.serverBlock;
	std::vector<LocationPath>::iterator locationPath = configIterator.locationPath;
	for (size_t i = 0; i < locationPath->mMethod.size(); i++)
	{
		// cout << i <<  ": " << locationPath->mMethod[i] << endl;
		if (request.GetMethod() == locationPath->mMethod[i])
			return (true);
	}
	return (false);
}