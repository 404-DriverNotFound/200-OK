#include "Server.hpp"
#include "ServerManager.hpp" // NOTE 상호참조 문제를 해결하기 위해서!
#include "Response.hpp"



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
		if (it2->second.GetSocket() == fd)
			continue ;
		if ((FD_ISSET(it2->second.GetSocket(), &(this->mManager->GetReadCopyFds())) == 0) &&
				it2->second.IsKeepConnection() == false)
		{
			// cout << "it2->second.GetSocket() " << it2->second.GetSocket() << endl;
			return (it2->second.GetSocket());
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
		if (it2->second.GetSocket() == clientFd)
		{
			std::cerr << REDB "[" << ft::getCurrentTime() << "][connection]" << "[DISCONNECTED]" << "[" << clientFd << "]" << NC << std::endl;
			mConnections.erase(it2);
			return ;
		}
	}
}

void			Server::recvRequest(Connection& connection)
{
	Request*	request = connection.GetRequest();
	char		buf[BUFFER_SIZE];
	
	ssize_t		count = read(connection.GetSocket(), buf, sizeof(buf));
	if (count > 0)
	{
		// REVIEW 파싱단계에서 count 변수를 사용해서 탐색 범위를 좁힐 수 있을까?
		connection.AddRbufFromClient(buf, count);
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
					// FD_SET(connection.GetSocket(), &(this->mManager->GetWriteFds()));
					// FD_SET(connection.GetSocket(), &(this->mManager->GetWriteCopyFds()));
					mManager->SetWriteFds(connection.GetSocket());
					mManager->SetWriteCopyFds(connection.GetSocket());
				}
			}
			else
			{
				request->SetPhase(Request::COMPLETE);
				mManager->SetWriteFds(connection.GetSocket());
				mManager->SetWriteCopyFds(connection.GetSocket());
				// FD_SET(connection.GetSocket(), &(this->mManager->GetWriteFds()));
				// FD_SET(connection.GetSocket(), &(this->mManager->GetWriteCopyFds()));
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
	Request*			request = connection.GetRequest();
	const std::string&	requestLine = request->GetHttpMessage();
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
	Request*		request = connection.GetRequest();
	std::size_t		found = request->GetHttpMessage().find("\r\n\r\n", request->GetSeek());
	if (found == std::string::npos)
	{
		return (false);
	}

	while (true)
	{
		std::size_t		found = request->GetHttpMessage().find("\r\n", request->GetSeek());
		if (found == std::string::npos)
		{
			throw 400;
		}
		std::string	line = request->GetHttpMessage().substr(request->GetSeek(), found - request->GetSeek());
		if (line.empty())
		{
			request->SetSeek(found + 2);
			break ;
		}

		request->AddHeader(line);
		request->SetSeek(found + 2);
	}
	return (true);
}

bool			Server::parseBody(Connection& connection)
{
	Request*		request = connection.GetRequest();

	// NOTE chunked parsing
	if (request->GetTransferType() == Request::CHUNKED)
	{
		while (true)
		{
			// hex 기다림
			std::size_t		foundHex = request->GetHttpMessage().find("\r\n", request->GetSeek());
			if (foundHex == std::string::npos)
			{
				return (false);
			}
			else
			{
				// 바디 기다림
				std::size_t	foundBody = request->GetHttpMessage().find("\r\n", foundHex + 2);
				if (foundBody == std::string::npos)
				{
					return (false);
				}
				else
				{
					std::string		hex = request->GetHttpMessage().substr(request->GetSeek(), foundHex - request->GetSeek());
					unsigned long	hexValue = ft::stohex(hex);
					request->SetSeek(foundHex + 2);
					std::string		body = request->GetHttpMessage().substr(request->GetSeek(), foundBody - request->GetSeek());
					if (hexValue != body.length())
					{
						throw 413; // REVIEW payload too large 이거 맞는지 모르겠음
					}
					else
					{
						request->AddBody(body);
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
		int												bodyLength = request->GetHttpMessage().length() - request->GetSeek();
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
			request->SetBody(request->GetHttpMessage().substr(request->GetSeek()));
			return (true);
		}
	}
}

void			Server::createResponseStatusCode(Connection &connection, int statusCode)
{
	if (connection.GetResponse() != NULL)
	{
		delete connection.GetResponse();
		connection.SetResponse(NULL);
	}
	// createResponse_statuscode
	std::map<int, std::string> &status_map = Response::mStatusMap;
	std::map<int, std::string>::iterator it;
	if ((it = status_map.find(statusCode)) == status_map.end())
	{
		statusCode = 0;
	}
	connection.SetResponse(new Response(&connection, statusCode));
	Response *response = connection.GetResponse();
	response->setHeaders("Server", "YKK_webserv");
	response->setHeaders("Date", ft::getCurrentTime());
	std::string errorpage_body;
	if (connection.GetRequest() != NULL)
		errorpage_body = Response::makeStatusPage(statusCode, connection.GetRequest()->GetMethod());
	else
		errorpage_body = Response::makeStatusPage(statusCode, "");
	if (statusCode == 205)
	{
		response->setHeaders("Content-Length", "0");
		response->setBody("");
	}
	else
	{
		response->setHeaders("Content-Type", "text/html");
		response->setHeaders("Content-Length", ft::itos(errorpage_body.size()));
		response->setBody(errorpage_body);
	}
	if (statusCode == 401)
	{
		response->setHeaders("WWW-Authenticate", "Basic realm=\"Access to the auth source\"");
	}
	if (statusCode == 301)
	{
		std::string url;
		// url += "http://" + this->mHost + connection.GetRequest()->GetDirectory() + "/" + connection.GetRequest()->GetFileName() + "/";
		// NOTE 상대 경로만 적어주어도 됨
		response->setHeaders("Location", connection.GetRequest()->GetDirectory() + "/" + connection.GetRequest()->GetFileName() + "/");
	}
	if (statusCode >= 500)
	{
		response->setHeaders("Retry-After", "60");
	}
}

void			Server::createResponse0(Connection &connection, std::string uriPlusFile)
{
	connection.SetResponse(new Response(&connection, 0));
	Response *response = connection.GetResponse();
	response->setHeaders("Server", "webserv");
	response->setHeaders("Date", ft::getCurrentTime().c_str());
	response->setHeaders("Content-Type", "text/html");
	response->setHeaders("Content-Language", "en-US");

	std::string body;
	int fd = -1;
	body = ft::GetBodyFromFile(uriPlusFile);
	if (body.size() == 0)
	{
		fd = open("./error.html", O_RDONLY);
		body = ft::GetBodyFromFd(fd);
	}
	response->setBody(body);
	response->setHeaders("Content-Length", ft::itos(response->GetBody().length()));
	if (fd > 2)
		close(fd);
	return ;
}

void			Server::createResponse200(Connection &connection, std::string targetUri)
{
	connection.SetResponse(new Response(&connection, 200));
	Response *response = connection.GetResponse();
	response->setHeaders("Server", "webserv");
	response->setHeaders("Date", ft::getCurrentTime().c_str());
	response->setHeaders("Content-Type", "text/html");
	response->setHeaders("Content-Language", "en-US");

	std::string body;
	body = ft::GetBodyFromFile(targetUri);
	response->setBody(body);
	response->setHeaders("Content-Length", ft::itos(response->GetBody().length()));
	
	if (connection.GetRequest()->GetMethod().compare("GET") == 0)
	{
		struct stat buf;
		stat(targetUri.c_str(), &buf);
		response->setHeaders("Last-Modified", ft::getHTTPTimeFormat(buf.st_mtime));
	}
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
	connection.SetTargetUri(targetUri);
	connection.SetServerName(serverBlock->mserverName);
	// TODO
	size_t found = request.GetFileName().find(".");
	if (found != std::string::npos)
	{
		for (size_t i = 0; i < locationPath->mCgiProgramPath.size(); i++)
		{
			if (locationPath->mCgiProgramPath[i].find(request.GetFileName().substr(found + 1)) != std::string::npos)
			{
				connection.SetCgiProgramPath(locationPath->mCgiProgramPath[i]);
			}
		}
	}


	if (isValidMethod(request, configIterator) == false)
	{
		throw 405;
	}
	if (hasAuthModule(configIterator)) // STUB 	일단 항상 참이게 구현
	{
		std::map<std::string, std::string>::iterator	it = request.GetHeaders().find("authorization");
		if (it == request.GetHeaders().end())
		{
			throw 401;
		}
		else
		{
			if (!isRightCredentials(it->second))
			{
				throw 403;
			}
		}
	}
	if (request.GetURItype() == Request::FILE_TO_CREATE || request.GetURItype() == Request::FILE_TO_DELETE) // NOTE 멱등성이 있는 메소드
	{
		if (request.GetURItype() == Request::FILE_TO_CREATE)
		{
			if (locationPath->mClientMaxBodySize < request.GetBody().length() && locationPath->mClientMaxBodySize != 0)
				throw 413;
			executePut(connection, request, targetUri);
			connection.SetStatus(Connection::SEND_READY);
			return ;
		}
		else
		{
			executeDelete(connection, targetUri);
			connection.SetStatus(Connection::SEND_READY);
			return ;
		}
	}
	else if (request.GetURItype() == Request::DIRECTORY)
	{
		if (locationPath->mClientMaxBodySize < request.GetBody().length() && locationPath->mClientMaxBodySize != 0)
			throw 413;
		if (ft::access(targetUri, 0) == 0) // NOTE 있는 폴더 경로에 접근 했을 때, index,html or autoindex
		{
			if (request.GetMethod().compare("OPTIONS") == 0)
			{
				executeOptions(connection, configIterator);
				connection.SetStatus(Connection::SEND_READY);
				return ;
			}
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
		if (locationPath->mClientMaxBodySize < request.GetBody().length() && locationPath->mClientMaxBodySize != 0)
			throw 413;
		if (request.GetURItype() == Request::CGI_PROGRAM)
		{
			if (request.GetMethod().compare("GET") == 0)
			{
				executeGet(connection, targetUri);
				if (request.GetURItype() == Request::FILE)
					connection.SetStatus(Connection::SEND_READY);
				else
					connection.SetStatus(Connection::CGI_READY);
				return ;
			}
			else if (request.GetMethod().compare("HEAD") == 0)
			{
				executeHead(connection, targetUri);
				if (request.GetURItype() == Request::FILE)
					connection.SetStatus(Connection::SEND_READY);
				else
					connection.SetStatus(Connection::CGI_READY);
				return ;
			}
			else if (request.GetMethod().compare("POST") == 0)
			{
				executePost(connection, request);
				if (request.GetURItype() == Request::FILE)
					connection.SetStatus(Connection::SEND_READY);
				else
					connection.SetStatus(Connection::CGI_READY);
				return ;
			}
			else
			{
				throw 405;
			}
		}
		else
		{
			errno = 0;
			if (ft::access(targetUri, 0) == 0)
			{
				if (ft::isDirPath(targetUri) == true && ft::isFilePath(targetUri) == false)
				{
					throw 301;
				}
				if (request.GetMethod().compare("GET") == 0)
				{
					executeGet(connection, targetUri);
					if (request.GetURItype() == Request::FILE)
						connection.SetStatus(Connection::SEND_READY);
					else
					{
						connection.SetStatus(Connection::CGI_READY);
					}
				}
				else if (request.GetMethod().compare("HEAD") == 0)
				{
					executeHead(connection, targetUri);
					if (request.GetURItype() == Request::FILE)
						connection.SetStatus(Connection::SEND_READY);
					else
					{
						connection.SetStatus(Connection::CGI_READY);
					}
				}
				else if (request.GetMethod().compare("POST") == 0)
				{
					if (locationPath->mClientMaxBodySize < request.GetBody().length() && locationPath->mClientMaxBodySize != 0)
						throw 413;
					executePost(connection, request);
					if (request.GetURItype() == Request::FILE)
						connection.SetStatus(Connection::SEND_READY);
					else
						connection.SetStatus(Connection::CGI_READY);
				}
				else if (request.GetMethod().compare("DELETE") == 0)
				{
					executeDelete(connection, targetUri);
					connection.SetStatus(Connection::SEND_READY);
				}
				else if (request.GetMethod().compare("OPTIONS") == 0)
				{
					executeOptions(connection, configIterator);
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
				if (errno == ENOENT)
					throw 404;
			}
		}
	}
	else
	{
		throw 0;
	}
}

const char* Server::IOError::what() const throw(){ return ("I/O error occurred."); }

char**			Server::createCgiEnvp(const Connection& connection) const
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
	
	Request*	request = connection.GetRequest();

	std::map<std::string, std::string>::iterator	it = request->GetHeaders().find("authorization");
	if (it != request->GetHeaders().end())
	{
		std::size_t	found = it->first.find(" ");
		if (found != std::string::npos)
		{
			cgiEnv["AUTH_TYPE"] = it->second.substr(0, found);
			cgiEnv["REMOTE_IDENT"] = it->second.substr(found + 1);
			cgiEnv["REMOTE_USER"] = it->second.substr(found + 1);
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

	cgiEnv["GATEWAY_INTERFACE"] = "CGI/1.1";

	cgiEnv["PATH_INFO"] = request->GetURI();
	cgiEnv["PATH_TRANSLATED"] = connection.GetTargetUri();
	cgiEnv["QUERY_STRING"] = request->GetQuery();
	cgiEnv["REMOTE_ADDR"] = connection.GetClientIp();
	cgiEnv["REQUEST_METHOD"] = request->GetMethod();
	cgiEnv["REQUEST_URI"] = request->GetURI();
	cgiEnv["SCRIPT_NAME"] = request->GetFileName();
	cgiEnv["SERVER_NAME"] = connection.GetServerName();
	cgiEnv["SERVER_PORT"] = ft::itos(mPort);
	cgiEnv["SERVER_PROTOCOL"] = "HTTP/1.1";
	cgiEnv["SERVER_SOFTWARE"] = "webserv/1.0";

	std::map<std::string, std::string> headers = request->GetHeaders();
	std::map<std::string, std::string>::iterator it_http;
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

bool		Server::hasAuthModule(const configIterator& config_it)
{
	std::vector<LocationPath>::iterator locationPath = config_it.locationPath;
	if (locationPath->mAuthBasicUserFile.getSize() != 0)
		return (true);
	else
		return (false);
}

bool		Server::isRightCredentials(const std::string& authorization)
{
	// NOTE authorization = "Basic asdf3r323if"
	std::size_t found = authorization.find(" ");
	if (found == std::string::npos)
	{
		return (false);
	}
	std::string	type = authorization.substr(0, found);
	if (type.compare("Basic") != 0)
	{
		return (false);
	}
	std::string	credentials = authorization.substr(found + 1);
	
	// NOTE 파일에서 우리서버 인증정보 가져오기
	int fd = open(".htpasswd", O_RDONLY);
	if (fd < 0)
	{
		throw 500;
	}
	char	buf[BUFFER_SIZE];
	ssize_t	cnt = read(fd, buf, sizeof(buf));
	if (cnt < 0)
	{
		throw 500;
	}
	if (fd > 2)
		close(fd);
	std::string	htpasswd(buf, cnt);

	// NOTE 일치하는지 확인하기
	found = htpasswd.find(credentials + "\n");
	if (found == std::string::npos)
	{
		return (false);
	}
	return (true);
}

char**			Server::createCgiArgv(const Connection& connection) const
{
	try
	{
		char**		ret = new char*[3];
		std::string	targetUri = connection.GetTargetUri();
		std::string	cgiProgramPath = connection.GetCgiProgramPath();

		ret[0] = new char[cgiProgramPath.length() + 1];
		strcpy(ret[0], const_cast<char*>(cgiProgramPath.c_str())); ret[0][cgiProgramPath.length()] = '\0';
		ret[1] = new char[targetUri.length() + 1];
		strcpy(ret[1], const_cast<char*>(targetUri.c_str())); ret[1][targetUri.length()] = '\0';
		ret[2] = 0;
		return (ret);
	}
	catch(const std::exception& e)
	{
		throw 500;
	}
}