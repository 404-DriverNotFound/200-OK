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
#include <cstring>
#include <netinet/tcp.h>

#include "Define.hpp"
#include "Path/Path.hpp"
#include "Utils/utils.hpp"
#include "Connection.hpp"

class ServerManager;
class Request;
class Response;
struct configIterator;

extern char**	gEnv;
extern int		gTotalClients;

class LocationPath
{
public:
	LocationPath(void);
	virtual ~LocationPath(void);
	LocationPath(const LocationPath& ref);
	LocationPath&				operator=(const LocationPath& ref);

public:
	Path						mLocationPath;		// def = 
	Path						mRoot;				// def = 
	std::vector<Path>			mIndexPages;		// def = index.html
	Path						mErrorPage;			// def = error.html
	bool						mAutoIndex;
	size_t						mClientMaxBodySize;	//def = 0
	Path						mAuthBasicUserFile;	//def =

	std::vector<std::string>	mMethod;
	std::vector<std::string>	mCgiExtension;
};

class serverBlock
{
public :
	serverBlock(void);
	virtual ~serverBlock(void);
	serverBlock(const serverBlock& ref);
	serverBlock&				operator=(const serverBlock& ref);

public :
	std::vector<LocationPath>	mlocationPaths;
	std::string					mserverName;
	int							mTimeOut;
};

class Server // NOTE port별로 나뉘는 블록
{
public:
	Server(void);
	Server(ServerManager* manager);
	virtual ~Server(void);
	Server(const Server& ref);
	Server&								operator=(const Server& ref);

	int SetSocket(void);

	void								recvRequest(Connection& connection);
	char**								createCGIEnv(const Connection& connection) const;
	bool								hasNewConnection(void);
	bool								acceptNewConnection(void);
	int									getUnuseConnectionFd();
	void								closeConnection(int clientFd);

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
	void								executeAutoindex(Connection& connection, std::string uriCopy); // NOTE 살짝 변형함.
	void								executeGet(Connection& connection, std::string targetUri);
	void								executeHead(Connection& connection, std::string targetUri);
	void								executePost(Connection& connection, const Request& request);
	void								executePut(Connection& connection, const Request& request, std::string targetUri);
	void								executeDelete(Connection& connection, const Request& request, std::string targetUri);
	void								executeOptions(Connection& connection, configIterator configIterator);
	void								aexecuteTrace(Connection& connection);
	void								executeCGI(Connection& connection);
	void								createResponseStatusCode(Connection& connection, int statusCode);
	void								createResponse0(Connection &connection, std::string uriPlusFile);
	void								createResponse200(Connection &connection, std::string targetUri);

	const int&							GetSocket(void) const;

	//ANCHOR yunslee
	std::vector<serverBlock>&			getServerBlocks(void);
	bool isValidMethod(Request& request, configIterator configIterator);

	std::vector<serverBlock>::iterator	returnIteratorServerBlock(std::vector<serverBlock> &serverBlocks, std::string serverName);
	std::vector<LocationPath>::iterator	returnIteratorLocationPath(std::vector<LocationPath> &locationPaths, std::string locationPathStr);


	class IOError : public std::exception
	{
		public:
			const char* what() const throw();
	};

private:
	bool								hasAuthModule(const configIterator& config_it);
	bool								isRightCredentials(const std::string& authorization);

public :
	ServerManager*						mManager;
	uint16_t							mPort; // def = 8000;
	std::string							mHost; // def = "0.0.0.0";
	std::vector<serverBlock>			mServerBlocks;
	int									mSocket; // NOTE mConnections의 첫번째 값이 모두 서버소켓의 fd임!
	std::map<int, Connection>			mConnections;
};

struct configIterator
{
	std::vector<serverBlock>::iterator	serverBlock;
	std::vector<LocationPath>::iterator	locationPath;
};


#endif