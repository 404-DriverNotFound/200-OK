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
#include <cstring>
#include <netinet/tcp.h>

#include "Path/Path.hpp"
#include "Utils/utils.hpp"
#include "Connection.hpp"


// #include "ServerManager.hpp"

using namespace std;

class ServerManager;
class Request;
class Response;
struct configIterator;

class LocationPath
{
public:
	LocationPath();
	virtual ~LocationPath();
	LocationPath(const LocationPath &);
	LocationPath&	operator=(const LocationPath &);

public:
	Path				mLocationPath;		// def = 
	Path				mRoot;				// def = 
	std::vector<Path>	mIndexPages;		// def = index.html
	Path				mErrorPage;		// def = error.html
	bool				mAutoIndex;
	size_t				mClientMaxBodySize;		//def = 0
	Path				mAuthBasicUserFile;	//def =

	std::vector<std::string>	mMethod;
	std::vector<std::string>	mCgiExtension;
};

class serverBlock
{
public :
	serverBlock();
	virtual ~serverBlock();
	serverBlock(const serverBlock &);
	serverBlock&	operator=(const serverBlock &);

public :
	std::vector<LocationPath>	mlocationPaths;
	std::string					mserverName;
	int							mTimeOut;
};

class Server // NOTE port별로 나뉘는 블록
{
public:
	Server(void);
	Server(ServerManager *);
	virtual ~Server();
	Server(const Server &);
	Server&	operator=(const Server &);

	int SetSocket();

	// ANCHOR 참고 코드
	// bool						hasException(int clientFd);
	// void						isSendable(int clientFd);
	void						recvRequest(Connection& connection);
	// void						sendResponse(Response response);
	char**						createCGIEnv(const Connection& connection) const;
	bool						hasNewConnection(void);
	bool						acceptNewConnection(void);
	int							getUnuseConnectionFd();
	void						closeConnection(int clientFd);

	bool						parseStartLine(Connection& connection);
	bool						parseHeader(Connection& connection);
	bool						parseBody(Connection& connection);

	bool						isRequestHasBody(Request *);
	bool						hasRequest(const Connection& connection);
	bool						runRecvAndSolve(Connection& connection);
	bool						hasExecuteWork(const Connection& connection) const;
	bool						runExecute(Connection& connection);
	bool						hasSendWork(Connection& connection);
	bool						runSend(Connection& connection);

	void						run(void);
	void						solveRequest(Connection& connection, Request& request); // NOTE reponse를 만드는 함수. Method, autoindex etc...
	// void						executeAutoindex(Connection& connection, const Request& request);
	void						executeAutoindex(Connection& connection, std::string uriCopy); // NOTE 살짝 변형함.
	void						executeGet(Connection& connection, std::string targetUri);
	void						executeHead(Connection& connection, std::string targetUri);
	void						executePost(Connection& connection, const Request& request);
	void						executePut(Connection& connection, const Request& request, std::string targetUri);
	void						executeDelete(Connection& connection, const Request& request, std::string targetUri);
	void						executeOptions(Connection& connection, std::string targetUri, configIterator configIterator);
	void						aexecuteTrace(Connection& connection);
	void						executeCGI(Connection& connection);
	void						createResponseStatusCode(Connection& connection, int statusCode);
	void						createResponse0(Connection &connection, std::string uriPlusFile);
	void						createResponse200(Connection &connection, std::string uriPlusFile);

	const int&					getSocket(void) const;

	//ANCHOR yunslee
	std::vector<serverBlock>&	getServerBlocks(void);
	bool isValidMethod(Request &request, configIterator configIterator);

	// std::vector<Server>::iterator return_iterator_server(std::vector<Server> servers);
	std::vector<serverBlock>::iterator returnIteratorServerBlock(std::vector<serverBlock> &serverBlocks, std::string serverName);
	std::vector<LocationPath>::iterator returnIteratorLocationPath(std::vector<LocationPath> &locationPaths, std::string locationPathStr);


	class IOError : public std::exception
	{
		public:
			const char* what() const throw();
	};

private:
	bool						hasAuthModule(const configIterator& config_it);
	bool						isRightCredentials(const std::string& authorization);

public :
	ServerManager*				mManager;
	uint16_t					mPort; // def = 8000;
	std::string					mHost; // def = "0.0.0.0";
	std::vector<serverBlock>	mServerBlocks;

	int							mSocket; // NOTE mConnections의 첫번째 값이 모두 서버소켓의 fd임!

	// ANCHOR 참고코드
	// REVIEW 어떻게 적용될지 생각해봐야함
	std::map<int, Connection>	mConnections;
	// std::queue<Response>		mResponses;
	// Config*						m_config;
};

struct configIterator
{
	std::vector<serverBlock>::iterator serverBlock;
	std::vector<LocationPath>::iterator locationPath;
};


#endif