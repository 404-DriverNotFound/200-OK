#include "../base_headers/Socket.hpp"
#include "HttpMessage/HttpMessageRequest.hpp"
#include "HttpMessage/HttpMessageResponse.hpp"

#include <fstream>	//	FIXME	나중에 지우기 테스트용
#include <sstream>	//	FIXME	나중에 지우기 테스트용
#include <string.h>	//	FIXME	나중에 지우기 테스트용

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
		// std::stringstream resp;
		// std::ifstream header("./testcase/header.http");
		// std::ifstream body("./testcase/body.html");

		// resp << header.rdbuf() << "\r\n" << body.rdbuf();

		// std::string response = resp.str();
		int len = response.GetMessage().size();

		// std::cout << "Answer is :\n";
		// std::cout << response << std::endl;

		// std::cout << "Wrote " << len  << " bytes" << '\n';
		int ret = send(connections, response.GetMessage().c_str(), len, 0);
		// std::cout << "Wrote " << len  << " bytes, sent " << ret << '\n';

	// Close the connections
	close(connections);
	close(socket.GetFd());
}
