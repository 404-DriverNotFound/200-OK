#include "../all_header.hpp"
#include "./server.hpp"

typedef int FD;

int error_return(char const * str)
{
	cout << str << endl;
	exit(EXIT_FAILURE);
	return (-1);
}


// class server_config
// {
// 	std::string block_name[3] = {"http", "server", "location"};
// };

void setPath(std::vector<std::string> &vec, const std::string& str)
{
	size_t before = 0; //string처음부터 검사
	size_t after = 0;
	std::string substr;
	while((after = str.find("/", before)) != std::string::npos)  //from을 찾을 수 없을 때까지
	{
		substr = str.substr(before, after - before);
		cout << "   str: " << str << endl;
		cout << "substr: " << substr << endl;
		if (substr.size() != 0)
			vec.push_back(substr);
		before = after + 1;
	}

	// usr/yunslee/index.html 마지막을 넣어주기 위해
	if (before < str.size())
	{
		substr = str.substr(before, after - 1);
		vec.push_back(substr);
		before = after + 1;
	}
}

int parsingServerBlock(std::string &str, int read_size)
{
	size_t before = 0; //string처음부터 검사
	size_t after = 0;
	size_t temp = 0;
	std::string name = "server";
	std::string substr;
	while ((after = str.find(name, before)) != std::string::npos)  //from을 찾을 수 없을 때까지
	{
		if ((after = str.find("server", after + name.size())) == std::string::npos)
		{
			substr = str.substr(before, std::string::npos);	
			if (substr.size() != 0)
			{
				// parsingServerBlock
			}
			return (1);
		}
		substr = str.substr(before, after - before);	
		if (substr.size() != 0)
		{
			// parsingServerBlock
		}
		cout << "   str: " << str << endl;
		cout << "substr: " << substr << endl;
		before = after + 1;
	}
	return (-1);
}

int main()
{
	Server server;
	FD server_config;
	char buffer[BUFSIZ];
	server_config = open("./server_config_example", O_RDONLY, 0755);
	if (server_config < 0)
		return (error_return("fd open error"));
	int read_size;
	read_size = read(server_config, &buffer, BUFSIZ);
	std::string str_buffer = buffer;
	// parsingHttpBlock();
	if (-1 == parsingServerBlock(str_buffer, read_size))
		return (error_return("no server block"));
	// parsinglocationBlock();
	// server.config_locations.push_back();

	write(STDOUT_FILENO, buffer, read_size);
	close(server_config);
	return (1);
}
