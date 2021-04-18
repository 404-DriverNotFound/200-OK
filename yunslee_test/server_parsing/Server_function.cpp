#include "../all_header.hpp"
#include "./server.hpp"
#include "../gnl/get_next_line_bonus.hpp"

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

enum e_config_pool
{
	ROOT, SERVER_NAME, PORT, ERROR_PAGE, INDEX,
	HEAD_LENGTH, BODY_LENGTH, AUTOINDEX, TIMEOUT, AUTH,
	LOCATION, METHOD
};

std::string config_pool[12] = {"\troot ", "\tserver_name ", "\tport ", "\terro_page ", "\tindex ",
									"\thead_length ", "\tbody_length ", "\tautoindex ", "\ttimeout ", "\tauth "
										, "\tlocation ", "\tmethod "};

int parsingServerBlock(Server &servers, std::vector<std::string> &gnl)
{
	int flag_server = 0;
	int i = 0;
	while (i < gnl.size())
	{
		Config config;
		if (gnl[i][0] != '\t' && gnl[i].compare("server"))
		{
			i++; // 다음 줄로 이동
			while (gnl[i][0] == '\t')
			{
				if (gnl[i].find("\tlocation ") != std::string::npos)
				{
					i++; // 다음 줄로 이동
					while (gnl[i][0] == '\t' && gnl[i][1] == '\t')
					{
						if (gnl[i].find(config_pool[ROOT]) != std::string::npos)
						{
						
						}
						else if (gnl[i].find(config_pool[SERVER_NAME]) != std::string::npos)
						{
							
						}
						//...
						i++;
					}
				}
				if (gnl[i].find(config_pool[ROOT]) != std::string::npos)
				{
				
				}
				else if (gnl[i].find(config_pool[SERVER_NAME]) != std::string::npos)
				{
					
				}
				//...
				i++;
			}
			servers.config_locations.push_back(config);
		}
		i++;
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
	std::vector<std::string> gnl;
	char *line;
	while (get_next_line(server_config, &line) > 0)
	{
		gnl.push_back(line);
		free(line);
	}
	gnl.push_back(line);
	free(line);
	
	for (size_t i = 0; i < gnl.size(); i++)
	{
		cout << gnl[i] << endl;
	}
	cout << gnl.size() << endl;

	if (-1 == parsingServerBlock(server, gnl))
		return (error_return("no server block"));

	close(server_config);
	return (1);
}
