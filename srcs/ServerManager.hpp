#ifndef SERVERMANAGER_HPP
#define SERVERMANAGER_HPP

#define INIT_FD_MAX 1023

#include <iostream>
#include <vector>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include "Define.hpp"
#include "ServerConfigIdx/ServerConfigIdx.hpp"
#include "Utils/utils.hpp"
#include "Path/Path.hpp"
#include "ConfigFile/ConfigFiles.hpp"
#include "Response.hpp"
#include "Server.hpp"
class Server;

class ServerManager
{
	public:
		ServerManager(void);

		void					CreateServers(const std::string& configurationFilePath);
		void					RunServers(void);

		const struct fd_set&	GetReadCopyFds(void) const;
		void					SetReadCopyFds(const int& fd);
		void					ClrReadCopyFds(const int& fd);

		const struct fd_set&	GetReadFds(void) const;
		void					SetReadFds(const int& fd);
		void					ClrReadFds(const int& fd);

		const struct fd_set&	GetWriteCopyFds(void) const;
		void					SetWriteCopyFds(const int& fd);
		void					ClrWriteCopyFds(const int& fd);
		
		const struct fd_set&	GetWriteFds(void) const;
		void					SetWriteFds(const int& fd);
		void					ClrWriteFds(const int& fd);
		std::vector<Server>&	GetServers(void);

	private:
		void					updateMaxFd(void);
		// ANCHOR yunslee
		int						setServersValue(ConfigFiles* configfiles);
		int						setServers(void);
		int						getIdxServer(int port);
		int						getIdxServerBlock(std::vector<ServerBlock>& serverBlocks, std::string serverName);
		int						getIdxLocationPath(std::vector<LocationPath>& locationPaths, Path locationPath);
		int						showServers(void);
		void					closeOldConnection(std::vector<Server>::iterator server_it);

		std::vector<Server>		mServers;		// NOTE: server 객체들
		int						mMaxFd;			// NOTE: 관리하는 서버의 max_fd 중 가장 큰 fd
		fd_set					mReadFds;		// NOTE: 요청 발생여부 확인을 위한 fd_set
		fd_set					mReadCopyFds;	// NOTE: select에 실제 인자로 넣을 read_set
		fd_set					mWriteFds;		// NOTE: 응답 송신 가능여부 확인을 위한 fd_set, enum 0 번.
		fd_set					mWriteCopyFds;	// NOTE: select에 실제 인자로 넣을 write_set
};

#endif