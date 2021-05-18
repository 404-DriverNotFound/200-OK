#include "Response.hpp"

Response::Response()
{
	// init_status_map(); 초기화는 메인 문에서 한번 해주고 있음. 안해줘도 됨
}

Response::~Response()
{
	
}

Response::Response(Connection* connection, int status_code, std::string body)
	: m_connection(connection), m_status_code(status_code), m_body(body)
{
	// init_status_map(); 초기화는 메인 문에서 한번 해주고 있음. 안해줘도 됨
	this->set_m_connection(connection);
	this->set_m_status_code(status_code);
	if (this->m_status_map.find(status_code) != this->m_status_map.end())
		this->set_m_status_description(this->m_status_map[status_code]);
	else
	{
		std::cout << "There is no status " << status_code << " code" << std::endl; // FIXME try catch로 throw를 던져야할 것 같음.
	}
	this->make_m_firstline();	
	this->set_m_transfer_type(GENERAL); // REVIEW chunked 일때, 값을 넣어줘야함.
	this->set_m_body(body);
}

const Connection*								Response::get_m_connection(void) const{return (this->m_connection);}

const int&										Response::get_m_status_code(void) const{return (this->m_status_code);}

const std::string&								Response::get_m_status_description(void) const{return (this->m_status_description);}

std::map<std::string, std::string>&				Response::get_m_headers(void) {return (this->m_headers);}

const enum TransferType&						Response::get_m_transfer_type(void) const{	return (this->m_transfer_type);}

const std::string&								Response::get_m_body(void) const{	return (this->m_body);}

const std::string&								Response::get_m_firstline(void) const { return (this->m_firstline);}

const std::string								Response::getResponse(void)
{
	std::string all;
	
	all += this->m_firstline;
	all += "\r\n";
	std::map<std::string, std::string>::iterator it = m_headers.begin();
	while (it != m_headers.end())
	{
		all += it->first;
		all += ": ";
		all += it->second;
		all += "\r\n";
		it++;
	}
	all += "\r\n";
	all += this->m_body;
	std::cout << all << std::endl;
	return (all);
}

std::string										Response::makeErrorPage(int status_code, std::string method)
{
	std::string errorpage;
	errorpage += "<html><head><title>STATUS_CODE STATUS_DESCRIPTION</title></head><body bgcolor=\"white\"><center>\n";
	errorpage += "<h1>STATUS_CODE STATUS_DESCRIPTION</h1></center><hr>\n";
	errorpage += "<h2>Method: METHOD</h2></center><hr>\n";
	errorpage += "<center>YKK_Webserver</center></body></html>";
	
	std::string status_str = std::to_string(status_code);
	ft::ReplaceAll(errorpage, "STATUS_CODE", status_str);
	ft::ReplaceAll(errorpage, "STATUS_DESCRIPTION", Response::m_status_map[status_code]);
	ft::ReplaceAll(errorpage, "METHOD", method);
	return (errorpage);
}



void		Response::set_m_connection(Connection *connect)
{
	this->m_connection = connect;
}

void		Response::set_m_status_code(int status_code)
{
	this->m_status_code = status_code;
}

void		Response::set_m_status_description(std::string &status_description)
{
	this->m_status_description = status_description;
}

void		Response::make_m_firstline()
{
	this->m_firstline += "HTTP/1.1 ";
	this->m_firstline += std::to_string(m_status_code);
	this->m_firstline += " ";
	this->m_firstline += this->m_status_description;
}

void		Response::set_m_headers(std::string header_key, std::string header_value)
{
	this->m_headers[header_key] = header_value;
}

void		Response::set_m_transfer_type(enum TransferType type)
{
	this->m_transfer_type = type;
}

void		Response::set_m_body(std::string &body)
{
	this->m_body = body;
}

void		Response::clear_m_headers()
{
	this->m_headers.clear();
}

void		Response::copy_m_headers(std::map<std::string, std::string> &ref)
{
	this->clear_m_headers();
	std::map<std::string, std::string>::iterator it = ref.begin();
	while (it != ref.end())
	{
		this->m_headers[it->first] = it->second;
		it++;
	}
}