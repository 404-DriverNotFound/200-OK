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
#include "Server.hpp"
#include "ServerConfigIdx/ServerConfigIdx.hpp"
#include "Utils/utils.hpp"
#include "Path/Path.hpp"
#include "ConfigFile/ConfigFiles.hpp"
#include "Response.hpp"
class Server;

extern int 						gTotalClients;

class ServerManager
{
	public:
		ServerManager(void);

		void					CreateServers(const std::string& configurationFilePath);
		void					RunServers(void);
		void					ExitServer(const std::string& msg) const;

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

		const int&				GetMaxFd(void) const;
		void					SetMaxFd(const int &fd);

		const uint16_t&			GetTotalClients(void) const;
		void					SetTotalClients(const uint16_t& totalClient);

		std::vector<Server>&	GetServers(void);

	private:
		// ANCHOR yunslee
		void					updateMaxFd(void);
		int						setServersValue(ConfigFiles* configFiles);
		int						setServers(void);
		int						getIdxServer(const int& port, const std::string& host);
		int						getIdxServerBlock(const std::vector<serverBlock>& serverBlocks, const std::string& serverName);
		int						getIdxLocationPath(std::vector<LocationPath>& locationPaths, const Path& locationPath);
		int						showServers(void);
		bool					isOverlapedServer(ConfigFiles* configFiles);
		void					closeOldConnection(const std::vector<Server>::iterator& serverIterator);

		std::vector<Server>		mServers;		// NOTE: server 객체들
		int						mMaxFd;			// NOTE: 관리하는 서버의 max_fd 중 가장 큰 fd
		fd_set					mReadFds;		// NOTE: 요청 발생여부 확인을 위한 fd_set
		fd_set					mReadCopyFds;	// NOTE: select에 실제 인자로 넣을 read_set
		fd_set					mWriteFds;		// NOTE: 응답 송신 가능여부 확인을 위한 fd_set, enum 0 번.
		fd_set					mWriteCopyFds;	// NOTE: select에 실제 인자로 넣을 write_set
		uint16_t				mTotalClients;	// NOTE: 전체 클라이언트의 수
};

#endif