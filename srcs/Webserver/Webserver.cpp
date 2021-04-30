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
	Socket socket_one(INADDR_ANY, 8000);
	int client_socket; // connections 를 client_socket 으로 이름 바꿈. 통신 소켓
	char buffer[BUFFER_SIZE];
	int bytesRead = BUFFER_SIZE - 1;
	fd_set readfds;
	int ret;
	int fd_max;
	timeval	timeout;
	sockaddr_in	clnt_addr;
	socklen_t addr_size;

	/* 테스트 진행 */
	bytesRead = BUFFER_SIZE - 1;

	/* 1. clear the socket set */
	FD_ZERO(&readfds);

	/* 2. add master socket to set */
	FD_SET(socket_one.GetFd(), &readfds);

	/* 3. max_sd 초기화 */
	fd_max = socket_one.GetFd();

	while (1)
	{
		timeout.tv_sec = 5;
		timeout.tv_usec = 0;

		/* 4. select */
		if((ret = select(fd_max + 1, &readfds, NULL, NULL, &timeout)) == -1)
		{
			std::cout << "Select error\n";
			break ;
		}
		if (ret == 0)
		{
			std::cout << "timeout\n";
			continue ;
		}
		if (ret > 0)
		{
			std::cout << "Received connection\n";
		}

		/* 4. FD_ISSET 서버 소켓 버퍼 감시 */
		if (FD_ISSET(socket_one.GetFd(), &readfds))
		{
			std::cerr << "ret is " << ret << std::endl;
			client_socket = socket_one.Accept(client_socket);
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
			// Close the connections
			close(client_socket);
		}
	}
	close(socket_one.GetFd());
}