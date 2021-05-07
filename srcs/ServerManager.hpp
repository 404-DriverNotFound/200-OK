#ifndef SERVERMANAGER_HPP
#define SERVERMANAGER_HPP

#define DEFAULT_CONFIG_FILE_PATH "webserv.config"

#include <iostream>
#include <vector>
#include <sys/select.h> // REVIEW: FD_SET 을 위해 추가
#include "Config.hpp" // REVIEW: 일단 추가
#include "Server.hpp" // REVIEW: 일단 추가
class Server;
#include "Location.hpp"  // REVIEW: 일단 추가



class ServerManager
{
	public:
		ServerManager(void);

		// const Config&		get_m_config(void) const;
		// const int&			get_m_max_fd(void) const;
		// void					set_m_config(const Config& config);
		void					set_m_max_fd(const int& fd);

		enum SetType		{ WRITE_SET, WRITE_COPY_SET, READ_SET, READ_COPY_SET, ERROR_SET, ERROR_COPY_SET, ALL_SET };

		// void				fdSet(int fd, SetType fdset);
		void				fdZero(SetType fdset);
		// void				fdClear(int fd, SetType fdset);
		bool				fdIsset(int fd, SetType fdset);
		void				fdCopy(SetType fdset);

		void				resetMaxFd(int new_max_fd);

		void				exitServer(const std::string& msg) const; // NOTE: 에러 메시지 출력 후 프로그램 종료
		void				createServer(const std::string& configuration_file_path, char** envp); // NOTE: 서버 객체 생성
		void				runServer(void); // NOTE: 관리하고 있는 전체 서버 실행

	private:
		// bool					splitConfigString(config_string config_string, config_block config_block, std::vector<std::string> server_strings); // NOTE: config 파일을 config block/server strings로 분리
		// bool					splitServerString(std::string server_string, std::string server_block, std::vector<std::string> location_block); // NOTE: server string을 server/location blocks로 분리
		// bool					isValidConfigBlock(std::string config_block); // NOTE: config block 유효성 확인, ServerManager::creator 에 있는 함수 타입 참고하여 타입 수정
		// bool					isValidServerBlock(std::string server_block); // NOTE: server block 유효성 확인, ServerManager::creator 에 있는 함수 타입 참고하여 타입 수정
		// bool					isValidLocationBlock(std::string location_block); // NOTE: location block 유효성 확인, ServerManager::creator 에 있는 함수 타입 참고하여 타입 수정

		std::set<int>		m_server_fdset;		// FIXME: 존재이유 파악못했고, set 컨테이너 사용 불가
		std::vector<Server>	m_servers;			// NOTE: server 객체들
		Config				m_config;			// NOTE: configuration 파일을 파싱한 결과
		int					m_max_fd;			// NOTE: 관리하는 서버의 max_fd 중 가장 큰 fd
		fd_set				m_read_set;			// NOTE: 요청 발생여부 확인을 위한 fd_set
		fd_set				m_read_copy_set;	// NOTE: select에 실제 인자로 넣을 read_set
		fd_set				m_write_set;		// NOTE: 응답 송신 가능여부 확인을 위한 fd_set, enum 0 번.
		fd_set				m_write_copy_set;	// NOTE: select에 실제 인자로 넣을 write_set
		// fd_set				m_error_set;		// NOTE: 예외 발생여부 확인을 위한 fd_set
		// fd_set				m_error_copy_set;	// NOTE: select에 실제 인자로 넣을 error_set



};

#endif