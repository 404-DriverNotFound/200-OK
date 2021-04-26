#pragma once

#include "HttpMessageRequest.hpp"

class HttpMessageResponse
{
public:
    HttpMessageResponse(const HttpMessageRequest& request);
    ~HttpMessageResponse(void);

   	void 				SetMessage(void);
   	const std::string&	GetMessage(void) const;

private:
   	void setStatusLine(void);
	void setHeaderField(void);
	void setMessageBody(void);

	const HttpMessageRequest&			mRequest;
	std::string							mMessage;
	static std::map<int, std::string>	mReasonPhrase;

    // std::map<std::string, std::string>	mHeaderField;
};