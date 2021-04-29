#include "Webserver.hpp"
#define BUFFER_SIZE 100

Webserver::Webserver(int argc, char** argv, char** envp)
{
	(void)argc;
	(void)argv;
	(void)envp;
}

void Webserver::start_server(void)
{
	Socket socket_one(INADDR_ANY, 8000);
	size_t connections; // 통신 소켓
	char buffer[BUFFER_SIZE * 1000];
	int bytesRead = BUFFER_SIZE - 1;

	/* 테스트 진행 */
	while (1)
	{
		bytesRead = BUFFER_SIZE - 1;
		connections = socket_one.Accept(connections);
		//	STUB : Read from the connection
		char *temp = buffer;
		while (bytesRead == BUFFER_SIZE - 1)
		{
			// ft_memset(buffer, 0, BUFFER_SIZE);	//	REVIEW : 전체 탐색하는 것들은 성능 개선의 여지가 있음
			bytesRead = read(connections, temp, BUFFER_SIZE - 1); // request 를 여기서 받아서..
			if (bytesRead == -1)
				std::cerr << "Could not read request." << std::endl;
			temp += bytesRead;
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

			// FIXME 확인용. 아직 reponse header를 보내진 않았구나.
			std::cout << "-- - - - - - - - - - - -- -  -" << std::endl;			
			std::cout << response.GetMessage() << std::endl;
			std::cout << "-- - - - - - - - - - - -- -  -" << std::endl;			
			
			int ret = send(connections, response.GetMessage().c_str(), len, 0);
		}
		// Close the connections
		close(connections);
	}
	close(socket_one.GetFd());

}