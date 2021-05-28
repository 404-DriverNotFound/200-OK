#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <string>
#include <sys/time.h>

#include "Define.hpp"
#include "Request.hpp"
class Request;
#include "Response.hpp"
class Response;
// #include "ServerManager.hpp"

class Connection
{
public:
	// REVIEW 영어 명칭이 잘 와닿지 않아서 영어 명칭을 수정하였는데, 이에 대해서 합의를 봐야할 것 같음.
	// enum Status { ON_WAIT, ON_RECV, TO_EXECUTE, ON_EXECUTE, TO_SEND, ON_SEND};
	// 커넥션은 리퀘스트 대기, 리퀘스트 읽기, CGI 프로그램과 입출력 시작 전, 입출력 시작 중, 응답 전송 전, 응답 전송 중이라는
	enum eStatus { REQUEST_READY, REQUEST_ING, CGI_READY, CGI_ING, SEND_READY, SEND_ING};
	Connection(int client_fd, std::string client_ip, int client_port);
	Connection(void);
	
	// std::map<int, Connection>	m_connections;
	// TODO map에서 컨낵션이 끊어졌을 때, *mRequest free 해줘야함(not NULL 일 때,)
	// TODO map에서 컨낵션이 끊어지지는 않았으나(keep-alive), 한번의 요청을 처리해줫을 때( ePhase== COMPLETE), delete 후 NULL값 할당.
	virtual ~Connection(void);

	const int&				get_m_fd(void) const;
	const struct timeval&	get_m_last_request_at(void) const;
	const std::string&		get_m_client_ip(void) const;
	const int&				get_m_client_port(void) const;
	void					set_m_last_reqeust_at(const struct timeval& time);
	
	Request*				get_m_request(void) const;
	void					set_m_request(Request* request);
	Response*				get_m_response(void) const;
	void					set_m_response(Response* response);

	const eStatus&			GetStatus(void) const;
	void					SetStatus(const eStatus& status);

	void					addRbufFromClient(char* buf, size_t count);

	//ANCHOR yunslee
	bool					isKeepConnection(void);
private:
	eStatus					mStatus;
	int						m_fd;
	struct timeval			m_last_request_at;
	std::string				m_client_ip;
	int						m_client_port;
	
	Request*				mRequest;
	Response*				mResponse;
};

#endif