#include "Server.hpp"
#include "ServerManager.hpp"
#include "Response.hpp"

void		Server::executeAutoindex(Connection& connection, std::string uriCopy)
{
	connection.SetResponse(new Response(&connection, 200, ft::makeAutoindexHTML(uriCopy)));
	Response *response = connection.GetResponse();
	response->setHeaders("Server", "webserv");
	response->setHeaders("Content-Type", "text/html");
	response->setHeaders("Connection", "keep-alive");
	response->setHeaders("Date", ft::getCurrentTime().c_str());
	response->setHeaders("Content-Length", ft::itos(connection.GetResponse()->GetBody().length()));

	connection.SetStatus(Connection::SEND_READY);
}

void		Server::executeGet(Connection& connection, std::string targetUri)
{
	if (ft::access(targetUri, 0) == -1)
	{
		throw 404;
	}
	else
	{
		createResponse200(connection, targetUri);
		return ;
	}
}

void		Server::executeHead(Connection& connection, std::string targetUri)
{
	executeGet(connection, targetUri);
	std::string null_str;
	connection.GetResponse()->setBody(null_str);
	Response *response = connection.GetResponse();

	std::map<std::string, std::string>::iterator it = response->getHeaders().begin();
	std::map<std::string, std::string> temp;
	while (it != response->getHeaders().end())
	{
		if (it->first == "Date")
			temp["Date"] = it->second;
		else if (it->first == "Server")
			temp["Server"] = it->second;
		else if (it->first == "Content-Length")
			temp["Content-Length"] = it->second;
		it++;
	}
	response->copyHeaders(temp);
	
}

void		Server::executePost(Connection& connection, const Request& request)
{
	connection.SetResponse(new Response(&connection, 200, request.GetBody()));
}

void		Server::executePut(Connection& connection, const Request& request, std::string targetUri)
{
	bool file_exist = 0;
	errno = 0;
	if (targetUri.back() == '/') // NOTE 무조껀 파일경로로 open 하도록 함. 폴더 경로로 open하면, 동작이 조금 다르다.
		targetUri.pop_back();
	int open_fd = open(targetUri.c_str(), O_WRONLY | O_TRUNC);

	if (open_fd > 2)
	{
		file_exist = true;
		if (errno == 0) // NOTE 파일를 open으로 열었음
		{
			if (connection.GetResponse() != NULL)
			{
				delete connection.GetResponse();
				connection.SetResponse(NULL);
			}
			Response *temp2  = new Response(&connection, 200, request.GetBody());
			connection.SetResponse(temp2);
			write(open_fd, connection.GetResponse()->GetBody().c_str(), connection.GetResponse()->GetBody().length());
		}
		close(open_fd);
	}
	else
	{
		if (errno == EISDIR) // NOTE 폴더를 open으로 열었음
		{
			connection.SetResponse(new Response(&connection, 204));
			std::string temp;
			connection.GetResponse()->setBody(temp);
		}
		else if (errno == ENOENT)
		{
			if (connection.GetResponse() != NULL)
			{
				delete connection.GetResponse();
				connection.SetResponse(NULL);
			}
			connection.SetResponse(new Response(&connection, 201, request.GetBody()));
			std::string temp = connection.GetTargetUri();
			std::string delete_double_slash = ft::ReplaceAllModified(temp, "//", "/");
			connection.GetResponse()->setHeaders("Content-Location", delete_double_slash);
			int open_fd2 = open(targetUri.c_str(), O_WRONLY | O_CREAT, 0755);

			write(open_fd2, connection.GetResponse()->GetBody().c_str(), connection.GetResponse()->GetBody().length());
			close(open_fd2);

		}
	}
	Response *response = connection.GetResponse();
	response->setHeaders("Date", ft::getCurrentTime().c_str());
	response->setHeaders("Server", "webserv");
	if (errno == 0)
	{
		response->setHeaders("Content-Length", ft::itos(response->GetBody().length()));
		response->setHeaders("Content-Type", "text/html");
		response->setHeaders("Content-Language", "en-US");
		if (file_exist == false)
		{
			response->setHeaders("Location", targetUri);
		}
	}
}

void		Server::executeDelete(Connection& connection, std::string targetUri)
{
	errno = 0;
	if (ft::access(targetUri, 0) == 0)
	{
		if (ft::isFilePath(targetUri) == true)
		{
			this->createResponse200(connection,targetUri);
			unlink(targetUri.c_str());
		}
		else if (ft::isDirPath(targetUri) == true)
		{
			int statusCode = 204;
			connection.SetResponse(new Response(&connection, statusCode));
			// NOTE 폴더라도 지우는 쪽으로 한다면 아래 주석 해제
			// rmdir(targetUri.c_str());
		}
	}
	else
	{
		int statusCode = 204;
		connection.SetResponse(new Response(&connection, statusCode));
	}
}

void		Server::executeOptions(Connection& connection, configIterator configIterator)
{
	connection.SetResponse(new Response(&connection, 200));
	Response *response = connection.GetResponse();
	response->setHeaders("Date", ft::getCurrentTime().c_str());
	response->setHeaders("Server", "webserv");
	std::string value;
	for (size_t i = 0; i < configIterator.locationPath->mMethod.size(); i++)
	{
		value += configIterator.locationPath->mMethod[i];
		if (i != configIterator.locationPath->mMethod.size() - 1)
			value += " ";
	}
	response->setHeaders("Allow", value);
}

void		Server::aexecuteTrace(Connection& connection)
{
	connection.GetSocket();
}

void		Server::executeCGI(Connection& connection)
{
	Response*	response = connection.GetResponse();

	std::string connectionFD = ft::itos(connection.GetSocket());
	if (connectionFD.size() == 0)
	{
		throw 500;
	}
	std::string	fromCGIfileName = "fromCGI" + connectionFD;

	if (connection.GetStatus() == Connection::CGI_READY)
	{
		std::string	toCGIfileName("toCGI");

		int	toCGI = open(toCGIfileName.c_str(), O_CREAT | O_TRUNC | O_RDWR, 0666);
		if (toCGI == -1)
		{
			throw 500;
		}
		int	fromCGI = open(fromCGIfileName.c_str(), O_CREAT | O_TRUNC | O_RDWR, 0666);
		if (fromCGI == -1)
		{
			close(toCGI);
			throw 500;
		}
		int	cnt = write(toCGI, response->GetBody().c_str(), response->GetBody().length());
		if (cnt < 0)
		{
			close(fromCGI);
			close(toCGI);
			throw 500;
		}
		else if (cnt == 0)
		{
			close(fromCGI);
			close(toCGI);
			throw 205;
		}
		if (lseek(toCGI, 0, SEEK_SET) == -1)
		{
			close(fromCGI);
			close(toCGI);
			throw 500;
		}

		char**	argv = createCgiArgv(connection);
		if (argv == NULL)
		{
			close(fromCGI);
			close(toCGI);
			throw 500;
		}
		char**	envp = createCgiEnvp(connection);
		if (envp == NULL)
		{
			for (size_t i = 0; argv[i] != NULL; i++)
			{
				free(argv[i]);
			}
			free(argv); argv = NULL;
			close(fromCGI);
			close(toCGI);
			throw 500;
		}
		int pid = fork();
		if (pid < 0)
		{
			for (size_t i = 0; argv[i] != NULL; i++)
			{
				free(argv[i]);
			}
			free(argv); argv = NULL;
			for (size_t i = 0; envp[i] != NULL; i++)
			{
				free(envp[i]);
			}
			free(envp); envp = NULL;
			close(fromCGI);
			close(toCGI);
			throw 500;
		}
		else if (pid == 0)
		{
			dup2(toCGI, 0); dup2(fromCGI, 1);
			execve(connection.GetCgiProgramPath().c_str(), argv, envp); // NOTE execve 돌아가는 코드
			exit(0);
		}
		else
		{
			waitpid(pid, 0, 0);
			for (size_t i = 0; argv[i] != NULL; i++)
			{
				free(argv[i]);
			}
			free(argv); argv = NULL;
			for (size_t i = 0; envp[i] != NULL; i++)
			{
				free(envp[i]);
			}
			free(envp); envp = NULL;
			close(fromCGI);
			close(toCGI); unlink(toCGIfileName.c_str());
			connection.SetStatus(Connection::CGI_ING);
			return ;
		}
	}
	else
	{
		int	fromCGI = open(fromCGIfileName.c_str(), O_RDONLY);
		if (fromCGI < 0)
		{
			throw 500;
		}
		struct stat	statBuf;
		if (fstat(fromCGI, &statBuf) == -1)
		{
			close(fromCGI);
			throw 500;
		}
		char*	buf = (char *)malloc(sizeof(char) * (statBuf.st_size + 1));
		if (buf == NULL)
		{
			close(fromCGI);
			throw 500;
		}
		int cnt = read(fromCGI, buf, statBuf.st_size);
		close(fromCGI); unlink(fromCGIfileName.c_str());
		// cout << "cnt: " << cnt << endl;
		buf[cnt] = 0;
		if (cnt < 0)
		{
			throw 500;
		}
		std::string	fromCgiStr(buf);
		free(buf); buf = NULL;

		// TODO REFACTORING
		if (connection.GetCgiProgramPath().find("php") != std::string::npos)
		{
			response->setBody(fromCgiStr);
			response->setHeaders("Content-Length", ft::itos(response->GetBody().length()));
			connection.SetStatus(Connection::SEND_READY);
			return ;
		}

		// STUB 파싱과정 필요
		std::size_t seek = fromCgiStr.find("\r\n\r\n", 0);
		if (seek == std::string::npos)
		{
			throw 500;
		}
		std::size_t seek_cur = 0;
		while (true)
		{
			std::size_t	seek = fromCgiStr.find("\r\n", seek_cur);
			if (seek == std::string::npos)
			{
				throw 500;
			}
			std::string	header = fromCgiStr.substr(seek_cur, seek - seek_cur);
			if (header.empty())
			{
				seek_cur += 2;
				break ;
			}
			std::size_t	found = header.find(": ");
			if (found == std::string::npos)
			{
				throw 500;
			}
			std::string	key = header.substr(0, found);
			std::locale loc;
			for(std::string::size_type i = 0; i < key.length(); i++)
			{
				key[i] = std::tolower(key[i], loc);
			}

			std::string	value = header.substr(found + 2);
			// REVIEW CGI program 에서 Chunked로 보내주기도 하나?
			if (key == "status")
			{
				std::size_t found = value.find(" ");
				std::string statusCode_str = value.substr(0, found);
				int statusCode = std::atoi(statusCode_str.c_str());
				if (statusCode != 200)
				{
					this->createResponseStatusCode(connection, statusCode);
					return ;
				}
			}
			else
			{
				connection.GetResponse()->setHeaders(key, value);
			}
			seek_cur = seek + 2;
		}
		std::size_t seek_body = seek_cur;
		response->setBody(fromCgiStr.substr(seek_body));
		response->setHeaders("Content-Length", ft::itos(response->GetBody().length()));
		connection.SetStatus(Connection::SEND_READY);
	}
}