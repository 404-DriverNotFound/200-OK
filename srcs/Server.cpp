#include "Server.hpp"
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/socket.h>

Server::Server(
	ServerManager* server_manager,
	const std::string& server_block,
	std::vector<std::string>& location_blocks,
	Config* config)
{
	if((m_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
	{
		throw std::runtime_error("SOCKET ERROR");
	}
	int value;	value = 1;
	if (setsockopt(m_fd, SOL_SOCKET, SO_REUSEADDR, &value, sizeof(int)) == -1)
	{
		throw std::runtime_error("SOCKET_OPTION ERROR");
	}
	sockaddr_in server_addr;
	bzero(&server_addr, sizeof(struct sockaddr_in));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl((uint32_t)m_host.c_str());
	server_addr.sin_port = htons(m_port);
	if(bind(m_fd, reinterpret_cast<struct sockaddr *>(&server_addr), sizeof(struct sockaddr)) == -1)
	{
		throw std::runtime_error("BIND ERROR");
	}
	if(listen(m_fd, 256) == -1)
	{
		throw std::runtime_error("LISTEN ERROR");
	}
	if (fcntl(m_fd, F_SETFL, O_NONBLOCK) == -1)
	{
		throw std::runtime_error("FCNTL ERROR");
	}
}

void	Server::run(void)
{
	std::map<int, Connection>::iterator	it = m_connections.begin();

	while (it != m_connections.end())
	{
		std::map<int, Connection>::iterator it2 = it++;
		int fd = it2->first;

		if (m_fd == fd)
			continue ;
			/* 커넥션들들을 순회하며 예외, 요청, 응답, 연결 작업 실행 */
			try {
				if (hasSendWork(it2->second)) // 얘가 어디서 날라온 애인가? 생각을 해봐야함.
				{
					runSend(it2->second);
					continue ;
				}
				if (hasExecuteWork(it2->second))
				{
					runExecute(it2->second);
					continue ;
				}
				if (hasRequest(it2->second)) {
					runRecvAndSolve(it2->second);
				}
			} catch (Server::IOError& e) {
				ft::log(ServerManager::log_fd, ft::getTimestamp() + e.location() + std::string("\n"));
				closeConnection(fd);
			} catch (...) {
				ft::log(ServerManager::log_fd, ft::getTimestamp() + "detected some error" + std::string("\n"));
				closeConnection(fd);
			}
	}
}

/* 해당 서버가 클라이언트로 전송할 때인지 판단해준다. */
bool	Server::hasSendWork(int connection)
{

}

/* 해당 서버가 클라이언트로 실행할 때인지 판단해준다. */
bool	Server::hasExecuteWork(int connection)
{

}

/* 해당 서버가 클라이언트로부터	request 받을 때인지 판단해준다. */
bool	Server::hasRequest(int connection)
{

}

void	Server::runSend(int connection)
{

}

void	Server::runExecute(int connection)
{

}

void	Server::runSend(int connection)
{

}

// // ref : https://42seoul.gitbook.io/webserv/or-server-control
// 	if (hasNewConnection())
// 	{
// 		if (m_connections.size() >= (1024 / m_manager->get_m_servers().size()))
// 		{
// 			int fd = getUnuseConnectionFd();
// 			if (fd == -1)
// 			{
// 				return ;
// 			}
// 			closeConnection(fd);
// 		}
// 		if (!acceptNewConnection())
// 		{
// 			reportCreateNewConnectionLog();
// 		}
// 	}

/* Requset 이후에 이것을 실행한다 */
bool	Server::runRecvAndSolve(Connection& connection)
{
	try
	{
		recvRequest(connection, connection.get_m_request());
	}
	catch (int status_code)
	{
		createResponse(connection, status_code);
		return (true);
	}
	catch (Server::IOError& e)
	{
		throw (e);
	}
	catch (std::exception& e)
	{
		ft::log(ServerManager::log_fd, std::string("[Failed][Request] Failed to create request because ") + e.what());
		createResponse(connection, 50001);
		return (true);
	}

	const Request&	request = connection.get_m_request();

	if (request.get_m_phase() == Request::COMPLETE)
	{
		writeCreateNewRequestLog(request);
		connection.set_m_status(Connection::ON_EXECUTE);
		solveRequest(connection, connection.get_m_request());
		return (true);
	}
	return (false);
}

/* recvRequest */
Request						recvRequest(int client_fd)
{

}
