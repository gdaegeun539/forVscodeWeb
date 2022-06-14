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
	/* �ּ� ����ü �ʱ�ȭ �� ���� */
	memset(&adr, 0, sizeof(adr));
	adr.sin_family = AF_INET;
	adr.sin_addr.s_addr = htonl(INADDR_ANY); // �� IP�ּ� - ���ε���
	adr.sin_port = htons(9000);

	int ret = bind(hRecvSock, (SOCKADDR*)&adr, sizeof(adr));
	if (ret == SOCKET_ERROR){
		ErrorHandling("ERROR> ���� ���ε� ����");
	}
	
	/* ��Ƽĳ��Ʈ �ּҸ� IP�� ���
	: ��Ƽĳ��Ʈ �׷� ���� */
	joinAdr.imr_multiaddr.s_addr = inet_addr("224.1.1.2"); // �����Ϸ��� ��Ƽĳ��Ʈ �ּ� �ֱ�
	joinAdr.imr_interface.s_addr = htonl(INADDR_ANY); // ���Ź������� ȣ��Ʈ: �˾Ƽ� �����ض�
	ret = setsockopt(hRecvSock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (void*)&joinAdr, sizeof(joinAdr));
	if (ret == SOCKET_ERROR){
		ErrorHandling("ERROR> ���� �ɼ� �Ҵ� ����");
	}

	int flag = 1;
	while (flag) {
		// ���� ���� �ޱ� �ȴ�: from null, fromlen 0
		strLen = recvfrom(hRecvSock, buf, BUF_SIZE-1, 0, NULL, 0); // ������-1: �� ���� ����
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