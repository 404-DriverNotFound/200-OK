# include "ServerConfigIdx.hpp"

int main()
{
	Server server;
	int fd_conf;
	char buffer[BUFSIZ];
	fd_conf = open("webserv.config", O_RDWR);
	if (fd_conf < 0)
	{
		cout << ("fd open error") << endl;
		return (-1);
	}
		
	// ANCHOR gnl로 벡터에 넣어줌
	std::vector<std::string> gnl;
	char *line;
	while (get_next_line(fd_conf, &line) > 0)
	{
		gnl.push_back(line);
		free(line);
	}
	gnl.push_back(line);
	free(line);

	// STUB 벡터에 넣어진 값 출력	
	// for (size_t i = 0; i < gnl.size(); i++)
	// {
	// 	cout << gnl[i] << endl;
	// }
	// cout << gnl.size() << endl;

	if (-1 == SetServer(server, gnl))
	{
		cout << ("no server block") << endl;
		return (-1);
	}
	
	// NOTE 값이 제대로 들어왔나 출력해보기
	server.ShowServerConfigs();
	close(fd_conf);
	return (1);
}
