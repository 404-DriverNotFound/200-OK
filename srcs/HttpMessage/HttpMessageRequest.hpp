#pragma once

#include <map>
#include <queue>
#include <string>

class HttpMessageRequest
{
public:
    HttpMessageRequest(char* buf);
    ~HttpMessageRequest(void);

	void	Parser(void);

	void	Test(void);	//	NOTE : test용 코드

private:

	struct RequestLine
	{
		std::string	mMethod;
		std::string	mRequestTarget;
		std::string	mHttpVersion;
	};
	
	enum eCursor
	{
		REQUESTLINE,
		HEADERFIELD,
		MESSAGEBODY
	};

	char*								mBuffer;
	RequestLine							mRequestLine;
    std::map<std::string, std::string>	mHeaderField;
    std::deque<char*>					mMessageBody;	//	NOTE : messagebody 바꾸어야 할수도 있음
	bool								mIsvalid;		//	FIXME : 어디에서 invalid한지 알아야 적절한 statuscode 날릴 수 있음 수정 필요 Cursor랑 조합..?
	eCursor								mCursor;
};