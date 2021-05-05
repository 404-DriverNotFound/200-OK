#include "Server.hpp"

// Server 생성자
// 이미 유효성 검사가 끝난 safety block들이 인자로 들어오기 때문에, Server 생성자에서 해야 하는 작업은 많지 않습니다.
// server_block을 파싱하여 Server 인스턴스의 멤버 변수에 값을 저장하고, location block들은 그대로 Location 생성자에 넘겨 Server 인스턴스의 벡터 멤버 변수에 저장합니다.
// 그 외에는 일반적인 서버 프로그램과 같습니다.

Server::Server(ServerManager* server_manager, const std::string& server_block, std::vector<std::string>& location_blocks, Config* config)
	: m_manager(server_manager)
	, m_config(config)
{
	if((m_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
	{
		throw std::runtime_error("SOCKET ERROR");
	}

	int	value = 1;
	if (setsockopt(m_fd, SOL_SOCKET, SO_REUSEADDR, &value, sizeof(int)) == -1)
	{
		throw std::runtime_error("SOCKET_OPTION ERROR");
	}

	struct sockaddr_in	server_addr;
	memset(&server_addr, 0, sizeof(struct sockaddr_in));
	server_addr.sin_family = AF_INET;
	// NOTE: 임시로 localhost:80으로 설정
	m_host = "localhost";
	m_port = 80;
	server_addr.sin_addr.s_addr = inet_addr(m_host.c_str());
	server_addr.sin_port = htons(m_port);
	
	if(bind(m_fd, reinterpret_cast<struct sockaddr *>(&server_addr), sizeof(struct sockaddr)) == -1)
	{
		throw std::runtime_error("BIND ERROR");
	}
	if(listen(m_fd, 256) == -1)
	{
		throw std::runtime_error("LISTEN ERROR");
	}
	if (fcntl(m_fd, F_SETFL, O_NONBLOCK) == -1)
	{
		throw std::runtime_error("FCNTL ERROR");
	}

	// location block들은 그대로 Location 생성자에 넘겨 Server 인스턴스의 벡터 멤버 변수에 저장
	for (std::vector<std::string>::iterator it = location_blocks.begin(); it != location_blocks.end(); ++it)
	{
		m_locations.push_back(Location(*it));
	}
}

const int&					Server::get_m_fd(void) const	{ return (m_fd); }