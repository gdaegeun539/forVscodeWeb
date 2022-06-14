#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>     // for IP_MULTICAST_TTL option

#define TTL 64
#define BUF_SIZE 30
void ErrorHandling(char *message);

int main(void)
{
	WSADATA wsaData;
	SOCKET hSendSock;
	struct sockaddr_in broad_adr;
	FILE* fp;
	char buf[BUF_SIZE];
	int	timeLive = TTL, so_brd = 1, flag = 1;

	if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		ErrorHandling("WSAStartup() error!"); 
  	
	hSendSock = socket(PF_INET, SOCK_DGRAM, 0);

	/* 목적지 주소 설정 */
	// 디렉티드 브로드캐스트는 보안 상 이유 때문에 라우터가 차단하는 경우도 있어 힘듦
	memset(&broad_adr, 0, sizeof(struct sockaddr_in));
	broad_adr.sin_family = AF_INET;
	broad_adr.sin_addr.s_addr = inet_addr("255.225.255.255"); // 로컬 브로드캐스트
	broad_adr.sin_port = htons(9000);

	/* Broadcast 가능하도록 소켓 옵션 설정 */
	setsockopt(hSendSock, SOL_SOCKET, SO_BROADCAST, (char*)&so_brd, sizeof(so_brd));

	/* 파일 읽어서 반복해서 기사 전송 */
	fp = fopen("./data.txt", "rt");
	if (fp == NULL){
		ErrorHandling("ERROR> 파일 열기 작업을 실패했습니다. 파일 존재 여부를 확인해주세요.");
	}
	if (feof(fp)) {
		flag = 0;
		printf("ERROR> 파일이 비어있습니다. 파일 내용을 확인해주세요.");
	}
	while (flag){
		fgets(buf, BUF_SIZE, fp);
		sendto(hSendSock, buf, sizeof(buf), 0, (SOCKADDR*) & broad_adr, sizeof(broad_adr));

		Sleep(2000);
		printf("%s", buf);
		if (feof(fp)){
			fseek(fp, 0, SEEK_SET);
			printf("\nServer> 파일을 끝까지 읽었습니다. 파일 포인터를 맨 앞으로 조정합니다.\n");
		}
	}


	closesocket(hSendSock);
	WSACleanup();
	fclose(fp);
	return 0;
}

void ErrorHandling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
