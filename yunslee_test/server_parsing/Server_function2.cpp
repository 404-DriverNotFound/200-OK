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

const int eDirective_num = 12;
enum eDirective
{
	ROOT, SERVER_NAME, PORT, ERROR_PAGE, INDEX,
	HEAD_LENGTH, BODY_LENGTH, AUTOINDEX, TIMEOUT, AUTH,
	LOCATION, METHOD
};

std::string server_directives[12] = {"\troot ", "\tserver_name ", "\tport ", "\terro_page ", "\tindex ",
									"\thead_length ", "\tbody_length ", "\tautoindex ", "\ttimeout ", "\tauth "
										, "\tlocation ", "\tmethod "};
std::string location_directives[12] = {"\t\troot ", "\t\tserver_name ", "\t\tport ", "\t\terro_page ", "\t\tindex ",
									"\t\thead_length ", "\t\tbody_length ", "\t\tautoindex ", "\t\ttimeout ", "\t\tauth "
										, "\t\tlocation ", "\t\tmethod "};

void outdentTab(std::string &str)
{
	if (str.size() < 2 || str[0] != '\t')
		return ;
	std::string temp;
	temp.clear();
	for (size_t i = 1; i < str.size(); i++)
	{
		temp.append(1, str[i]);
	}
	str = temp;
	return ;
}


#define START 0
#define END 1

int setRangeBracket(std::vector<std::string> &gnl, int serverBracket[2], int start)
{
	// error 처리는 잘 안함 ({}가 여러번 들어온다던지)
	int i = start;
	bool startCheck = false;
	bool endCheck = false;
	while (i < gnl.size())
	{
		if (gnl[i].size() == 1 && gnl[i].compare("{") == 0)
		{
			serverBracket[START] = i;
			startCheck = true;
		}
		else if (gnl[i].size() == 1 && gnl[i].compare("}") == 0)
		{
			serverBracket[end] = i;
			endCheck = true;
		}
		i++;
		if (startCheck == true && endCheck == true)
			return (true);
	}
	if (!(startCheck == true && endCheck == true))
		return (false);
}

struct serverBracket
{
	int start;
	int end;
};
class ServerConfig
{
	public :
		std::vector<serverBracket> serverBrack;
		static int serverNum;
		
};
int A::serverNum = 0;

int setServerBracket()
{
	
}


int parsingServerBlock(Server &servers, std::vector<std::string> &gnl)
{
	std::string oneline;
	ServerConfig confings;
	int flagServer = 0;
	int i = 0;
	int j = 0;
	int serverBracket[2];
	int locationBracket[2];
	serverBracket[START] = 0; serverBracket[END] = 0;
	locationBracket[START] = 0; locationBracket[END] = 0;
	while (i < gnl.size())
	{
		if (gnl[i][0] != '\t' && gnl[i].compare("server") == 0) // server block 단위 하나 처리
		{
			if (setRangeBracket(gnl, serverBracket, i) == false)
				return (-1);
			else
			{
				Config config; //default webserv 생성
				for (size_t i = serverBracket[START] + 1; i < serverBracket[END] - 1; i++)
				{
					outdentTab(gnl[i]); // '\t'을 없애는 전처리를 함
				}
			}
			
			for (j = serverBracket[START] + 1; j < serverBracket[END] - 1; j++)
			{
				if (gnl[j].size() == 1 && gnl[j] == '\n')
					// setConfig()
			}
			
			
			if (gnl)
			
			oneline = gnl[i];
			while (oneline[0] == '\t' && (oneline[0] != '\n' && oneline[1] != '\n')
					&& i < gnl.size())
			{
				
			}
		}
		i = serverBracket[1];
	}
	
	
	return (-1);
}

int main()
{
	Server server;
	FD server_config;
	char buffer[BUFSIZ];
	server_config = open("./server_config2", O_RDONLY, 0755);
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
	// cout << gnl.size() << endl;

	if (-1 == parsingServerBlock(server, gnl))
		return (error_return("no server block"));

	close(server_config);
	return (1);
}
