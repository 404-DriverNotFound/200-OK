#ifndef SERVER_HPP
#define SERVER_HPP

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
#include <netinet/tcp.h>
#include "Define.hpp"
#include "Path/Path.hpp"
#include "Utils/utils.hpp"
#include "Connection.hpp"

extern char**					g_envp;

class ServerManager;
class Request;
class Response;
struct config_iterator;

class LocationPath
{
public:
	LocationPath(void);
	LocationPath(const LocationPath& ref);
	virtual ~LocationPath(void);
	LocationPath&				operator=(const LocationPath& ref);

public:
	Path						mLocationPath;		// def = 
	Path						mRoot;				// def = 
	std::vector<Path>			mIndexPages;		// def = index.html
	Path						mErrorPage;		// def = error.html
	bool						mAutoIndex;
	size_t						mClientMaxBodySize;

	std::vector<std::string>	mMethods;
	std::vector<std::string>	mCgiExtensions;
};

class ServerBlock
{
public :
	ServerBlock(void);
	ServerBlock(const ServerBlock& ref);
	virtual ~ServerBlock(void);
	
	ServerBlock&				operator=(const ServerBlock& ref);

public :
	std::vector<LocationPath>	mLocationPaths;
	std::string					mServerName;
	int							mTimeOut;
};

class Server // NOTE port별로 나뉘는 블록
{
public:
	Server(void);
	Server(ServerManager* manager);
	virtual ~Server();
	Server(const Server& ref);
	Server&								operator=(const Server &);

	int									SetSocket(std::string ip, uint16_t port);

	// ANCHOR 참고 코드
	void								recvRequest(Connection& connection);
	char**								createCGIEnv(const Connection& connection) const;
	bool								hasNewConnection(void);
	bool								acceptNewConnection(void);
	int									getUnuseConnectionFd();
	void								closeConnection(int client_fd);

	bool								parseStartLine(Connection& connection);
	bool								parseHeader(Connection& connection);
	bool								parseBody(Connection& connection);

	bool								isRequestHasBody(Request *);
	bool								hasRequest(const Connection& connection);
	bool								runRecvAndSolve(Connection& connection);
	bool								hasExecuteWork(const Connection& connection) const;
	bool								runExecute(Connection& connection);
	bool								hasSendWork(Connection& connection);
	bool								runSend(Connection& connection);

	void								run(void);
	void								solveRequest(Connection& connection, Request& request); // NOTE reponse를 만드는 함수. Method, autoindex etc...
	// void								executeAutoindex(Connection& connection, const Request& request);
	void								executeAutoindex(Connection& connection, std::string uri_copy); // NOTE 살짝 변형함.
	void								executeGet(Connection& connection, std::string target_uri);
	void								executeHead(Connection& connection, std::string target_uri);
	void								executePost(Connection& connection, const Request& request);
	void								executePut(Connection& connection, const Request& request, std::string target_uri);
	void								executeDelete(Connection& connection, const Request& request, std::string target_uri);
	void								executeOptions(Connection& connection, std::string target_uri, config_iterator config_it);
	void								aexecuteTrace(Connection& connection);
	void								executeCGI(Connection& connection);
	void								create_Response_statuscode(Connection& connection, int status_code);
	void								create_Response_0(Connection &connection, std::string uri_plus_file);
	void								create_Response_200(Connection &connection, std::string uri_plus_file, TYPE_HTML type);

	const int&							GetSocket(void) const;

	//ANCHOR yunslee
	std::vector<ServerBlock>&			getServerBlocks(void);
	bool 								isValidMethod(Request &request, config_iterator config_it);

	// std::vector<Server>::iterator return_iterator_server(std::vector<Server> servers);
	std::vector<ServerBlock>::iterator	returnIteratorServerBlock(std::vector<ServerBlock>& serverblocks, std::string hostname);
	std::vector<LocationPath>::iterator	returnIteratorLocationPath(std::vector<LocationPath>& locationpaths, std::string locationpath_str);

	class IOError : public std::exception
	{
		public:
			const char* what() const throw();
	};

public :
	ServerManager*						mManager;
	uint16_t							mPort; // def = 8000;
	std::vector<ServerBlock>			mServerBlocks;
	int									mSocket; // NOTE mConnections의 첫번째 값이 모두 서버소켓의 fd임!
	std::map<int, Connection>			mConnections;
};

struct config_iterator
{
	std::vector<ServerBlock>::iterator	serverBlock;
	std::vector<LocationPath>::iterator	locationPath;
};

#endif