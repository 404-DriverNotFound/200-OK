#include "../base_headers/Socket.hpp"
#include "HttpMessage/HttpMessageRequest.hpp"
#include "HttpMessage/HttpMessageResponse.hpp"

#define BUFFER_SIZE 65536
int main()
{
	// Create a socket (IPv4, TCP)
	Socket socket(80, INADDR_ANY); // Listen 작업까지 되어 있는 소켓
	size_t connections;

	connections = socket.Accept(connections);

	/* 테스트 진행 */
		//	STUB : Read from the connection
		char buffer[BUFFER_SIZE];	memset(buffer, 0, BUFFER_SIZE);	//	REVIEW : 전체 탐색하는 것들은 성능 개선의 여지가 있음
		int bytesRead = read(connections, buffer, BUFFER_SIZE);
		// std::cout << "The message was: " << buffer;

		//	STUB : HttpMessageRequest
		HttpMessageRequest	request(buffer);
		request.Parser();

		//	STUB : HttpMessageResponse
		HttpMessageResponse	response(request);
		response.SetMessage();

		//	STUB : Send a message to the connection
		int len = response.GetMessage().size();

		int ret = send(connections, response.GetMessage().c_str(), len, 0);

	// Close the connections
	close(connections);
	close(socket.GetFd());
}
