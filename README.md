# 200_OK
Jikang https://profile.intra.42.fr/users/jikang
Yunslee https://profile.intra.42.fr/users/yunslee
Ykoh https://profile.intra.42.fr/users/ykoh
JYY webserv

과제진행하기전에 **윤성우의 열혈 TCP/IP 소켓프로그래밍** 꼭 읽어보시길 권장드립니다.
http://bimage.interpark.com/goods_image/1/2/0/8/203941208g.jpg
2012년 12월 5쇄판 에서 잘못된 부분
 책의 내용과 다르게 FD_ISSET는 꼭 양수를 반환하지 않습니다.
 fd_set[32]에서 각 요소는 int형(4byte)이고, 따라서 8(bit/byte) * 4(bytes/개) * 32개 = 1024 bits로 되어있고, 각 bit의 T/F는 fd의 상태를 말해줍니다.
 이때 FD_ISSET의 반환값은 4byte로 fd % 32 값의 bit가 1로 설정된다. unsigned int로 타입캐스팅한다면 -2147483648로 인식될 수 있다.