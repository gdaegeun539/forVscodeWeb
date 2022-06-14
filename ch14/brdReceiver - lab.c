#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>     // for struct ip_mreq

#define BUF_SIZE 30
void ErrorHandling(char *message);

int main(void)
{
	WSADATA		wsaData;
	SOCKET		hRecvSock;
	SOCKADDR_IN adr;	
	char		buf[BUF_SIZE];
	int			strLen;

	if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		ErrorHandling("WSAStartup() error!"); 
  
	hRecvSock = socket(PF_INET, SOCK_DGRAM, 0);

	/* 소켓에 주소 설정 */
	memset(&adr, 0, sizeof(adr));
	adr.sin_family = AF_INET;
	adr.sin_addr.s_addr = htonl(INADDR_ANY);
	//adr.sin_addr.s_addr = inet_addr("127.0.0.1");
	adr.sin_port = htons(9000);

	int ret = bind(hRecvSock, (SOCKADDR*)&adr, sizeof(adr)); // 클라는 bind로 소켓과 주소 매칭 해줘야됨
	if (ret = SOCKET_ERROR) {
		ErrorHandling("ERROR> 소켓 바인딩 에러가 발생했습니다.");
	}

	/* 브로드캐스트는 추가 설정 없이 받아주면 됨 */
	while (TRUE) {
		strLen = recvfrom(hRecvSock, buf, BUF_SIZE - 1, 0, NULL, 0);
		if (strLen < 0){
			break;
		}
		buf[strLen] = 0; // 받아온 마지막 자리 널 필요

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
