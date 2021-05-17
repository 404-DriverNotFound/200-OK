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
		get_htmlpage_Response(connection, target_uri, ANY_FILE);
		return ;
	}
}

void		Server::executeHead(Connection& connection, const Request& request, std::string target_uri)
{
	executeGet(connection, request, target_uri);
	std::string null_str;
	connection.get_m_response()->set_m_body(null_str);
}

void		Server::executePost(Connection& connection, const Request& request, std::string target_uri)
{
	connection.set_m_response(new Response(&connection, 200, request.getBody()));
	Response *response = connection.get_m_response();
	int fd = open(target_uri.c_str(), O_WRONLY | O_TRUNC | O_CREAT, 0755);
	int ret = write(fd, response->get_m_body().c_str(), response->get_m_body().length());
	cout << "ret: " << ret << endl;

}

void		Server::executePut(Connection& connection, const Request& request, std::string target_uri)
{
	connection.set_m_response(new Response(&connection, 200, request.getBody()));
	Response *response = connection.get_m_response();
	int fd = open(target_uri.c_str(), O_WRONLY | O_TRUNC | O_CREAT, 0755);
	int ret = write(fd, response->get_m_body().c_str(), response->get_m_body().length());
	cout << "ret: " << ret << endl;
}

void		Server::executeDelete(Connection& connection, const Request& request, std::string target_uri)
{
	connection.set_m_response(new Response(&connection, 200));
	Response *response = connection.get_m_response();
	int ret = unlink(target_uri.c_str());
	cout << "ret: " << ret << endl;
}

void		Server::executeOptions(Connection& connection, const Request& request)
{

}

void		Server::executeTrace(Connection& connection, const Request& request)
{

}

void		Server::executeCGI(Connection& connection, const Request& request)
{
	return ;
}
