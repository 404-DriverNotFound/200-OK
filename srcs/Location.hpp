/*
************* Location *************
NOTE : Configuration 파일의 Location Block 안 정보들을 저장한다. Server 객체의 멤버 변수로 사용된다.
*/

#ifndef LOCATION_HPP
# define LOCATION_HPP

#include <string>
#include <map>
#include <set>	// FIXME 허용 헤더 아님
class Location
{
public:
	Location(location_block);

	const std::string&							get_m_uri(void) const;
	const std::string&							get_m_root_path(void) const;
	const std::set<std::string>&				get_m_allow_method(void) const;
	const std::string&							get_m_auth_basic_realm(void) const;
	const std::map<std::string, std::string>&	get_m_auth_basic_file(void) const;
	const std::set<std::string>&				get_m_index(void) const;
	const std::set<std::string>&				get_m_cgi(void) const;
	const bool&									get_m_autoindex(void) const;

private:
	std::string									m_uri;	// NOTE: m_uri, type 부정확 ??
	std::string									m_root_path; // NOTE: 웹서버 소프트웨어 이름
	std::set<std::string>						m_allow_method;		// FIXME: set 허용 컨테이너 아님, NOTE: 허용하는 메소드
	std::string									m_auth_basic_realm; // NOTE: 인증 영역 이름
	std::map<std::string, std::string>			m_auth_basic_file; // NOTE: 인증 파일 데이터
	std::set<std::string>						m_index;			//	FIXME set 허용 컨테이너 아님, NOTE: 인덱스 파일
	std::set<std::string>						m_cgi;				//	FIXME set 허용 컨테이너 아님, NOTE: 허용하는 cgi 확장자
	bool										m_autoindex; // NOTE: 오토 인덱스 설정 여부
};

#endif