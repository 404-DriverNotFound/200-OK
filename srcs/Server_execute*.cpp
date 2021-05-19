#include "Server.hpp"
#include "ServerManager.hpp" // NOTE 상호참조 문제를 해결하기 위해서!
#include "Response.hpp"

extern char**	g_env;

void		Server::executeAutoindex(Connection& connection, const Request& request, std::string uri_copy)
{
	connection.set_m_response(new Response(&connection, 200, ft::makeAutoindexHTML(uri_copy)));
	Response *response = connection.get_m_response();
	response->set_m_headers("Server", "webserv");
	response->set_m_headers("Content-Type", "text/html");
	response->set_m_headers("Connection", "keep-alive");
	response->set_m_headers("Date", ft::getCurrentTime().c_str());

	connection.SetStatus(Connection::SEND_READY);
}

void		Server::executeGet(Connection& connection, const Request& request, std::string target_uri)
{
	if (ft::access(target_uri) == false)
	{
		throw 404;
	}
	else
	{
		create_Response_200(connection, target_uri, ANY_FILE);
		return ;
	}
}

void		Server::executeHead(Connection& connection, const Request& request, std::string target_uri)
{
	executeGet(connection, request, target_uri);
	std::string null_str;
	connection.get_m_response()->set_m_body(null_str);
	Response *response = connection.get_m_response();

	std::map<std::string, std::string>::iterator it = response->get_m_headers().begin();
	std::map<std::string, std::string> temp;
	while (it != response->get_m_headers().end())
	{
		if (it->first == "Date")
			temp["Date"] = it->second;
		else if (it->first == "Server")
			temp["Server"] = it->second;
		else if (it->first == "Content-Length")
			temp["Content-Length"] = it->second;
		it++;
	}
	response->copy_m_headers(temp);
	
}

void		Server::executePost(Connection& connection, const Request& request, std::string target_uri)
{
	connection.set_m_response(new Response(&connection, 200, request.getBody()));
	Response *response = connection.get_m_response();
	int fd = open(target_uri.c_str(), O_WRONLY | O_TRUNC | O_CREAT, 0755);
	int ret = write(fd, response->get_m_body().c_str(), response->get_m_body().length());
	cout << "ret: " << ret << endl;

}

void		Server::executePut(Connection& connection, const Request& request, std::string target_uri, config_iterator config_it)
{
	// NOTE 우선은 파일만 Put에 들어온다고 가정하자.
	bool file_exist = 0;
	int open_fd = open(target_uri.c_str(), O_RDWR | O_TRUNC);
	if (open_fd > 0)
	{
		file_exist = true;
		if (errno == 0) // NOTE 파일를 open으로 열었음
		{
			connection.set_m_response(new Response(&connection, 200, request.getBody()));
			write(open_fd, connection.get_m_response()->get_m_body().c_str(), connection.get_m_response()->get_m_body().length());
		}
		else if (errno == EISDIR) // NOTE 폴더를 open으로 열었음
		{
			connection.set_m_response(new Response(&connection, 204));
			std::string temp;
			connection.get_m_response()->set_m_body(temp);
		}
		close(open_fd);
	}
	else // ENOENT 2
	{
		connection.set_m_response(new Response(&connection, 201, request.getBody()));
		errno = 0;
		int open_fd = open(target_uri.c_str(), O_WRONLY | O_CREAT, 0755);
		write(open_fd, connection.get_m_response()->get_m_body().c_str(), connection.get_m_response()->get_m_body().length());
		close(open_fd);
	}
	Response *response = connection.get_m_response();

	response->set_m_headers("Date", ft::getCurrentTime().c_str());
	response->set_m_headers("Server", "webserv");
	if (errno == 0)
	{
		char *itoa_temp = ft::itoa(response->get_m_body().length());
		response->set_m_headers("Content-Length", itoa_temp);
		free(itoa_temp);
		response->set_m_headers("Content-Type", "text/html");
		response->set_m_headers("Content-Language", "en-US");
		if (file_exist == false)
		{
			response->set_m_headers("Location", target_uri);
		}
	}
	errno = 0; // NOTE 초기화!
}

void		Server::executeDelete(Connection& connection, const Request& request, std::string target_uri)
{
	bool file_exist = 0;
	int open_fd = open(target_uri.c_str(), O_RDWR);
	int temp = errno;
	if (open_fd > 0)
	{
		file_exist = true;
		if (errno == 0) // NOTE 파일를 open으로 열었음
		{
			int status_code = 200;
			connection.set_m_response(new Response(&connection, status_code, request.getBody()));
			std::string errorpage_body = Response::makeStatusPage(status_code, connection.get_m_request()->GetMethod());
			connection.get_m_response()->set_m_body(errorpage_body);
			unlink(target_uri.c_str());
			temp = errno;
		}
		else if (errno == EISDIR) // NOTE 폴더를 open으로 열었음
		{
			int status_code = 204;
			connection.set_m_response(new Response(&connection, status_code));
			rmdir(target_uri.c_str());
			temp = errno;
		}
		close(open_fd);
		temp = errno;
	}
	else // ENOENT 2
	{
		int status_code = 204;
		connection.set_m_response(new Response(&connection, status_code));
		int open_fd = open(target_uri.c_str(), O_WRONLY | O_CREAT, 0755);
		write(open_fd, connection.get_m_response()->get_m_body().c_str(), connection.get_m_response()->get_m_body().length());
		close(open_fd);
	}
	Response *response = connection.get_m_response();

	response->set_m_headers("Date", ft::getCurrentTime().c_str());
	response->set_m_headers("Server", "webserv");
	if (errno == 0 && file_exist == true)
	{
		char *itoa_temp = ft::itoa(response->get_m_body().length());
		response->set_m_headers("Content-Length", itoa_temp);
		free(itoa_temp);
		response->set_m_headers("Content-Type", "text/html");
		response->set_m_headers("Content-Language", "en-US");
	}
	errno = 0; // NOTE 초기화!
}

void		Server::executeOptions(Connection& connection, const Request& request, std::string target_uri, config_iterator config_it)
{
	int fd = open(target_uri.c_str(), O_RDONLY);
	if (fd < 0)
		throw 404;
	else
	{
		connection.set_m_response(new Response(&connection, 200));
		Response *response = connection.get_m_response();
		response->set_m_headers("Date", ft::getCurrentTime().c_str());
		response->set_m_headers("Server", "webserv");
		std::string value;
		for (size_t i = 0; i < config_it.locationPath->m_method.size(); i++)
		{
			value += config_it.locationPath->m_method[i];
			if (i != config_it.locationPath->m_method.size() - 1)
				value += " ";
		}
		response->set_m_headers("Allow", value);
		close(fd);
	}
}

void		Server::executeTrace(Connection& connection, const Request& request)
{

}

void		Server::executeCGI(Connection& connection, const Request& request)
{
	return ;
}
