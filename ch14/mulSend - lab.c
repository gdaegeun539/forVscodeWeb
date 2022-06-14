#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>     // for IP_MULTICAST_TTL option

#define TTL 64
#define BUF_SIZE 30
#define null NULL
void ErrorHandling(char *message);

// ��Ƽ�÷��� ���� �ڵ�
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
  	
	hSendSock = socket(PF_INET, SOCK_DGRAM, 0); // UDP ���

	//...
	/* ������ �ּ� ���� */
	memset(&mulAdr, 0, sizeof(SOCKADDR_IN));
	mulAdr.sin_family = AF_INET; // IPv4
	mulAdr.sin_addr.s_addr = inet_addr("224.1.1.2"); // ��Ƽĳ��Ʈ�� �ּ� �� �ϳ�
	mulAdr.sin_port = htons(9000);

	/* TTL�� ���� */
	setsockopt(hSendSock, IPPROTO_IP, IP_MULTICAST_TTL, (void*)&timeLive, sizeof(timeLive));

	/* �ؽ�Ʈ ���� �о�, �ݺ��� ������ */
	fp = fopen("./data.txt", "rt");
	if (fp == null){
		ErrorHandling("ERROR> ������ ���� �� ������ �߻��߽��ϴ�.");
	}

	flag = 1;
	if (feof(fp)){
		flag = 0;
		puts("Server> ������ ����ֽ��ϴ�. ������ Ȯ�����ּ���.");
	}
	while (flag){
		fgets(buf, BUF_SIZE, fp);
		sendto(hSendSock, buf, strlen(buf), 0, (SOCKADDR*)&mulAdr, sizeof(mulAdr));
		printf("%s", buf);
		Sleep(1000);

		if (feof(fp)){
			fseek(fp, 0, SEEK_SET);
			puts("\n������ �� �о����ϴ�. �ٽ� �б� �����մϴ�.(File Pointer �̵�)");
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