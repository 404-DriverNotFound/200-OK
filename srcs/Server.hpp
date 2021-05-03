#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <vector>
#include <queue>
#include <map>
#include "Location.hpp"
#include "Config.hpp"
#include "ServerManager.hpp"
#include "Connection.hpp"
#include "Request.hpp"
#include "Response.hpp"

class Server
{
public:
	Server(ServerManager*, Server server_block, Location location_blocks, Config*);

	bool						hasException(int client_fd);
	void						closeConnection(int client_fd);
	void						isSendable(int client_fd);
	void						sendResponse(Response response);
	void						hasRequest(int client_fd);
	Request						recvRequest(int client_fd);
	void						solveRequest(Request request);
	void						executeAutoindex(void);
	void						executeGet(Request request);
	void						executeHead(Request request);
	void						executePut(Request request);
	void						executePost(Request request);
	void						executeDelete(Request request);
	void						executeOptions(Request request);
	void						executeTrace(Request request);
	char**						createCGIEnv(void);
	void						executeCGI(Request request);
	void						createResponse(int status_code);
	bool						hasNewConnection(void);
	void						acceptNewConnection(void);
	void						run(void);



private:
	ServerManager*				m_manager;
	std::string					m_server_name;
	std::string					m_host;
	int							m_port;
	int							m_fd;
	int							m_request_uri_limit_size;
	int							m_request_header_limit_size;
	int							m_limit_client_body_size;
	std::string					m_default_error_page;
	Config*						m_config;
	std::vector<Location>		m_locations;
	std::map<int, Connection>	m_connections;
	std::queue<Response>		m_responses;
};

#endif