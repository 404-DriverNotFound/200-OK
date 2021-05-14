#include "Request.hpp"

Request::Request(void)
	: mPhase(READY)
	, mSeek(0)
{
}

void				Request::addOrigin(const std::string& added_origin)
{
	m_origin.append(added_origin);
}

const std::string&		Request::get_m_origin(void) const
{
	return (m_origin);
}

void					Request::SetOrigin(const std::string& origin)
{
	m_origin = origin;
}

const Request::eMethod&	Request::get_m_method(void) const
{
	return (m_method);
}

void					Request::set_m_method(const eMethod& method)
{
	m_method = method;
}

void					Request::set_m_uri(const std::string& uri)
{
	m_uri = uri;
}

const Request::ePhase&	Request::GetPhase(void) const
{
	return (mPhase);
}

void					Request::SetPhase(const ePhase& phase)
{
	mPhase = phase;
}

const std::string&		Request::get_m_uri(void) const
{
	return (m_uri);
}

bool					Request::isValidHeader(const std::string& header)
{
	std::size_t	found = header.find(": ");
	if (found == std::string::npos)
	{
		throw 400;
	}

	std::string	key = header.substr(0, found);

	// FIXME request에 없는 헤더도 있는데, 그 부분을 수정해야할 듯 일단 다 넣음
	std::string			headerSet[18] = {"Accept-Charsets", "Accept-Language", "Allow", "Authorization", "Content-Language", "Content-Length", "Content-Location", "Content-Type", "Date", "Host", "Last-Modified", "Location", "Referer", "Retry-After", "Server", "Transfer-Encoding", "User-Agent", "WWW-Authenticate"};
	int					i;

	for (i = 0 ; i < 18; i++)
	{
		if ((key.compare(headerSet[i]) == 0))
		{
			return (true);
		}
	}
	return (false);
}

void					Request::addHeader(const std::string& header)
{
	std::size_t	found = header.find(": ");
	if (found == std::string::npos)
	{
		throw 400;
	}
	std::string	key = header.substr(0, found);
	std::string	value = header.substr(found + 2);
	std::cout << "\t\t|" << key << "| |" << value << "|" << std::endl;
	m_headers.insert(std::pair<std::string, std::string>(key, value)); // REVIEW pair 허용인지 확인해야함
}

const std::string&		Request::get_m_content(void) const
{
	return (m_content);
}

void					Request::SetContent(const std::string& content)
{
	m_content = content;
}

const std::size_t&		Request::GetSeek(void) const
{
	return (mSeek);
}

void					Request::SetSeek(const std::size_t& seek)
{
	mSeek = seek;
}

