#include "Server/Socket.hpp"
#include "HttpMessage/HttpMessageRequest.hpp"
#include "HttpMessage/HttpMessageResponse.hpp"

#define BUFFER_SIZE 100

int main()
{
	// Create a socket (IPv4, TCP)
	Socket socket(INADDR_ANY, 80); // Listen 작업까지 되어 있는 소켓
	size_t connections; // 통신 소켓
	char buffer[BUFFER_SIZE];
	int bytesRead = BUFFER_SIZE - 1;

	/* 테스트 진행 */
	while (1)
	{
		bytesRead = BUFFER_SIZE - 1;
		connections = socket.Accept(connections);
		//	STUB : Read from the connection
		while (bytesRead == BUFFER_SIZE - 1)
		{
			ft_memset(buffer, 0, BUFFER_SIZE);	//	REVIEW : 전체 탐색하는 것들은 성능 개선의 여지가 있음
			bytesRead = read(connections, buffer, BUFFER_SIZE - 1); // request 를 여기서 받아서..
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
			int ret = send(connections, response.GetMessage().c_str(), len, 0);
		}
		// Close the connections
		close(connections);
	}
	close(socket.GetFd());
}
