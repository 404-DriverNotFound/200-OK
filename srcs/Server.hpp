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

/* 서버 블록과 로케이션 블록 기반으로 서버 생성 */
class Server
{
public:
	Server(ServerManager* server_manager,
			const std::string& server_block,
			std::vector<std::string>& location_blocks,
			Config* config);

	bool						runRecvAndSolve(Connection& connection);
	bool						hasException(int client_fd); // NOTE: 특정 커넥션에 대해 예외 발생여부 확인
	void						closeConnection(int client_fd); // NOTE: 특정 커넥션 제거
	void						isSendable(int client_fd); // NOTE: response 전송 가능 여부 확인
	void						sendResponse(Response response); // NOTE: response 전송
	void						hasRequest(int client_fd); // NOTE: 특정 커넥션에 대한 리퀘스트 존재여부 확인
	Request						recvRequest(Connection& connection, int client_fd); // NOTE: 특정 커넥션에 대한 리퀘스트 수신
	void						solveRequest(Request request); // NOTE: 수신한 리퀘스트 처리
	void						executeAutoindex(void); // NOTE: autoindex 실행
	void						executeGet(Request request); // NOTE: GET Method 실행
	void						executeHead(Request request); // NOTE: HEAD Method 실행
	void						executePut(Request request); // NOTE: PUT Method 실행
	void						executePost(Request request); // NOTE: POST Method 실행
	void						executeDelete(Request request); // NOTE: DELETE Method 실행
	void						executeOptions(Request request); // NOTE: OPTIONS Method 실행
	void						executeTrace(Request request); // NOTE: TRACE Method 실행
	char**						createCGIEnv(void); // NOTE: CGI 환경 변수 생성
	void						executeCGI(Request request); // NOTE: CGI 프로그램 실행
	void						createResponse(Connection& connection, int status_code); // NOTE: 응답 객체 생성, param(status_code), 응답 queue 에 push
	bool						hasNewConnection(void); // NOTE: 서버 소켓 fd에 대한 연결 요청 확인
	void						acceptNewConnection(void); // NOTE: 새로운 클라이언트 fd 생성
	void						run(void); // NOTE: 커넥션들을 순회하며 예외, 요청, 응답, 연결작업 실행



private:
	ServerManager*				m_manager; // NOTE: 자신을 관리하는 서버 매니저 참조
	std::string					m_server_name; // NOTE: 서버 이름을 저장
	std::string					m_host; // NOTE: 서버 host 를 저장
	int							m_port; // NOTE: 서버 port 를 저장
	int							m_fd; // NOTE: 서버 소켓 fd 를 저장
	int							m_request_uri_limit_size; // NOTE: 허용하는 uri 최대 크기
	int							m_request_header_limit_size; // NOTE: 허용하는 header 최대 크기
	int							m_limit_client_body_size; // NOTE: 허용하는 body 최대 크기
	std::string					m_default_error_page; // NOTE: Default 에러 페이지
	Config*						m_config; // NOTE: config 저장
	std::vector<Location>		m_locations; // NOTE: server 에 속한 location 저장
	std::map<int, Connection>	m_connections; // NOTE: server 에 속한 connection 저장
	std::queue<Response>		m_responses; // NOTE: server 에 속한 response 저장
};

#endif