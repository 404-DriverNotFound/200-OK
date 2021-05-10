#include "Response.hpp"

Response::Response()
{
	this->init_status_map();
}

Response::~Response()
{
	
}

Response::Response(Connection* connection, int status_code, std::string body)
	: m_connection(connection), m_status_code(status_code), m_body(body)
{
	this->init_status_map();
	if (this->m_status_map.find(status_code) == this->m_status_map.end())
	{
		this->m_status_description = this->m_status_map[status_code];
		this->addHeader(ft::itoa(this->m_status_code), this->m_status_description);
	}
	else
	{
		std::cout << "There is no status " << status_code << " code" << std::endl;
		// FIXME try catch로 throw를 던져야할 것 같음.
	}
	this->m_body = this->makeErrorPage(m_status_code);
}

const Connection*								Response::get_m_connection(void) const{return (this->m_connection);}

const int&										Response::get_m_status_code(void) const{return (this->m_status_code);}

const std::string&								Response::get_m_status_description(void) const{return (this->m_status_description);}

const std::map<std::string, std::string>		Response::get_m_headers(void) const{return (this->m_headers);}

const enum TransferType&						Response::get_m_transfer_type(void) const{	return (this->m_transfer_type);}

const std::string&								Response::get_m_body(void) const{	return (this->m_body);}

// const std::string&								Response::get_m_firstline(void) const { return (this->m_firstline);}

void											Response::addHeader(std::string header_key, std::string header_value)
{
	this->m_headers[header_key] = header_value;
}

const std::string								Response::getResponse(void)
{
	std::string all;
	
	std::string firstline;
	firstline += "HTTP/1.1 "; firstline += ft::itoa(m_status_code); firstline += " "; firstline += this->m_status_map[m_status_code]; firstline += "\r\n";
	all += "\r\n";
	std::map<std::string, std::string>::iterator it = m_headers.begin();
	while(it != m_headers.end())
	{
		all += it->first;
		all += ": ";
		all += it->second;
		all += "\r\n";
	}
	all += "\r\n";
	all += this->m_body;
	return (all);
}

std::string										Response::makeErrorPage(int status_code)
{
	std::string errorpage;
	errorpage = "<html><head><title>STATUS_CODE STATUS_DESCRIPTION</title></head><body bgcolor=\"white\"><center> \
					<h1>STATUS_CODE STATUS_DESCRIPTION</h1></center><hr><center>YKK_Webserver</center></body></html>";
	
	std::string status_str = ft::itoa(status_code);
	ft::ReplaceAll(errorpage, "STATUS_CODE", status_str);
	ft::ReplaceAll(errorpage, "STATUS_DESCRIPTION", this->m_status_description);
	return (errorpage);
}

