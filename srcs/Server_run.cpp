#include "Server.hpp"
#include "ServerManager.hpp"
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
				continue ;
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
	}
	if (hasNewConnection())
	{
		if (mConnections.size() >= (INIT_FD_MAX / mManager->GetServers().size()))
		{
			int fd = getUnuseConnectionFd();
			if (fd == -1)
				return ;
			closeConnection(fd);
		}
		if (acceptNewConnection() == false)
		{
			std::cout << "accept() Fail" << std::endl;
		}
		else
		{
			std::cout << "accept(): Success" << std::endl;
		}
	}
}

bool		Server::hasSendWork(Connection& connection)
{
	if (connection.GetRequest() == NULL)
		return (false);
	if (connection.GetStatus() == Connection::SEND_READY || connection.GetStatus() == Connection::SEND_ING)
	{
		if (indexKeventGet(connection.GetSocket(), EVFILT_WRITE) != -1)
		// if (this->mManager->mKevent_get[connection.GetSocket()].flags & EVFILT_WRITE)
		{
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
		response->setHttpMessage(response->makeHttpMessage());				// NOTE 보낼 response 만들어서, 앞으로 사용할 변수에 저장해서, 이 변수에서 뽑아내서 전송할꺼임!
		response->SetResponseLength(response->GetHttpMessage().length());	// NOTE 보낼 response 만들어서, 앞으로 사용할 변수에 저장해서, 이 변수에서 뽑아내서 전송할꺼임!
		int count = 1000000;
		int snd_buf= count * 1, rcv_buf= count * 3;
		int state;

		// NOTE  최적화1. 수신 버퍼의 크기 조절하기
		state = setsockopt(connection.GetSocket(), SOL_SOCKET, SO_RCVBUF, (void*)&rcv_buf, sizeof(rcv_buf)); // RECV buffer 늘리기
		state = setsockopt(connection.GetSocket(), SOL_SOCKET, SO_SNDBUF, (void*)&snd_buf, sizeof(snd_buf)); // SEND buffer 늘리기
		// NOTE  최적화1. Nagle 알고리즘 해제하기
		int opt_val = true;
		state = setsockopt(connection.GetSocket(), IPPROTO_TCP, TCP_NODELAY, (void *)&opt_val, sizeof(opt_val));
		connection.SetStatus(Connection::SEND_ING);
		return (false);
	}
	else if(connection.GetStatus() == Connection::SEND_ING)
	{
		ssize_t	count = write(connection.GetSocket(), response->GetHttpMessage().c_str() + response->GetSeek(), response->GetResponseLength());
		// NOTE length를 구하는 cost가 생각보다 엄청 크진 않았다. 위아래 코드 실행시간 차이가 거의 미미
		// ssize_t	count = write(connection.GetSocket(), response->GetHttpMessage().c_str() + response->GetSeek(), response->GetHttpMessage().length() - response->GetSeek());
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
	send_number++;
	std::cout << "send_nubmer: " << send_number << std::endl;
	std::cout << "fd_max: " << this->mManager->GetMaxFd() << std::endl;

	delete request;
	connection.SetRequest(NULL);
	delete response;
	connection.SetResponse(NULL);
	connection.SetStatus(Connection::REQUEST_READY);

	if (indexKeventGet(connection.GetSocket(), EVFILT_WRITE) != -1)
	// if (mManager->mKevent_set[connection.GetSocket()].filter & EVFILT_WRITE)
	{
		int temp;
		EV_SET(&mManager->mKevent_set[connection.GetSocket()], connection.GetSocket(), \
							EVFILT_WRITE, EV_DELETE, NULL, 0, NULL);
		temp = kevent(this->mManager->mKqueue, &mManager->mKevent_set[connection.GetSocket()], \
							1, NULL, 0, NULL);
		if (temp == -1)
		{
			std::cout << "kevent setting error " << std::endl;
			exit(1);
		}
		// mManager->mKevent_set[connection.GetSocket()].filter = (short)0 | EVFILT_READ;
		EV_SET(&mManager->mKevent_set[connection.GetSocket()], connection.GetSocket(), \
							EVFILT_READ, EV_ADD | EV_CLEAR, NULL, 0, NULL); // NOTE ㄷㅐ체 뭘까..? EV_CLEAR를 추가하면 안된다니 엣지 트리거?
		temp = kevent(this->mManager->mKqueue, &mManager->mKevent_set[connection.GetSocket()], \
							1, NULL, 0, NULL);
		if (temp == -1)
		{
			std::cout << "kevent setting error " << std::endl;
			exit(1);
		}
	}
	// mManager->ClrWriteFds(connection.GetSocket());
	// mManager->ClrWriteCopyFds(connection.GetSocket());
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
	if (indexKeventGet(connection.GetSocket(), EVFILT_READ) != -1)
	// if (this->mManager->mKevent_get[connection.GetSocket()].filter & (EVFILT_READ))
	{
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
		createResponseStatusCode(connection, statusCode);
		// mManager->SetWriteFds(connection.GetSocket());
		mManager->mKevent_set[connection.GetSocket()].filter = EVFILT_WRITE;
		// kevent(mManager->mKqueue, &mManager->mKevent_set[connection.GetSocket()], 1, NULL, 0, NULL);
		connection.SetStatus(Connection::SEND_READY);
		return (true);
	}
	catch (const std::exception& e)
	{
		throw (e);
	}

	try
	{
		Request& request = *connection.GetRequest();
		if (request.GetPhase() == Request::COMPLETE)
		{
			solveRequest(connection, *connection.GetRequest());
			return (true);
		}
		return (false);
	}
	catch (int statusCode)
	{
		createResponseStatusCode(connection, statusCode);
		connection.SetStatus(Connection::SEND_READY);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	return (false);
}

int			Server::indexKeventGet(int fd, int filter)
{
	for (int i = 0; i < this->mManager->GetMaxFd() + 1; i++)
	{
		if (this->mManager->mKevent_get[i].filter & filter \
				&& this->mManager->mKevent_get[i].ident == (unsigned long)fd)
			return (i);
	}
	return (-1);
}

bool		Server::hasNewConnection()
{
	if (indexKeventGet(this->mSocket, EVFILT_READ) != -1)
	// if (this->mManager->mKevent_get[this->mSocket].filter & (EVFILT_READ))
	{
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
		return (false);
	}
	fcntl(client_socket, F_SETFL, O_NONBLOCK);
	// mManager->SetReadFds(client_socket);
	// mManager->SetReadCopyFds(client_socket);
	// mManager->mKevent_set[client_socket].ident = client_socket;
	// mManager->mKevent_set[client_socket].filter = EVFILT_READ;
	// mManager->mKevent_set[client_socket].flags = EVFILT_READ;
	EV_SET(&mManager->mKevent_set[client_socket], client_socket, \
			EVFILT_READ, EV_ADD | EV_CLEAR, NULL, 0, NULL);
	int temp = kevent(mManager->mKqueue, &mManager->mKevent_set[client_socket], 1, NULL, 0, NULL);
	if (temp == -1)
	{
		std::cout << "what?: " << errno << std::endl;
		exit(1);
	}

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