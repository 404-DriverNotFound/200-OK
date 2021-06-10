#ifndef SERVERMANAGER_HPP
#define SERVERMANAGER_HPP

#define INIT_FD_MAX 1023

#include <iostream>
#include <vector>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <poll.h>

#include "Define.hpp"
#include "Server.hpp"
#include "ServerConfigIdx/ServerConfigIdx.hpp"
#include "Utils/utils.hpp"
#include "Path/Path.hpp"
#include "ConfigFile/ConfigFiles.hpp"
#include "Response.hpp"
class Server;

extern int	gTotalClients;

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
		void					updateMaxFd(void);
		int						setServersValue(ConfigFiles* configFiles);
		int						setServers(void);
		int						getIdxServer(const int& port, const std::string& host);
		int						getIdxServerBlock(const std::vector<serverBlock>& serverBlocks, const std::string& serverName);
		int						getIdxLocationPath(std::vector<LocationPath>& locationPaths, const Path& locationPath);
		int						showServers(void);
		bool					isOverlapedServer(ConfigFiles* configFiles);
		void					closeOldConnection(const std::vector<Server>::iterator& serverIterator);
		void					serviceUnavailable(const std::vector<Server>::iterator& serverIterator);

		std::vector<Server>		mServers;
		int						mMaxFd;
		fd_set					mReadFds;
		fd_set					mReadCopyFds;
		fd_set					mWriteFds;
		fd_set					mWriteCopyFds;
		uint16_t				mTotalClients;
	public:
		struct pollfd			mPollFds[1024];
};
#endif