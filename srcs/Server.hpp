#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <vector>
#include <queue>
#include <map>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>

#include "Location.hpp"
#include "Config.hpp"
#include "ServerManager.hpp"
class ServerManager;
// #include "Connection.hpp"
// #include "Request.hpp"
// #include "Response.hpp"

class Server
{
	public:
		Server(ServerManager* server_manager, const std::string& server_block, std::vector<std::string>& location_blocks, Config* config);

		// bool						hasException(int client_fd);
		// void						closeConnection(int client_fd);
		// void						isSendable(int client_fd);
		// Request						recvRequest(int client_fd);
		// void						sendResponse(Response response);
		// char**						createCGIEnv(void);
		// bool						hasNewConnection(void);
		// void						acceptNewConnection(void);


		// int							getUnuseConnectionFd();
		// bool						parseStartLine(Connection& connection, Request& request);
		// bool						parseHeader(Connection& connection, Request& request);
		// bool						parseBody(Connection& connection, Request& request);

		// bool						hasRequest(Connection& connection);
		// bool						runRecvAndSolve(Connection& connection);
		// bool						hasExecuteWork(Connection& connection);
		// bool						runExecute(Connection& connection);
		// bool						hasSendWork(Connection& connection);
		// bool						runSend(Connection& connection);

		// void						run(void);
		// void						solveRequest(Connection& connection, const Request& request);
		// void						executeAutoindex(Connection& connection, const Request& request);
		// void						executeGet(Connection& connection, const Request& request);
		// void						executeHead(Connection& connection, const Request& request);
		// void						executePost(Connection& connection, const Request& request);
		// void						executePut(Connection& connection, const Request& request);
		// void						executeDelete(Connection& connection, const Request& request);
		// void						executeOptions(Connection& connection, const Request& request);
		// void						executeTrace(Connection& connection, const Request& request);
		// void						executeCGI(Connection& connection, const Request& request);
		// void						createResponse(Connection& connection, int status, headers_t headers = headers_t(), std::string body = "");

		const int&					get_m_fd(void) const;

	private:
		ServerManager*				m_manager;
		// std::string					m_server_name;
		std::string					m_host;
		int							m_port;
		int							m_fd;
		// int							m_request_uri_limit_size;
		// int							m_request_header_limit_size;
		// int							m_limit_client_body_size;
		// std::string					m_default_error_page;
		Config*						m_config;
		std::vector<Location>		m_locations;
		// std::map<int, Connection>	m_connections;
		// std::queue<Response>		m_responses;
};

#endif