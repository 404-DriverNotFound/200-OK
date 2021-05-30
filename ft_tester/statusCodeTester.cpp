#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

void	test414(const std::string& host, const std::string& port)
{
	int					clientSocket = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in	sockAddr;
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_port = htons(std::stoi(port));
	sockAddr.sin_addr.s_addr = inet_addr(host.c_str());

	if (connect(clientSocket, (struct sockaddr*)&sockAddr, sizeof(sockAddr)) < 0)
	{
		throw std::exception();
	}
	std::cout << "TEST 414 STATUS CODE" << std::endl;
	std::cout << "We will send message below" << std::endl;
	std::cout << "--------------------------" << std::endl;
	std::string	message;
	std::string	method("GET");
	std::string	uri(201, '/');
	std::string	version("HTTP/1.1");
	message += method + " " + uri + " " + version + "\r\n";
	message += "\r\n";
	message += "\r\n";
	std::cout << message << std::endl;
	std::cout << "--------------------------" << std::endl;
	write(clientSocket, message.c_str(), message.length());

	char	buf[13];
	ssize_t	ret = read(clientSocket, buf, 13);
	std::string	returnStatusCode = std::string(buf).substr(9, 3);
	std::cout << "expected 414" << " returned " << returnStatusCode << std::endl;
	std::cout << "--------------------------" << std::endl;
	if (returnStatusCode.compare("414") != 0)
	{
		throw std::exception();
	}
	close(clientSocket);
}

int	main(int argc, char* argv[])
{
	try
	{
		if (argc < 2)
		{
			std::cerr << "example : " << argv[0] << " 127.0.0.1:8000" << std::endl;
			throw std::exception();
		}
		std::string	argument(argv[1]);
		std::size_t	found = argument.find(":");
		if (found == std::string::npos)
		{
			throw std::exception();
		}
		std::string	host = argument.substr(0, found);
		std::string	port = argument.substr(found + 1);
		//test408();
		//test503();
		//test400();
		test414(host, port);
		//test505();
		//test411();
		//test413();
		//test301();
		//test404();
		//test410();
		//test405();
		//test401();
		//test403();
		//test500();
		//test504();
		//test200();
		//test201();
		//test205();
		//test204();
	}
	catch(const std::exception& e)
	{
		std::cerr << "Something wrong!" << '\n';
	}
}
