#pragma once
#include <unistd.h>
#include <sys/wait.h>

class Cgi
{
	public:
	int		pid;
	int		fd_pipe[2];
	int&	fd_out;
	char*	path;
	char*	extention;
	char**	meta_variable;

			Cgi();
	void	init(char* path, char** meta_variable);
	void	start_cgi();
	void	destroy_pipe();
};