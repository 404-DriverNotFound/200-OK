#include "Server.hpp"
#include "ServerManager.hpp" // NOTE 상호참조 문제를 해결하기 위해서!
#include "Response.hpp"

extern char**	g_env;

void		Server::run(void)
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
					it2->second.set_m_last_reqeust_at(it2->second.get_m_request()->GetStartTime());
				}
				runRecvAndSolve(it2->second);
			}
		}
		catch (const std::exception& e)
		{
			closeConnection(it2->second.get_m_fd());
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
	// cout << "m_connections.size()" << m_connections.size() << endl;
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
			// cout << "accpetNewconnection(): Fail" << endl;
			// reportCreateNewConnectionLog();
		}
		else
		{
			// cout << "accpetNewconnection(): Success" << endl;
		}
	}
	// cout << this->mport << "'s connection_size: "<< m_connections.size() << endl; 
}

bool		Server::hasSendWork(Connection& connection)
{
	// value = connection.get_m_request()->GetPhase();
	// TODO COMPLETE로 가정하였으나, 실제로는 request의 진행상황에 따라서 { READY, ON_HEADER, ON_BODY, COMPLETE }; 단계로 나뉨.
	if (connection.get_m_request() == NULL)
		return (false);
	
	if (connection.GetStatus() == Connection::SEND_READY || connection.GetStatus() == Connection::SEND_ING)
	{
		if (FT_FD_ISSET(connection.get_m_fd(), &(this->m_manager->GetWriteCopySet())) <= 0)
		{
			closeConnection(connection.get_m_fd());
			return (false);
		}
		return (true);
	}
	else
	{
		return (false);
	}
}

bool		Server::runSend(Connection& connection)
{
	static int send_number = 0;
	int clinet_socket = connection.get_m_fd();
	Request *request = connection.get_m_request();
	Response *response = connection.get_m_response();
	bool send_complete = false;

	if (connection.GetStatus() == Connection::SEND_READY)
	{
		response->set_m_response(response->makeResponse()); // NOTE 보낼 response 만들어서, 앞으로 사용할 변수에 저장해서, 이 변수에서 뽑아내서 전송할꺼임!
		errno = 0;

		int count = 1000000;
		int snd_buf= count * 1, rcv_buf= count * 3;
		int state;

		// NOTE  최적화1. 수신 버퍼의 크기 조절하기
		// state=setsockopt(connection.get_m_fd(), SOL_SOCKET, SO_RCVBUF, (void*)&rcv_buf, sizeof(rcv_buf)); // RECV buffer 늘리기
		// state = setsockopt(connection.get_m_fd(), SOL_SOCKET, SO_SNDBUF, (void*)&snd_buf, sizeof(snd_buf)); // SEND buffer 늘리기
		// cout << "state: " << state << endl;

		// NOTE  최적화1. Nagle 알고리즘 해제하기
		int opt_val = true;
		// state = setsockopt(connection.get_m_fd(), IPPROTO_TCP, TCP_NODELAY, (void *)&opt_val, sizeof(opt_val));
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
		int write_size = write(connection.get_m_fd(), response->get_m_response().c_str(), response->get_m_response().length());
		if (write_size != response->get_m_response().length())
		{
			// cout << "write_size: " << write_size << endl;
			response->set_m_response(response->get_m_response().substr(write_size));
			// cout << "now_length: " << response->get_m_response().length() << endl;
			return (false);
		}
	}

	int	statusCode = response->get_m_status_code();
	if (statusCode >= 200 && statusCode < 300)
	{
		std::cout << GRN;
	}
	else
	{
		std::cout << RED;
	}
	std::cout << "[" << ft::getHTTPTimeFormat(request->GetStartTime().tv_sec) << "][access][" << connection.get_m_client_ip() << ":" << connection.get_m_client_port() << "]";
	std::cout << "[" << request->GetMethod() << "][" << response->get_m_status_code() << " " << response->m_status_map[statusCode] << "]" NC << std::endl;

	// request->ShowMessage(); // ANCHOR request message debugging 용
	// response->ShowMessage(); // ANCHOR response message debugging 용

	send_number++;
	cout << "send_nubmer: " << send_number << endl;

	// ANCHOR 작업중
	delete request;
	connection.set_m_request(NULL);
	delete response;
	connection.set_m_response(NULL);
	connection.SetStatus(Connection::REQUEST_READY);
	FT_FD_CLR(connection.get_m_fd(), &(this->m_manager->GetWriteSet()));
	// closeConnection(connection.get_m_fd());
	// ANCHOR 작업중

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
		if (connection.get_m_request()->GetURItype() == Request::CGI_PROGRAM)
		{
		executeCGI(connection, *(connection.get_m_request()));
		return (true);
		}
		else
		{
			connection.SetStatus(Connection::SEND_READY);
			return (false);
		}
	}
	catch (int status_code)
	{
		// std::cout << "runExecute catch: " << status_code << std::endl;
		create_Response_statuscode(connection, status_code);
		connection.SetStatus(Connection::SEND_READY);
		return (false);
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
}

bool		Server::hasRequest(const Connection& connection)
{
	if (FT_FD_ISSET(connection.get_m_fd(), &(this->m_manager->GetReadCopySet()))) // REVIEW	request의 phase도 함께 확인해야할 수도 있을 듯
	{
		// std::cout << "client(" << connection.get_m_fd() << ") : has request" << std::endl;
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
	catch (int status_code)
	{
		// std::cout << "runRecvAndSolve catch: " << status_code << std::endl;
		create_Response_statuscode(connection, status_code);
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
		// std::cout << "runRecvAndSolve catch: " << status_code << std::endl;
		create_Response_statuscode(connection, status_code);
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
	if (FT_FD_ISSET(this->msocket, &(this->m_manager->GetReadCopySet())))
	{
		// cout << "this->msocket: " << this->msocket << endl;
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
	int			client_socket = accept(this->msocket, reinterpret_cast<struct sockaddr*>(&sockaddr), reinterpret_cast<socklen_t*>(&socketlen));
	if (client_socket == -1)
	{
		// std::cerr << "Could not create socket." << std::endl;
		return (false);
	}
	fcntl(client_socket, F_SETFL, O_NONBLOCK);
	FT_FD_SET(client_socket, &(this->m_manager->GetReadSet()));
	FT_FD_SET(client_socket, &(this->m_manager->GetWriteSet()));
	this->m_connections[client_socket] = Connection(client_socket, ft::inet_ntos(sockaddr.sin_addr), this->mport);
	// close(client_socket); // NOTE 이제 keep-alive로 관리
	return (true);
}
