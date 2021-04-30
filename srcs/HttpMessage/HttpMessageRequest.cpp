#include <iostream>	//	FIXME	나중에 지우기 테스트용
#include <fstream>	//	FIXME	나중에 지우기 테스트용
#include <iomanip>	//	FIXME	나중에 지우기 테스트용
#include "HttpMessageRequest.hpp"

HttpMessageRequest::HttpMessageRequest(char* buf)
	: mBuffer(buf)
	, mIsvalid(false)
	, mCursor(REQUESTLINE)
{
}

HttpMessageRequest::~HttpMessageRequest(void)
{
}

void		HttpMessageRequest::Test(void)
{
	std::cout << std::endl << std::string(10, '-') << "RequestLine" << std::string(10, '-') << std::endl;
	std::cout << std::setw(20) << "method";
	std::cout << " |" << mRequestLine.mMethod << "|" << std::endl;
	std::cout << std::setw(20) << "ReqestTarget";
	std::cout << " |" << mRequestLine.mRequestTarget << "|" << std::endl;
	std::cout << std::setw(20) << "httpVersion";
	std::cout << " |" << mRequestLine.mHttpVersion << "|" << std::endl;

	std::cout << std::endl << std::string(10, '-') << "HeaderField" << std::string(10, '-') << std::endl;
	for (std::map<std::string, std::string>::iterator it = mHeaderField.begin(); it != mHeaderField.end(); ++it)
	{
		std::cout << std::setw(20) << it->first;
		std::cout << " |" << it->second << "|" << std::endl;
	}

	if (mMessageBody.empty() == false)
	{
		std::cout << std::endl << std::string(10, '-') << "MessageBody" << std::string(10, '-') << std::endl;
		int i = 0;
		for (std::deque<char*>::iterator it = mMessageBody.begin(); it != mMessageBody.end(); ++it)
		{
			i++;
			std::cout << std::setw(20) << "#" << i;
			std::cout << " |" << *it << "|" << std::endl;
		}

		std::cout << std::endl << std::string(10, '-') << "File   Test" << std::string(10, '-') << std::endl;
		std::ofstream	ofs("binarytest.png", std::ofstream::out | std::ofstream::trunc);

		ofs.write(*(mMessageBody.begin()), std::atol(mHeaderField["Content-Length"].c_str()));	//	REVIEW : atol 허용함수 인지

		ofs.close();
	}
}

static void	eraseCR(char* token)
{
	if (token == NULL)
	{
		return ;
	}
	char*	cr = strrchr(token, '\r');
	if (cr)
	{
		*cr = '\0';
	}
}

void	HttpMessageRequest::Parser(void)
{
	char* token = NULL;
	//	RequestLine PARSING
	if (mCursor == REQUESTLINE)
	{
		token = strtok(mBuffer, " ");
		mRequestLine.mMethod = token;
		token = strtok(NULL, " ");
		mRequestLine.mRequestTarget = token;
		token = strtok(NULL, "\n");
		eraseCR(token);
		mRequestLine.mHttpVersion = token;
		mCursor = HEADERFIELD;
	}
	//	HeaderField PARSING
	if (mCursor == HEADERFIELD)
	{
		while (true)
		{
			token = strtok(NULL, "\n");
			char*	colon = strchr(token, ':');
			if (colon)
			{
				*colon = '\0';
				std::string	name = token;
				colon++;
				eraseCR(colon);
				std::string	value = colon;

				//	OWS trim (앞뒤 공백 자름)
				std::size_t foundFromFront = value.find_first_not_of(" ");
				if (foundFromFront == std::string::npos)
				{
					foundFromFront = 0;
				}
				std::size_t foundFromBack = value.find_last_not_of(" ");
				//	REVIEW : substr이 leak을 유발할수도 있으니 확인 필요
				if (foundFromBack != value.length())
				{
					value = value.substr(foundFromFront, foundFromBack - foundFromFront + 1);
				}
				else
				{
					value = value.substr(foundFromFront, foundFromBack); // string is shorter as many as possible
				}
				mHeaderField[name] = value;
			}
			else
			{
				mCursor = MESSAGEBODY;
				token += 2;	//	header field 바로 다음 줄의 CR LF 넘기기 //	REVIEW : BUFFER가 딱 맞으면 index 벗어날 수도 있음
				break ;
			}
		}
	}

	//	MessageBody PARSING
	if (mCursor == MESSAGEBODY)
	{
		if (token == NULL)
		{
			mMessageBody.push_back(mBuffer); //	STUB : buffer가 작거나 chunked일 일 경우 고려해서 작성함
		}
		else
		{
			if (*token)
			{
				//	NOTE : message body 없는 경우 '\0'가 들어가면 문제 생길 수 있음
				mMessageBody.push_back(token);
				mIsvalid = true;
			}
		}
	}
	Test();	//	ANCHOR : 테스트 코드
}