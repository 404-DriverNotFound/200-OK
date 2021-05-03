#include "Webserver.hpp"
#include <sys/time.h> // FIXME 허용 매크로?

#define BUFFER_SIZE 1000

Webserver::Webserver(int argc, char** argv, char** envp)
{
	(void)argc;
	(void)argv;
	(void)envp;
}

void Webserver::start_server(void)
{
	Servers &servers = this->mservers; // NOTE yunslee
	servers.SetSockets(); // NOTE yunslee


	// Socket socket_one(INADDR_ANY, 8000); // STUB : Server socket 윤성이의 config 에 따라 ip, port 번호가 주어짐.
	int client_socket; // STUB : Accept 의 반환값을 받아 만들어지는 통신 소켓
	char buffer[BUFFER_SIZE]; // STUB : 버퍼. 버퍼사이즈는 위에 define 되어 있음.
	int bytesRead = BUFFER_SIZE - 1; // STUB : read 를 얼마나 했는 지 받는 변수. read 반복문에 들어가기 위해 초기화를 해준다.
	fd_set readfds; // STUB : 서버 소켓들을 보관할 fd_set 변수
	int ret; // STUB : select 의 반환 값을 받아. -1: error, 0: timeout, 그외 성공.
	int fd_max; // STUB : 가장 fd 번호가 늦는 서버 소켓을 저장. select 가 이 값을 참고해 서버 소켓의 변화를 확인한다.
	timeval	timeout; // STUB : timeout 시간 설정.

	/* TEST : 테스트 진행 */
	bytesRead = BUFFER_SIZE - 1;

	/* STUB 1. clear the socket set */
	FD_ZERO(&readfds);

	/* STUB 2. add master socket to set */
	for (size_t i = 0; i < servers.mservers.size(); i++) // NOTE yunslee
	{
		Server &server = servers.mservers[i];
		FD_SET(server.msocket.GetFd(), &readfds);
	}

	/* STUB 3. 여러개의 socket_fd 중에서 Get 하기 */
	fd_max = this->mservers.GetFdMax(); // NOTE yunslee

	std::cout << "fd_max: " << fd_max << std::endl;
	while (1)
	{
		timeout.tv_sec = 2; timeout.tv_usec = 0;
		fd_set cpy_readfds = readfds; // STUB : 서버 소켓들을 보관할 fd_set 변수
		/* STUB 4. select */
		if((ret = select(fd_max + 1, &cpy_readfds, NULL, NULL, &timeout)) == -1)
		{
			std::cout << "Select error\n";
			break ;
		}
		else if (ret == 0)
		{
			std::cout << "timeout\n";
			continue ;
		}
		else if (ret > 0)
		{
			std::cout << "Received connection\n";
		}

		/* STUB 4. FD_ISSET 서버 소켓 버퍼 감시 */
		for (size_t i = 0; i < servers.mservers.size(); i++) // NOTE yunslee port별로 ISSET 확인하기 위해서 반복문 씀
		{
			Server &server = servers.mservers[i];
			if (FD_ISSET(server.msocket.GetFd(), &cpy_readfds))
			{
				// std::cerr << "ret is " << ret << std::endl;
				client_socket = server.msocket.Accept();
				if (client_socket == -1)
					std::cerr << "Could not create socket." << std::endl;
				else
				{
					bytesRead = BUFFER_SIZE - 1;
					std::cout << "connected client fd: " << client_socket << std::endl;

					//	STUB : Read from the connection
					while (bytesRead == BUFFER_SIZE - 1)
					{
						ft_memset(buffer, 0, BUFFER_SIZE);	//	REVIEW : 전체 탐색하는 것들은 성능 개선의 여지가 있음
						bytesRead = read(client_socket, buffer, BUFFER_SIZE - 1); // request 를 여기서 받아서..
						/***************************************************
						 * 영환이가 버퍼를 받아 코드에서 적용시키는 영역
						 ***************************************************/
						if (bytesRead == -1)
							std::cerr << "Could not read request." << std::endl;
					}
					if (bytesRead != -1)
					{
						//	STUB : HttpMessageRequest
						HttpMessageRequest	request(buffer);
						request.Parser(); // request 를 parsing 한 후,

						//	STUB : HttpMessageResponse
						HttpMessageResponse	response(request); // reponse 를 정리한다.
						response.SetMessage();

						//	STUB : Send a message to the connection
						int len = response.GetMessage().size();
						int ret = send(client_socket, response.GetMessage().c_str(), len, 0);
					}
				}
				// STUB Close the connections
				close(client_socket);
			}
		}
	}

	 // STUB : close all server socket
	for (size_t i = 0; i < servers.mservers.size(); i++) // NOTE yunslee 서버 소켓 다 닫아야함.
	{
		Server &server = servers.mservers[i];
		close(server.msocket.GetFd());
	}
}