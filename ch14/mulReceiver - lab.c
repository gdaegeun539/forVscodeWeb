#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>     // for struct ip_mreq

#define BUF_SIZE 30
#define null NULL
void ErrorHandling(char *message);

int main(void){
	WSADATA			wsaData;
	SOCKET			hRecvSock;
	SOCKADDR_IN		adr;
	struct ip_mreq	joinAdr;
	char	buf[BUF_SIZE];
	int		strLen;

	if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		ErrorHandling("WSAStartup() error!"); 
  
	hRecvSock = socket(PF_INET, SOCK_DGRAM, 0);
 	
	// ...
	/* 주소 구조체 초기화 및 설정 */
	memset(&adr, 0, sizeof(adr));
	adr.sin_family = AF_INET;
	adr.sin_addr.s_addr = htonl(INADDR_ANY); // 내 IP주소 - 바인딩용
	adr.sin_port = htons(9000);

	int ret = bind(hRecvSock, (SOCKADDR*)&adr, sizeof(adr));
	if (ret == SOCKET_ERROR){
		ErrorHandling("ERROR> 소켓 바인딩 에러");
	}
	
	/* 멀티캐스트 주소를 IP에 등록
	: 멀티캐스트 그룹 가입 */
	joinAdr.imr_multiaddr.s_addr = inet_addr("224.1.1.2"); // 수신하려는 멀티캐스트 주소 넣기
	joinAdr.imr_interface.s_addr = htonl(INADDR_ANY); // 수신받으려는 호스트: 알아서 지정해라
	ret = setsockopt(hRecvSock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (void*)&joinAdr, sizeof(joinAdr));
	if (ret == SOCKET_ERROR){
		ErrorHandling("ERROR> 소켓 옵션 할당 에러");
	}

	int flag = 1;
	while (flag) {
		// 서버 정보 받기 싫다: from null, fromlen 0
		strLen = recvfrom(hRecvSock, buf, BUF_SIZE-1, 0, NULL, 0); // 사이즈-1: 널 빼고 보냄
		if (strLen < 0) {
			flag = 0;
			break;
		}
		buf[strLen] = '\0';
		printf("%s", buf);
	}

	closesocket(hRecvSock);
	WSACleanup();
	return 0;
}

void ErrorHandling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}