#include "../base_headers/Socket.hpp"
#include <fstream>
#include <sstream>
#include <string.h> // memset forbidden


int main() {
	// Create a socket (IPv4, TCP)
	Socket socket(100, INADDR_ANY); // Listen 작업까지 되어 있는 소켓
	size_t connections;

	connections = socket.Accept(connections);

	/* 테스트 진행 */
		// Read from the connection
		char buffer[1000];	memset(buffer, 0, 1000);
		int bytesRead = read(connections, buffer, 1000);
		std::cout << "The message was: " << buffer;

		// Send a message to the connection
		std::stringstream resp;
		std::ifstream header("./testcase/header.http");
		std::ifstream body("./testcase/body.html");

		resp << header.rdbuf() << "\r\n" << body.rdbuf();

		std::string response = resp.str();
		int len = response.size();

		std::cout << "Answer is :\n";
		std::cout << response << std::endl;

		std::cout << "Wrote " << len  << " bytes" << '\n';
		int ret = send(connections, response.c_str(), len, 0);
		std::cout << "Wrote " << len  << " bytes, sent " << ret << '\n';

	// Close the connections
	close(connections);
	close(socket.GetFd());
}