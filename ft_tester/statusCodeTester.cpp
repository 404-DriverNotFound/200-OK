#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

void	test301(const struct sockaddr_in& sockAddr)
{
	{
		int	clientSocket = socket(AF_INET, SOCK_STREAM, 0);
		if (connect(clientSocket, (struct sockaddr*)&sockAddr, sizeof(sockAddr)) < 0)
		{
			throw std::exception();
		}
		std::cout << "TEST 301 STATUS CODE" << std::endl;
		std::cout << "When request uri is file but, is folder." << std::endl;
		std::cout << "--------------------------" << std::endl;
		std::string	message;
		std::string	method("GET");
		std::string	uri("/put_test");
		std::string	version("HTTP/1.1");
		message += method + " " + uri + " " + version + "\r\n";
		message += "HeaderField: hellow~\r\n";
		message += "\r\n";
		std::cout << message << std::endl;
		std::cout << "--------------------------" << std::endl;
		write(clientSocket, message.c_str(), message.length());

		char	buf[13];
		ssize_t	ret = read(clientSocket, buf, 13);
		std::string	returnStatusCode = std::string(buf).substr(9, 3);
		std::cout << "expected 301" << " returned " << returnStatusCode << std::endl;
		std::cout << "--------------------------" << std::endl;
		if (returnStatusCode.compare("301") != 0)
		{
			throw std::exception();
		}
		close(clientSocket);
	}
}

void	test413(const struct sockaddr_in& sockAddr)
{
	{
		int	clientSocket = socket(AF_INET, SOCK_STREAM, 0);
		if (connect(clientSocket, (struct sockaddr*)&sockAddr, sizeof(sockAddr)) < 0)
		{
			throw std::exception();
		}
		std::cout << "TEST 413 STATUS CODE" << std::endl;
		std::cout << "When content-length 10 but, body 11" << std::endl;
		std::cout << "--------------------------" << std::endl;
		std::string	message;
		std::string	method("PUT");
		std::string	uri("/put_test/temp2");
		std::string	version("HTTP/1.1");
		message += method + " " + uri + " " + version + "\r\n";
		message += "content-length: 10\r\n";
		message += "\r\n";
		message += "kkkkkkkkkkk";
		std::cout << message << std::endl;
		std::cout << "--------------------------" << std::endl;
		write(clientSocket, message.c_str(), message.length());

		char	buf[13];
		ssize_t	ret = read(clientSocket, buf, 13);
		std::string	returnStatusCode = std::string(buf).substr(9, 3);
		std::cout << "expected 413" << " returned " << returnStatusCode << std::endl;
		std::cout << "--------------------------" << std::endl;
		if (returnStatusCode.compare("413") != 0)
		{
			throw std::exception();
		}
		close(clientSocket);
	}
	std::cout << std::endl;
	{
		int	clientSocket = socket(AF_INET, SOCK_STREAM, 0);
		if (connect(clientSocket, (struct sockaddr*)&sockAddr, sizeof(sockAddr)) < 0)
		{
			throw std::exception();
		}
		std::cout << "TEST 413 STATUS CODE" << std::endl;
		std::cout << "When chunked," << std::endl;
		std::cout << "--------------------------" << std::endl;
		std::string	message;
		std::string	method("PUT");
		std::string	uri("/put_test/414test2");
		std::string	version("HTTP/1.1");
		message += method + " " + uri + " " + version + "\r\n";
		message += "Transfer-Encoding: chunked\r\n";
		message += "\r\n";
		message += "2\r\n";
		message += "kk\r\n";
		message += "3\r\n";
		message += "kkkk\r\n";
		message += "0\r\n";
		message += "\r\n";
		std::cout << message << std::endl;
		std::cout << "--------------------------" << std::endl;
		write(clientSocket, message.c_str(), message.length());

		char	buf[13];
		ssize_t	ret = read(clientSocket, buf, 13);
		std::string	returnStatusCode = std::string(buf).substr(9, 3);
		std::cout << "expected 413" << " returned " << returnStatusCode << std::endl;
		std::cout << "--------------------------" << std::endl;
		if (returnStatusCode.compare("413") != 0)
		{
			throw std::exception();
		}
		close(clientSocket);
	}
	std::cout << std::endl;
}

void	test411(const struct sockaddr_in& sockAddr)
{
	{
		int	clientSocket = socket(AF_INET, SOCK_STREAM, 0);
		if (connect(clientSocket, (struct sockaddr*)&sockAddr, sizeof(sockAddr)) < 0)
		{
			throw std::exception();
		}
		std::cout << "TEST 411 STATUS CODE" << std::endl;
		std::cout << "When existing content-length" << std::endl;
		std::cout << "--------------------------" << std::endl;
		std::string	message;
		std::string	method("PUT");
		std::string	uri("/put_test/temp2");
		std::string	version("HTTP/1.1");
		message += method + " " + uri + " " + version + "\r\n";
		message += "content-length: 10\r\n";
		message += "\r\n";
		message += "kkkkkkkkkk";
		std::cout << message << std::endl;
		std::cout << "--------------------------" << std::endl;
		write(clientSocket, message.c_str(), message.length());

		char	buf[13];
		ssize_t	ret = read(clientSocket, buf, 13);
		std::string	returnStatusCode = std::string(buf).substr(9, 3);
		std::cout << "expected 200, 201" << " returned " << returnStatusCode << std::endl;
		std::cout << "--------------------------" << std::endl;
		if (!(returnStatusCode.compare("200") == 0 || returnStatusCode.compare("201") == 0 || returnStatusCode.compare("204") == 0))
		{
			throw std::exception();
		}
		close(clientSocket);
	}
	std::cout << std::endl;
	{
		int	clientSocket = socket(AF_INET, SOCK_STREAM, 0);
		if (connect(clientSocket, (struct sockaddr*)&sockAddr, sizeof(sockAddr)) < 0)
		{
			throw std::exception();
		}
		std::cout << "TEST 411 STATUS CODE" << std::endl;
		std::cout << "When not existing content-length" << std::endl;
		std::cout << "--------------------------" << std::endl;
		std::string	message;
		std::string	method("PUT");
		std::string	uri("/put_test/414test2");
		std::string	version("HTTP/1.1");
		message += method + " " + uri + " " + version + "\r\n";
		message += "MyHeader: kkk\r\n";
		message += "\r\n";
		message += "kkkkkkkkkk";
		std::cout << message << std::endl;
		std::cout << "--------------------------" << std::endl;
		write(clientSocket, message.c_str(), message.length());

		char	buf[13];
		ssize_t	ret = read(clientSocket, buf, 13);
		std::string	returnStatusCode = std::string(buf).substr(9, 3);
		std::cout << "expected 411" << " returned " << returnStatusCode << std::endl;
		std::cout << "--------------------------" << std::endl;
		if (returnStatusCode.compare("411") != 0)
		{
			throw std::exception();
		}
		close(clientSocket);
	}
	std::cout << std::endl;
}

void	test505(const struct sockaddr_in& sockAddr)
{
	{
		int	clientSocket = socket(AF_INET, SOCK_STREAM, 0);
		if (connect(clientSocket, (struct sockaddr*)&sockAddr, sizeof(sockAddr)) < 0)
		{
			throw std::exception();
		}
		std::cout << "TEST 505 STATUS CODE" << std::endl;
		std::cout << "When version 1.0" << std::endl;
		std::cout << "--------------------------" << std::endl;
		std::string	message;
		std::string	method("GET");
		std::string	uri("/");
		std::string	version("HTTP/1.0");
		message += method + " " + uri + " " + version + "\r\n";
		message += "\r\n";
		message += "\r\n";
		std::cout << message << std::endl;
		std::cout << "--------------------------" << std::endl;
		write(clientSocket, message.c_str(), message.length());

		char	buf[13];
		ssize_t	ret = read(clientSocket, buf, 13);
		std::string	returnStatusCode = std::string(buf).substr(9, 3);
		std::cout << "expected 200" << " returned " << returnStatusCode << std::endl;
		std::cout << "--------------------------" << std::endl;
		if (returnStatusCode.compare("200") != 0)
		{
			throw std::exception();
		}
		close(clientSocket);
	}
	std::cout << std::endl;
	{
		int	clientSocket = socket(AF_INET, SOCK_STREAM, 0);
		if (connect(clientSocket, (struct sockaddr*)&sockAddr, sizeof(sockAddr)) < 0)
		{
			throw std::exception();
		}
		std::cout << "TEST 505 STATUS CODE" << std::endl;
		std::cout << "When version 1.2" << std::endl;
		std::cout << "--------------------------" << std::endl;
		std::string	message;
		std::string	method("GET");
		std::string	uri("/");
		std::string	version("HTTP/1.2");
		message += method + " " + uri + " " + version + "\r\n";
		message += "\r\n";
		message += "\r\n";
		std::cout << message << std::endl;
		std::cout << "--------------------------" << std::endl;
		write(clientSocket, message.c_str(), message.length());

		char	buf[13];
		ssize_t	ret = read(clientSocket, buf, 13);
		std::string	returnStatusCode = std::string(buf).substr(9, 3);
		std::cout << "expected 505" << " returned " << returnStatusCode << std::endl;
		std::cout << "--------------------------" << std::endl;
		if (returnStatusCode.compare("505") != 0)
		{
			throw std::exception();
		}
		close(clientSocket);
	}
	std::cout << std::endl;
}

void	test414(const struct sockaddr_in& sockAddr)
{
	{
		int	clientSocket = socket(AF_INET, SOCK_STREAM, 0);
		if (connect(clientSocket, (struct sockaddr*)&sockAddr, sizeof(sockAddr)) < 0)
		{
			throw std::exception();
		}
		std::cout << "TEST 414 STATUS CODE" << std::endl;
		std::cout << "When URI length 200" << std::endl;
		std::cout << "--------------------------" << std::endl;
		std::string	message;
		std::string	method("GET");
		std::string	uri(200, '/');
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
		std::cout << "expected 200" << " returned " << returnStatusCode << std::endl;
		std::cout << "--------------------------" << std::endl;
		if (returnStatusCode.compare("200") != 0)
		{
			throw std::exception();
		}
		close(clientSocket);
	}
	std::cout << std::endl;
	{
		int	clientSocket = socket(AF_INET, SOCK_STREAM, 0);
		if (connect(clientSocket, (struct sockaddr*)&sockAddr, sizeof(sockAddr)) < 0)
		{
			throw std::exception();
		}
		std::cout << "TEST 414 STATUS CODE" << std::endl;
		std::cout << "When URI length 201" << std::endl;
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
	std::cout << std::endl;
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
		std::string			host = argument.substr(0, found);
		std::string			port = argument.substr(found + 1);
		struct sockaddr_in	sockAddr;
		sockAddr.sin_family = AF_INET;
		sockAddr.sin_port = htons(std::stoi(port));
		sockAddr.sin_addr.s_addr = inet_addr(host.c_str());
		//test408();
		//test503();
		//test400();
		//test414(sockAddr);
		//test505(sockAddr);
		//test411(sockAddr);
		//test413(sockAddr);
		test301(sockAddr);
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
