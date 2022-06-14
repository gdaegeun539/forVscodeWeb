#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>     // for IP_MULTICAST_TTL option

#define TTL 64
#define BUF_SIZE 30
#define null NULL
void ErrorHandling(char *message);

// 멀티플렉싱 예제 코드
int main(void){
	WSADATA		wsaData;
	SOCKET		hSendSock;
	SOCKADDR_IN mulAdr;
	int			timeLive = TTL;
	FILE		*fp;
	char		buf[BUF_SIZE];
	int			flag;

	if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		ErrorHandling("WSAStartup() error!"); 
  	
	hSendSock = socket(PF_INET, SOCK_DGRAM, 0); // UDP 사용

	//...
	/* 목적지 주소 설정 */
	memset(&mulAdr, 0, sizeof(SOCKADDR_IN));
	mulAdr.sin_family = AF_INET; // IPv4
	mulAdr.sin_addr.s_addr = inet_addr("224.1.1.2"); // 멀티캐스트용 주소 중 하나
	mulAdr.sin_port = htons(9000);

	/* TTL값 설정 */
	setsockopt(hSendSock, IPPROTO_IP, IP_MULTICAST_TTL, (void*)&timeLive, sizeof(timeLive));

	/* 텍스트 파일 읽어, 반복해 보내기 */
	fp = fopen("./data.txt", "rt");
	if (fp == null){
		ErrorHandling("ERROR> 파일을 여는 중 오류가 발생했습니다.");
	}

	flag = 1;
	if (feof(fp)){
		flag = 0;
		puts("Server> 파일이 비어있습니다. 파일을 확인해주세요.");
	}
	while (flag){
		fgets(buf, BUF_SIZE, fp);
		sendto(hSendSock, buf, strlen(buf), 0, (SOCKADDR*)&mulAdr, sizeof(mulAdr));
		printf("%s", buf);
		Sleep(1000);

		if (feof(fp)){
			fseek(fp, 0, SEEK_SET);
			puts("\n파일을 다 읽었습니다. 다시 읽기 시작합니다.(File Pointer 이동)");
		}
	}

	closesocket( hSendSock );
	WSACleanup();
	return 0;
}

void ErrorHandling(char *message){
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}