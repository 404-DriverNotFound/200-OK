#ifndef SERVER_HPP
#define SERVER_HPP

#include "Define.hpp"

#include <string>
#include <cstring>
#include <vector>
#include <queue>
#include <map>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <iostream>

#include "Location.hpp"
#include "Config.hpp"
#include "Path/Path.hpp"
#include "Utils/utils.hpp"
#include "Connection.hpp"


// #include "ServerManager.hpp"

using namespace std;

class ServerManager;
class Request;
class Response;

class LocationPath
{
public:
	LocationPath();
	virtual ~LocationPath();
	LocationPath(const LocationPath &);
	LocationPath&	operator=(const LocationPath &);

public:
	Path				mlocationPath;		// def = 
	Path				mroot;				// def = 
	std::vector<Path>	mindex_pages;		// def = index.html
	Path				merror_page;		// def = error.html
};

class ServerBlock
{
public :
	ServerBlock();
	virtual ~ServerBlock();
	ServerBlock(const ServerBlock &);
	ServerBlock&	operator=(const ServerBlock &);

public :
	std::vector<LocationPath>	mlocationPaths;
	std::string					mserverName;
	int							mtimeout;
	bool						mauto_index;
};

class Server // NOTE port별로 나뉘는 블록
{
public:
	Server(void);
	Server(ServerManager *);
	virtual ~Server();
	Server(const Server &);
	Server&	operator=(const Server &);

	int SetSocket(std::string ip, uint16_t port);

	// ANCHOR 참고 코드
	// bool						hasException(int client_fd);
	// void						isSendable(int client_fd);
	void						recvRequest(Connection& connection);
	ssize_t						recvWithoutBody(Connection& connection, void* buf, size_t nbyte);
	ssize_t						recvBody(Connection& connection, void* buf, size_t nbyte);
	// void						sendResponse(Response response);
	// char**						createCGIEnv(void);
	bool						hasNewConnection(void);
	bool						acceptNewConnection(void);
	int							getUnuseConnectionFd();
	void						closeConnection(int client_fd);

	bool						parseStartLine(Connection& connection);
	bool						parseHeader(Connection& connection);
	bool						parseBody(Connection& connection);

	bool						isRequestHasBody(Request *);
	bool						hasRequest(const Connection& connection);
	bool						runRecvAndSolve(Connection& connection);
	// bool						hasExecuteWork(Connection& connection);
	// bool						runExecute(Connection& connection);
	bool						hasSendWork(Connection& connection);
	bool						runSend(Connection& connection);

	void						run(void);
	void						solveRequest(Connection& connection, const Request& request); // NOTE reponse를 만드는 함수. Method, autoindex etc...
	// void						executeAutoindex(Connection& connection, const Request& request);
	void						executeAutoindex(Connection& connection, const Request& request, std::string uri_copy); // NOTE 살짝 변형함.
	void						executeGet(Connection& connection, const Request& request);
	void						executeHead(Connection& connection, const Request& request);
	void						executePost(Connection& connection, const Request& request);
	void						executePut(Connection& connection, const Request& request);
	void						executeDelete(Connection& connection, const Request& request);
	void						executeOptions(Connection& connection, const Request& request);
	void						executeTrace(Connection& connection, const Request& request);
	// void						executeCGI(Connection& connection, const Request& request);
	void						create_errorpage_Response(Connection& connection, int status_code);

	const int&					get_m_fd(void) const;

	//ANCHOR yunslee
	std::vector<ServerBlock>&	get_m_serverBlocks(void);
	// bool						isHostname_IN_server_name(std::vector<ServerBlock> &serverblocks, std::string hostname);

	// std::vector<Server>::iterator return_iterator_server(std::vector<Server> servers);
	std::vector<ServerBlock>::iterator return_iterator_serverblock(std::vector<ServerBlock> &serverblocks, std::string hostname);
	std::vector<LocationPath>::iterator return_iterator_locationpathlocationPath(std::vector<LocationPath> &locationpaths, std::string locationpath_str);


	class ClientServerClose : public std::exception
	{
		public:
			const char* what() const throw();
	};

public :
	ServerManager*				m_manager;
	uint16_t					mport; // def = 8000;
	std::vector<ServerBlock>	mserverBlocks;

	int							msocket; // NOTE m_connections의 첫번째 값이 모두 서버소켓의 fd임!

	// ANCHOR 참고코드
	// REVIEW 어떻게 적용될지 생각해봐야함
	std::map<int, Connection>	m_connections;
	// std::queue<Response>		m_responses;
	// Config*						m_config;
};

#endif