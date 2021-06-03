#include "Server.hpp"
#include "ServerManager.hpp" // NOTE 상호참조 문제를 해결하기 위해서!
#include "Response.hpp"

void		Server::run(void)
{
	std::map<int, Connection>::iterator	it = mConnections.begin();
	while (it != mConnections.end())
	{
		std::map<int, Connection>::iterator it2 = it++;
		int fd = this->mSocket;
		if (it2->second.GetSocket() == fd)
		{
			continue ;
		}
		try
		{
			if (hasSendWork(it2->second))
			{
				struct timeval	time;
				gettimeofday(&time, NULL);
				it2->second.SetLastReqeustAt(time);
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
				if (it2->second.GetRequest() == NULL)
				{
					it2->second.SetRequest(new Request());
				}
				runRecvAndSolve(it2->second);
			}
		}
		catch (const std::exception& e)
		{
			closeConnection(it2->second.GetSocket());
		}

		// STUB 예시 코드
		// } catch (Server::IOError& e) {
		// 	ft::log(ServerManager::log_fd, ft::getTimestamp() + e.location() + std::string("\n"));
		// 	closeConnection(fd);
		// } catch (...) {
		// 	ft::log(ServerManager::log_fd, ft::getTimestamp() + "detected some error" + std::string("\n"));
		// 	closeConnection(fd);				
		// }
	}
	// cout << "mConnections.size()" << mConnections.size() << endl;
	if (hasNewConnection())
	{
		if (mConnections.size() >= (INIT_FD_MAX / mManager->GetServers().size()))
		{
			int fd = getUnuseConnectionFd();
			if (fd == -1)
				return ;
			closeConnection(fd);
		}
		
		if (false == acceptNewConnection())
		{
			// cout << "accpetNewconnection(): Fail" << endl;
			// reportCreateNewConnectionLog();
		}
		else
		{
			// cout << "accpetNewconnection(): Success" << endl;
		}
	}
	// cout << this->mPort << "'s connection_size: "<< mConnections.size() << endl; 
}

bool		Server::hasSendWork(Connection& connection)
{
	// value = connection.GetRequest()->GetPhase();
	// TODO COMPLETE로 가정하였으나, 실제로는 request의 진행상황에 따라서 { READY, ON_HEADER, ON_BODY, COMPLETE }; 단계로 나뉨.
	if (connection.GetRequest() == NULL)
		return (false);
	
	if (connection.GetStatus() == Connection::SEND_READY || connection.GetStatus() == Connection::SEND_ING)
	{
		if (FT_FD_ISSET(connection.GetSocket(), &(this->mManager->GetWriteCopyFds())))
		{
			// closeConnection(connection.GetSocket());
			return (true);
		}
		return (false);
	}
	else
	{
		return (false);
	}
}

bool		Server::runSend(Connection& connection)
{
	static int send_number = 0;
	Request *request = connection.GetRequest();
	Response *response = connection.GetResponse();

	if (connection.GetStatus() == Connection::SEND_READY)
	{
		response->setHttpMessage(response->makeHttpMessage()); // NOTE 보낼 response 만들어서, 앞으로 사용할 변수에 저장해서, 이 변수에서 뽑아내서 전송할꺼임!
		response->SetResponseLength(response->GetHttpMessage().length()); // NOTE 보낼 response 만들어서, 앞으로 사용할 변수에 저장해서, 이 변수에서 뽑아내서 전송할꺼임!
		errno = 0;


		int count = 1000000;
		int snd_buf= count * 1, rcv_buf= count * 3;
		int state;
		// NOTE  최적화1. 수신 버퍼의 크기 조절하기
		state = setsockopt(connection.GetSocket(), SOL_SOCKET, SO_RCVBUF, (void*)&rcv_buf, sizeof(rcv_buf)); // RECV buffer 늘리기
		state = setsockopt(connection.GetSocket(), SOL_SOCKET, SO_SNDBUF, (void*)&snd_buf, sizeof(snd_buf)); // SEND buffer 늘리기
		// cout << "state: " << state << endl;

		// NOTE  최적화1. Nagle 알고리즘 해제하기
		int opt_val = true;
		state = setsockopt(connection.GetSocket(), IPPROTO_TCP, TCP_NODELAY, (void *)&opt_val, sizeof(opt_val));
		// cout << "state: " << state << endl;

		// perror("what?:");
		errno = 0;

		connection.SetStatus(Connection::SEND_ING);
		return (false);

	}
	else if(connection.GetStatus() == Connection::SEND_ING)
	{
		errno = 0;
		// perror("what?:");
		ssize_t	count = write(connection.GetSocket(), response->GetHttpMessage().c_str() + response->GetSeek(), response->GetResponseLength());
		
		// NOTE length를 구하는 cost가 생각보다 엄청 크진 않았다. 위아래 코드 실행시간 차이가 거의 미미
		//ssize_t	count = write(connection.GetSocket(), response->GetHttpMessage().c_str() + response->GetSeek(), response->GetHttpMessage().length() - response->GetSeek());

		if (count <= 0)
		{
			throw IOError();
		}
		response->SetSeek(response->GetSeek() + count);
		response->SetResponseLength(response->GetResponseLength() - count);
		if (response->GetResponseLength() != 0)
		{
			return (false);
		}
	}

	int	statusCode = response->getStatusCode();
	if (statusCode >= 200 && statusCode < 300)
	{
		std::cout << GRN;
	}
	else
	{
		std::cout << RED;
	}
	std::cout << "[" << ft::getHTTPTimeFormat(request->GetStartTime().tv_sec) << "][access][" << connection.GetClientIp() << ":" << connection.GetClientPort() << "]";
	std::cout << "[" << request->GetMethod() << "][" << response->getStatusCode() << " " << response->mStatusMap[statusCode] << "]" NC << std::endl;

	// request->ShowMessage(); // ANCHOR request message debugging 용
	// response->ShowMessage(); // ANCHOR response message debugging 용

	send_number++;
	std::cout << "send_nubmer: " << send_number << std::endl;
	std::cout << "fd_max: " << this->mManager->GetMaxFd() << std::endl;

	// ANCHOR 작업중
	delete request;
	connection.SetRequest(NULL);
	delete response;
	connection.SetResponse(NULL);
	connection.SetStatus(Connection::REQUEST_READY);
	mManager->ClrWriteFds(connection.GetSocket());
	mManager->ClrWriteCopyFds(connection.GetSocket());
	// FD_CLR(connection.GetSocket(), &(this->mManager->GetWriteFds()));
	// FD_CLR(connection.GetSocket(), &(this->mManager->GetWriteCopyFds()));
	// closeConnection(connection.GetSocket());
	// ANCHOR 작업중
	if (statusCode >= 400)
	{
		closeConnection(connection.GetSocket());
	}
	return (true);
}

bool		Server::hasExecuteWork(const Connection& connection) const
{
	if (connection.GetStatus() == Connection::CGI_READY ||
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
	// TODO cgi 여부에 따라 걸러주는 로직이 있어야함 cgi 아니면 send ready
	try
	{
		if (connection.GetRequest()->GetURItype() == Request::CGI_PROGRAM)
		{
			executeCGI(connection);
			return (true);
		}
		else
		{
			connection.SetStatus(Connection::SEND_READY);
			return (false);
		}
	}
	catch (int statusCode)
	{
		// std::cout << "runExecute catch: " << statusCode << std::endl;
		createResponseStatusCode(connection, statusCode);
		connection.SetStatus(Connection::SEND_READY);
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	return (false);
}

bool		Server::hasRequest(const Connection& connection)
{
	if (FT_FD_ISSET(connection.GetSocket(), &(this->mManager->GetReadCopyFds()))) // REVIEW	request의 phase도 함께 확인해야할 수도 있을 듯
	{
		// std::cout << "client(" << connection.GetSocket() << ") : has request" << std::endl;
		return (true);
	}
	else
	{
		return (false);
	}
}

bool		Server::runRecvAndSolve(Connection& connection)
{
	try
	{
		recvRequest(connection);
	}
	catch (int statusCode)
	{
		// std::cout << "runRecvAndSolve catch: " << statusCode << std::endl;
		createResponseStatusCode(connection, statusCode);
		mManager->SetWriteFds(connection.GetSocket());
		connection.SetStatus(Connection::SEND_READY);
		return (true);
	}
	catch (const Server::IOError& e)
	{
		throw (e);
	}
	catch (const std::exception& e)
	{
		// ft::log(ServerManager::log_fd, std::string("[Failed][Request] Failed to create request because ") + e.what());
		// createResponse(connection, 50001);
	}

	try
	{
		Request& request = *connection.GetRequest();
		if (request.GetPhase() == Request::COMPLETE)
		{
		// 	writeCreateNewRequestLog(request);
		// 	connection.set_m_status(Connection::ON_EXECUTE); //REVIEW 이게 맞나?
			solveRequest(connection, *connection.GetRequest());
			return (true);
		}
		return (false);
	}
	catch (int statusCode)
	{
		// std::cout << "runRecvAndSolve catch: " << statusCode << std::endl;
		createResponseStatusCode(connection, statusCode);
		connection.SetStatus(Connection::SEND_READY);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	return (false);
}

bool		Server::hasNewConnection()
{
	if (FT_FD_ISSET(this->mSocket, &(this->mManager->GetReadCopyFds())))
	{
		// cout << "this->mSocket: " << this->mSocket << endl;
		return (true);
	}
	else
	{
		return (false);
	}
}

bool		Server::acceptNewConnection()
{
	sockaddr_in	sockaddr;
	socklen_t	socketlen = sizeof(struct sockaddr);
	int			client_socket = accept(this->mSocket, reinterpret_cast<struct sockaddr*>(&sockaddr), reinterpret_cast<socklen_t*>(&socketlen));
	if (client_socket == -1)
	{
		// std::cerr << "Could not create socket." << std::endl;
		return (false);
	}
	fcntl(client_socket, F_SETFL, O_NONBLOCK);
	mManager->SetReadFds(client_socket);
	mManager->SetReadCopyFds(client_socket);
	// FD_SET(client_socket, &(this->mManager->GetReadFds()));
	// FD_SET(client_socket, &(this->mManager->GetReadCopyFds()));
	// FD_SET(client_socket, &(this->mManager->GetWriteFds()));
	// FD_SET(client_socket, &(this->mManager->GetWriteCopyFds()));
	this->mConnections[client_socket] = Connection(client_socket, ft::inet_ntos(sockaddr.sin_addr), this->mPort);
	// this->mConnections[client_socket] = Connection(client_socket, this->mHost, this->mPort); // NOTE 이것도 됨
	gTotalClients++;
	if (this->mManager->GetMaxFd() < client_socket)
	{
		this->mManager->SetMaxFd(client_socket);
	}

	std::cerr << GRNB "[" << ft::getCurrentTime() << "][connection]" << "[ESTABLISHED]" << "[" << client_socket << "]" << NC << std::endl;
	// close(client_socket); // NOTE 이제 keep-alive로 관리
	return (true);
}