#include "Server.hpp"
#include "ServerManager.hpp" // NOTE 상호참조 문제를 해결하기 위해서!
#include "Response.hpp"

extern char**	g_env;

int				Server::SetSocket(std::string ip, uint16_t port)
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

void			Server::recvRequest(Connection& connection)
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
					std::cout << "|" << request->getBody() << "|" << std::endl;
				}
			}
			else
			{
				request->SetPhase(Request::COMPLETE);
			}
		}
	}
	else
	{
		throw Server::ClientServerClose();
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
	request->SetMethod(tmp);
	request->SetSeek(found + 1);

	// URI 파싱
	found = requestLine.find(' ', request->GetSeek());
	if (found == std::string::npos)
	{
		throw 414;
	}
	tmp = requestLine.substr(request->GetSeek(), found - request->GetSeek());
	std::cout << "\t|" << tmp << "|" << std::endl;

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
	std::cout << "\t|" << tmp << "|" << std::endl;
	// TODO 지원하지 않는 버전 관련 부분 추가해야함
	// if (isUnsopportingVersion())
	// {
	// 	throw 505;
	// }
	request->SetSeek(found + 2);
	return (true);
}

bool			Server::parseHeader(Connection& connection)
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

bool			Server::parseBody(Connection& connection)
{
	std::cout << "parseBody() called" << std::endl;
	Request*		request = connection.get_m_request();

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
		// NOTE contents-length parsing
		std::map<std::string, std::string>::iterator	it = request->GetHeaders().find("content-length");
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

void			Server::create_Response_statuscode(Connection &connection, int status_code)
{
	if (connection.get_m_response() != NULL)
	{
		delete connection.get_m_response();
		connection.set_m_response(NULL);
	}
	// createResponse_statuscode
	std::map<int, std::string> &status_map = Response::m_status_map;
	std::map<int, std::string>::iterator it;
	if ((it = status_map.find(status_code)) == status_map.end())
	{
		status_code = 0;
	}
	connection.set_m_response(new Response(&connection, status_code));
	Response *response = connection.get_m_response();
	response->set_m_headers("Server", "YKK_webserv");
	response->set_m_headers("Date", ft::getCurrentTime());
	std::string errorpage_body = Response::makeStatusPage(status_code, connection.get_m_request()->GetMethod());
	response->set_m_headers("Content-Type", "text/html");
	response->set_m_headers("Content-Length", ft::itos(errorpage_body.size()));
	response->set_m_body(errorpage_body);
}

void			Server::create_Response_0(Connection &connection, std::string uri_plus_file)
{
	connection.set_m_response(new Response(&connection, 0));
	Response *response = connection.get_m_response();
	response->set_m_headers("Server", "webserv");
	response->set_m_headers("Date", ft::getCurrentTime().c_str());
	response->set_m_headers("Content-Type", "text/html");
	response->set_m_headers("Content-Language", "en-US");

	std::string body;
	int fd = -1;
	body = ft::getBody_from_file(uri_plus_file);
	if (body.size() == 0)
	{
		fd = open("./error.html", O_RDONLY);
		body = ft::getBody_from_fd(fd);
	}
	response->set_m_body(body);
	response->set_m_headers("Content-Length", ft::itos(response->get_m_body().length()));
	if (fd > 2)
		close(fd);
	return ;
}

void			Server::create_Response_200(Connection &connection, std::string uri_plus_file, TYPE_HTML type)
{
	connection.set_m_response(new Response(&connection, 200));
	Response *response = connection.get_m_response();
	response->set_m_headers("Server", "webserv");
	response->set_m_headers("Date", ft::getCurrentTime().c_str());
	response->set_m_headers("Content-Type", "text/html");
	response->set_m_headers("Content-Language", "en-US");

	std::string body;
	int fd = -1;
	body = ft::getBody_from_file(uri_plus_file);
	if (body.size() == 0)
	{
		if (type == INDEX_HTML)
			fd = open("./index.html", O_RDONLY);
		else
		{
			delete connection.get_m_response();
			connection.set_m_response(NULL);
			throw 404;
		}
		body = ft::getBody_from_fd(fd);
	}
	response->set_m_body(body);
	response->set_m_headers("Content-Length", ft::itos(response->get_m_body().length()));
	if (fd > 2)
		close(fd);
	return ;
}

void			Server::solveRequest(Connection& connection, Request& request)
{
	cout << "solveRequest()" << endl;
	char temp[500];
	getcwd(temp, 500);
	std::string absolute_path(temp);
	
	std::string hostname;
	std::map<std::string, std::string>::iterator it;
	it = request.GetHeaders().find("host");
	if (it == request.GetHeaders().end())
		hostname = "localhost";
	else
		hostname = it->second;

	config_iterator config_it; // NOTE configfile에 있는 내용을 전달하기위해서 구조체를 이용함
	std::vector<ServerBlock>::iterator serverblock = return_iterator_serverblock(this->get_m_serverBlocks(), hostname);
	config_it.serverblock = serverblock;
	std::string relative_uri = request.GetDirectory() + "/" + request.GetFileName();
	std::vector<LocationPath>::iterator locationPath = return_iterator_locationpathlocationPath(serverblock->mlocationPaths, relative_uri);
	config_it.locationPath = locationPath;

	std::string root = locationPath->mroot.getPath();
	std::string relative_path = request.GetDirectory() + "/" + request.GetFileName();
	std::string target_uri = absolute_path + root + relative_path;
	cout << "target_uri: " << target_uri << endl;
	// NOTE 무작위 값이 들어감
	
	// DIR *dir = opendir(target_uri.c_str()); closedir(dir);
	if (request.GetURItype() == Request::FILE_TO_CREATE)
	{
		executePut(connection, request, target_uri, config_it);
		connection.SetStatus(Connection::SEND_READY);

	}
	else if (request.GetURItype() == Request::DIRECTORY)
	{
		if (ft::access(absolute_path + root + relative_path) == true) // NOTE 있는 폴더 경로에 접근 했을 때, index,html or autoindex
		{
			for (int i = 0; i < locationPath->mindex_pages.size(); i++)
			{
				std::string uri_indexhtml(absolute_path + root + request.GetDirectory());
				uri_indexhtml += "/" + locationPath->mindex_pages[i].getPath();
				if (ft::isFilePath(uri_indexhtml) == true && ft::access(uri_indexhtml) == true)
				{
					create_Response_200(connection, uri_indexhtml, INDEX_HTML);
					connection.SetStatus(Connection::SEND_READY);
					return ;
				}
			}

			// NOTE index_pages 으로 찾아봐도 해당 페이지가 없음. 에러페이지 혹은 오토인덱스 페이지를 보여줘야함.
			if (serverblock->mauto_index == true)
			{
				cout << "serverblock autoindex: " << serverblock->mauto_index << endl;
				std::string temp = root + relative_path;
				executeAutoindex(connection, *connection.get_m_request(), ft::ReplaceAll_modified(temp, "//", "/"));
				connection.SetStatus(Connection::SEND_READY);
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
			create_Response_0(connection, uri_errorhtml);
			connection.SetStatus(Connection::SEND_READY);
			return ;
		}
	}
	else if (request.GetURItype() == Request::FILE || request.GetURItype() == Request::CGI_PROGRAM)
	{
		if (request.GetMethod().compare("GET") == 0)
		{
			executeGet(connection, request, target_uri);
			if (request.GetURItype() == Request::FILE)
				connection.SetStatus(Connection::SEND_READY);
			else
				connection.SetStatus(Connection::CGI_READY);
		}
		else if (request.GetMethod().compare("HEAD") == 0)
		{
			executeHead(connection, request, target_uri);
			if (request.GetURItype() == Request::FILE)
				connection.SetStatus(Connection::SEND_READY);
			else
				connection.SetStatus(Connection::CGI_READY);
		}
		else if (request.GetMethod().compare("POST") == 0)
		{
			executePost(connection, request, target_uri);
			if (request.GetURItype() == Request::FILE)
				connection.SetStatus(Connection::SEND_READY);
			else
				connection.SetStatus(Connection::CGI_READY);
		}
		else if (request.GetMethod().compare("DELETE") == 0)
		{
			executeDelete(connection, request, target_uri);
			connection.SetStatus(Connection::SEND_READY);
		}
		else if (request.GetMethod().compare("OPTIONS") == 0)
		{
			executeOptions(connection, request, target_uri, config_it);
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
		throw 0;
	}

}

const char* Server::ClientServerClose::what() const throw(){ return ("Client close Server!"); }

char**			Server::createCGIEnv(const Connection& connection) const
{
	std::map<std::string, std::string>	cgiEnv;
	
	int	i = 0;
	while (g_env[i])
	{
		std::string	env = std::string(g_env[i]);
		std::size_t	found = env.find("=");
		if (found != std::string::npos)
		{
			cgiEnv[env.substr(0, found)] = env.substr(found + 1);
		}
		i++;
	}
	
	Request*	request = connection.get_m_request();

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

	struct sockaddr_in temp;
	char remote_addr[16];
	socklen_t temp_len = sizeof(struct sockaddr_in);

	getsockname(connection.get_m_fd(), (sockaddr *)&temp, &temp_len); // NOTE 반환값이 존재하여 에러를 체크할 수는 있으나, fd에 의한 에러를 발생할 가능성 거의 없다.
	strncpy(remote_addr, inet_ntoa(temp.sin_addr), 16); // NOTE  반듯이 buffer에 복사해서 사용해야함. 반환값이 동적할당없는 포인터임.
	cgiEnv["REMOTE_ADDR"] = remote_addr;														// STUB client ip 주소 필요함 IP address of the client (dot-decimal).

	cgiEnv["REQUEST_METHOD"] = request->GetMethod();
	cgiEnv["REQUEST_URI"] = request->GetURI();
	cgiEnv["SCRIPT_NAME"] = "/cgi-bin/script.cgi";											// STUB "relative path to the program, like /cgi-bin/script.cgi.";
	cgiEnv["SERVER_NAME"] = "YKK_Server"; 													// STUB "host name of the server, may be dot-decimal IP address.";
	cgiEnv["SERVER_PORT"] = ft::itos(mport);
	cgiEnv["SERVER_PROTOCOL"] = "HTTP/1.1";													// STUB 서버의 버전을 지정해줘야하는데 우선 문자열로 박아넣음 "HTTP/version.";
	cgiEnv["SERVER_SOFTWARE"] = cgiEnv["SERVER_NAME"] + "/" + cgiEnv["SERVER_PROTOCOL"];	// STUB "name/version of HTTP server.";

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
