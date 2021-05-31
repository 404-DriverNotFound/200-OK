#ifndef SERVERMANAGER_HPP
#define SERVERMANAGER_HPP

#define INIT_FD_MAX 1023

#include "Define.hpp"
#include <iostream>
#include <vector>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h> // REVIEW: FD_SET 을 위해 추가
#include "Server.hpp" // REVIEW: 일단 추가
class Server;

#include "ServerConfigIdx/ServerConfigIdx.hpp"
#include "Utils/utils.hpp"
#include "Path/Path.hpp"
#include "ConfigFile/ConfigFiles.hpp"
#include "Response.hpp"

using namespace std;

class ServerManager
{
	public:
		ServerManager(void);

		void				CreateServers(const std::string& configurationFilePath, char** envp);
		void				RunServers(void);
		void				exitServer(const std::string& msg) const;
		// const Config&		get_m_config(void) const;
		// const int&				get_mMaxFd(void) const;
		// void					set_m_config(const Config& config);
		// void					set_mMaxFd(const int& fd);

		// enum SetType		{ WRITE_SET, WRITE_COPY_SET, READ_SET, READ_COPY_SET, ERROR_SET, ERROR_COPY_SET, ALL_SET };

		// bool				fdIsset(int fd, SetType fdset);
		// void				fdCopy(SetType fdset);

		// void				initMaxFd();
		void				updateMaxFd(void);


		// ANCHOR yunslee
		int		SetServersValue(ConfigFiles *configFiles);
		int		SetServers(void);
		
		int		GetIdxServer(int port, std::string host);
		int		GetIdxserverBlock(std::vector<serverBlock> &serverBlocks, std::string serverName);
		int		GetIdxLocationPath(std::vector<LocationPath> &locationPaths, Path locationPath);
		int		ShowServers(void);
		bool	isOverlapedServer(ConfigFiles *);
		void	closeOldConnection(std::vector<Server>::iterator serverIterator);

		// Setter Getter
		// void	SetFdMax(int maxfd);
		// int		GetFdMax();
		// fd_set &GetReadSet();
		// fd_set &GetReadSet();
		// fd_set &GetWriteCopySet();
		// fd_set &GetWriteSet();
		// fd_set &GetErrorCopySet();
		// fd_set &GetErrorSet();

		const struct fd_set&				GetReadCopyFds(void) const;
		void								SetReadCopyFds(const int& fd);
		void								ClrReadCopyFds(const int& fd);

		const struct fd_set&				GetReadFds(void) const;
		void								SetReadFds(const int& fd);
		void								ClrReadFds(const int& fd);

		const struct fd_set&				GetWriteCopyFds(void) const;
		void								SetWriteCopyFds(const int& fd);
		void								ClrWriteCopyFds(const int& fd);
		
		const struct fd_set&				GetWriteFds(void) const;
		void								SetWriteFds(const int& fd);
		void								ClrWriteFds(const int& fd);


		const int&							GetMaxFd(void) const;
		void								SetMaxFd(const int &fd);

		const uint16_t&						GetTotalClients(void) const;
		void								SetTotalClients(const uint16_t &totalClient);


		std::vector<Server> &GetServers(void);


	private:
		// bool					splitConfigString(config_string config_string, config_block config_block, std::vector<std::string> server_strings); // NOTE: config 파일을 config block/server strings로 분리
		// bool					splitServerString(std::string server_string, std::string server_block, std::vector<std::string> location_block); // NOTE: server string을 server/location blocks로 분리
		// bool					isValidConfigBlock(std::string config_block); // NOTE: config block 유효성 확인, ServerManager::creator 에 있는 함수 타입 참고하여 타입 수정
		// bool					isValidserverBlock(std::string server_block); // NOTE: server block 유효성 확인, ServerManager::creator 에 있는 함수 타입 참고하여 타입 수정
		// bool					isValidLocationBlock(std::string location_block); // NOTE: location block 유효성 확인, ServerManager::creator 에 있는 함수 타입 참고하여 타입 수정

		// std::set<int>			m_server_fdset;		// FIXME: 존재이유 파악못했고, set 컨테이너 사용 불가
		std::vector<Server>		mServers;			// NOTE: server 객체들
		// Config					m_config;			// NOTE: configuration 파일을 파싱한 결과
		int						mMaxFd;			// NOTE: 관리하는 서버의 max_fd 중 가장 큰 fd
		fd_set					mReadFds;			// NOTE: 요청 발생여부 확인을 위한 fd_set
		fd_set					mReadCopyFds;	// NOTE: select에 실제 인자로 넣을 read_set
		fd_set					mWriteFds;		// NOTE: 응답 송신 가능여부 확인을 위한 fd_set, enum 0 번.
		fd_set					mWriteCopyFds;	// NOTE: select에 실제 인자로 넣을 write_set
		uint16_t				mTotalClients;	// NOTE: 전체 클라이언트의 수
		// fd_set					m_error_set;		// NOTE: 예외 발생여부 확인을 위한 fd_set
		// fd_set					m_error_copy_set;	// NOTE: select에 실제 인자로 넣을 error_set



};

#endif