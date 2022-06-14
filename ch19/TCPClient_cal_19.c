#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

void ErrorHandling(char *message);
#define MAX_PACKET_SIZE  120

int main(int argc, char *argv[]){
	WSADATA wsaData;
	SOCKET hSocket;
	SOCKADDR_IN servAdr;
	
	if(WSAStartup(MAKEWORD(2, 2), &wsaData)!=0)
		ErrorHandling("WSAStartup() error!"); 

	hSocket=socket(PF_INET, SOCK_STREAM, 0);   
	if(hSocket==INVALID_SOCKET)
		ErrorHandling("socket() error");
	
	memset(&servAdr, 0, sizeof(servAdr));
	servAdr.sin_family=AF_INET;
	servAdr.sin_addr.s_addr=inet_addr("127.0.0.1");
	servAdr.sin_port=htons(9000);

	// TCP ���� ��û...
	int ret;
	ret = connect(hSocket, (SOCKADDR*)&servAdr, sizeof(servAdr) );
	if (ret == SOCKET_ERROR) {
		printf("<ERROR> Client. connect() ���� ����.\n");
		closesocket(hSocket);
		printf("Client> close socket...\n");
		WSACleanup();
		return 0;
	}
	else {
		printf("Client> connection established...\n");
	}
	
	// 1. send/recv �� ���� ��� �Է¹ް� ������ �����ϰ� ��� ����...
	char msg[MAX_PACKET_SIZE];
	int opndCnt, rcvSum, rcvTotal, result, flag;
	flag = 1;
	while (flag)
	{
		// 1. ����� ���� �ڷ� �Է�....(�ǿ����� ��, �ǿ�����, ������)		
		printf("Client: �ǿ����� �� �Է�: ");
		scanf("%d", &opndCnt);
		msg[0] = (char)opndCnt;
		for (int i = 0; i < opndCnt; i++) {
			printf("- %d ��° �ǿ����� �Է�: ", i+1);
			scanf("%d", &msg[1+i*sizeof(int)]);
		}
		rewind(stdin);
		printf("> �����ڸ� �Է��ϼ���: ");
		scanf("%c", &msg[1+opndCnt*sizeof(int)]);
		send(hSocket, msg, 2+opndCnt*sizeof(int), 0);

		// 2. ��� ���� �� ���		
		rcvSum = 0; // ���� ����ġ.
		rcvTotal = sizeof(result); // ���� ��ǥġ.
		while(rcvSum < rcvTotal) {
			ret = recv(hSocket, &msg[rcvSum], rcvTotal-rcvSum, 0);
			if (ret <= 0) {
				printf("<ERROR> recv ����.\n");
				flag = 0;
				break;
			}
			else {
				rcvSum = rcvSum + ret;
				printf("Client> recv %d bytes. sum=%d, total=%d\n", ret, rcvSum, rcvTotal);
			}
		}
		if (flag == 1) {
			result = *((int*)msg);
			printf("Client> �����κ��� ���ŵ� ��� = %d\n", result);
		}
	}

	closesocket(hSocket);
	printf("Client> close socket...\n");
	WSACleanup();
	return 0;
}

void ErrorHandling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}