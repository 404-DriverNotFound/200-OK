# JYY webserv  
[Jikang](https://profile.intra.42.fr/users/jikang)  
[Yunslee](https://profile.intra.42.fr/users/yunslee)  
[Ykoh](https://profile.intra.42.fr/users/ykoh)  

## How to run
1. `make` or `make re`
2. `./webserv` or `./webserv test.config` or `./webserv multiServer.config`  
	- argv[1] != NULL 이면, default.config로 프로그램이 실행됨.
	- 단, multiServer.config로 실행시키기위해서는 클러스터의 아이맥마다 달라지는 private IP 을 확인해야함.

## How to change setting
1. srcs/Define.hpp  
	- Change **#define** value
		- BUFFER_SIZE 1000000
		- define KEEP_ALIVE_LIMIT 10
		- define SELECT_TIMEOUT_SEC 3
		- define SELECT_TIMEOUT_USEC 0
2. `.config` 파일 수정  
	- [Rule](https://www.notion.so/nginx-4aa332c284a74f7481c7458472a42a9e)  
	- if you don't know, contact [yunslee](https://profile.intra.42.fr/users/yunslee)

---

과제진행하기전에 [**윤성우의 열혈 TCP/IP 소켓프로그래밍**](http://book.interpark.com/product/BookDisplay.do?_method=detail&sc.shopNo=0000400000&sc.prdNo=206832627&sc.saNo=003002001&bid1=search&bid2=product&bid3=title&bid4=001) 꼭 읽어보시길 권장드립니다.  

[**윤성우 책 에러**](./윤성우TCP책_에러.md)