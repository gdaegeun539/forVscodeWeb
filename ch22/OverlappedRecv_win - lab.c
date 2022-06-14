#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#define BUF_SIZE 1024
void ErrorHandling(char *message);
void CompRoutine(DWORD dwError, DWORD szRecvBytes, LPWSAOVERLAPPED lpOverLapped, DWORD flags);
char	buf[BUF_SIZE];

int main(void){
	WSADATA			wsaData;
	SOCKET			hLisnSock, hRecvSock;
	SOCKADDR_IN		lisnAdr, recvAdr;
	int				recvAdrSz;

	WSABUF			dataBuf;
	WSAEVENT		evObj;
	WSAOVERLAPPED	overlapped;

	
	int		recvBytes=0, flags=0;		

	if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		ErrorHandling("WSAStartup() error!"); 

	hLisnSock = WSASocket(PF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	
	memset(&lisnAdr, 0, sizeof(lisnAdr));
	lisnAdr.sin_family = AF_INET;
	lisnAdr.sin_addr.s_addr = inet_addr("127.0.0.1");
	lisnAdr.sin_port = htons(9000);

	if(bind(hLisnSock, (SOCKADDR*)&lisnAdr, sizeof(lisnAdr)) == SOCKET_ERROR)
		ErrorHandling("bind() error");

	if(listen(hLisnSock, 5) == SOCKET_ERROR)
		ErrorHandling("listen() error");

	recvAdrSz = sizeof(recvAdr);    
	hRecvSock = accept(hLisnSock, (SOCKADDR*)&recvAdr, &recvAdrSz);
	printf("Receiver> Ŭ���̾�Ʈ ����Ǿ����ϴ�. IP: %s Port: %d\n", inet_ntoa(recvAdr.sin_addr), ntohs(recvAdr.sin_port) );
	
	// ---

	evObj = WSACreateEvent();
	memset(&overlapped, 0, sizeof(overlapped));
	/* �ݹ� ���� overlapped �̻�� */
	overlapped.hEvent = evObj;
	
	dataBuf.buf = buf;
	dataBuf.len = BUF_SIZE;
	
	int ret = WSARecv(hRecvSock, &dataBuf, 1, &recvBytes, &flags, &overlapped, CompRoutine);
	if (ret == SOCKET_ERROR){
		if (WSAGetLastError() == WSA_IO_PENDING){
			puts("Receiver> background data receive.");

			/* �ݹ� ���� �ݹ����� ��ü�Ǵ� �κ� */
			//WSAWaitForMultipleEvents(1, &evObj, TRUE, WSA_INFINITE, FALSE);
			//WSAGetOverlappedResult(hRecvSock, &overlapped, &recvBytes, FALSE, NULL);

			////dataBuf.buf[recvBytes] = 0;

			//printf("Receiver> - IO Pending - %d����Ʈ�� �޾ҽ��ϴ�.\nReceiver> ����: %s\n", recvBytes, dataBuf.buf);
		}
		else {
			ErrorHandling("Error> WSARecv Error");
		}
	}
	else { // �� �� �ް� �ٷ� ������ ���
		//dataBuf.buf[recvBytes] = 0;
		printf("Receiver> %d����Ʈ�� �޾ҽ��ϴ�.\nReceiver> ����: %s\n", recvBytes, dataBuf.buf);
	}
	int idx = WSAWaitForMultipleEvents(1, &evObj, TRUE, WSA_INFINITE, TRUE); // Alertable Wait ����
	if (idx == WAIT_IO_COMPLETION){
		puts("Receiver> overlapped IO�� �Ϸ�Ǿ����ϴ�.");
	}
	else {
		ErrorHandling("Error> WSARecv Error");
	}

	WSACloseEvent(evObj);

	// ---
	

	closesocket(hRecvSock);
	closesocket(hLisnSock);
	WSACleanup();
	return 0;
}

void ErrorHandling(char *message){
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}

void CALLBACK CompRoutine(
	DWORD dwError, DWORD szRecvBytes, LPWSAOVERLAPPED lpOverLapped, DWORD flags
) {
	if (dwError != 0){
		ErrorHandling("Error> CompRoutine Error");
	}
	else {
		printf("Receiver> - CompRoutine - %d����Ʈ�� �޾ҽ��ϴ�.\nReceiver> ����: %s\n", szRecvBytes, buf);
	}
}