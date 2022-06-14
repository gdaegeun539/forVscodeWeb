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

	/* ������ �ּ� ���� */
	// ��Ƽ�� ��ε�ĳ��Ʈ�� ���� �� ���� ������ ����Ͱ� �����ϴ� ��쵵 �־� ����
	memset(&broad_adr, 0, sizeof(struct sockaddr_in));
	broad_adr.sin_family = AF_INET;
	broad_adr.sin_addr.s_addr = inet_addr("255.225.255.255"); // ���� ��ε�ĳ��Ʈ
	broad_adr.sin_port = htons(9000);

	/* Broadcast �����ϵ��� ���� �ɼ� ���� */
	setsockopt(hSendSock, SOL_SOCKET, SO_BROADCAST, (char*)&so_brd, sizeof(so_brd));

	/* ���� �о �ݺ��ؼ� ��� ���� */
	fp = fopen("./data.txt", "rt");
	if (fp == NULL){
		ErrorHandling("ERROR> ���� ���� �۾��� �����߽��ϴ�. ���� ���� ���θ� Ȯ�����ּ���.");
	}
	if (feof(fp)) {
		flag = 0;
		printf("ERROR> ������ ����ֽ��ϴ�. ���� ������ Ȯ�����ּ���.");
	}
	while (flag){
		fgets(buf, BUF_SIZE, fp);
		sendto(hSendSock, buf, sizeof(buf), 0, (SOCKADDR*) & broad_adr, sizeof(broad_adr));

		Sleep(2000);
		printf("%s", buf);
		if (feof(fp)){
			fseek(fp, 0, SEEK_SET);
			printf("\nServer> ������ ������ �о����ϴ�. ���� �����͸� �� ������ �����մϴ�.\n");
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
