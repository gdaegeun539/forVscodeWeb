#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#define BUF_SIZE 1024

void CALLBACK ReadCompRoutine(DWORD, DWORD, LPWSAOVERLAPPED, DWORD);
void CALLBACK WriteCompRoutine(DWORD, DWORD, LPWSAOVERLAPPED, DWORD);
void ErrorHandling(char *message);

typedef struct{
	SOCKET		hClntSock;
	char		buf[BUF_SIZE];
	WSABUF		wsaBuf;
} PER_IO_DATA, *LPPER_IO_DATA;

int main(void){
	WSADATA			wsaData;
	SOCKET			hLisnSock, hRecvSock;	
	SOCKADDR_IN		lisnAdr, recvAdr;
	LPWSAOVERLAPPED lpOvLp;
	DWORD			recvBytes;
	LPPER_IO_DATA	hbInfo;
	int				mode = 1, recvAdrSz, flagInfo = 0;
	
	if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		ErrorHandling("Error> WSAStartup() error!"); 

	// 1. Overlapped IO�� ������ ���� ���� - �⺻�����δ� WSASend, WSARecv�� ����
	hLisnSock = WSASocket(PF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

	// 2. Non-blocking ����� IO �� accept() ����
	ioctlsocket(hLisnSock, FIONBIO, &mode);
	
	// 3. ���� �ּ� ����
	memset(&lisnAdr, 0, sizeof(lisnAdr));
	lisnAdr.sin_family		= AF_INET;
	lisnAdr.sin_addr.s_addr	= htonl(INADDR_ANY);
	lisnAdr.sin_port		= htons(9000);
	if(bind(hLisnSock, (SOCKADDR*) &lisnAdr, sizeof(lisnAdr))==SOCKET_ERROR)
		ErrorHandling("Error> bind() error");
	if(listen(hLisnSock, 5) == SOCKET_ERROR)
		ErrorHandling("Error> listen() error");

	recvAdrSz = sizeof(recvAdr);    
	while(1){
		// 4. CallBack �Լ��� ȣ��� �� �ִ� alertable wait ���·� õ��
		SleepEx(100, TRUE); 	// for alertable wait state: CPU���� �ٸ� �۾��� �� �� �ְ� ���� ����� + ȣ��Ǹ� �̺�Ʈ�� �߸� ���� �ݹ����� �����(bool��)

		// 5. �񵿱� ���� accept() ����.
		hRecvSock = accept(hLisnSock, (SOCKADDR*)&recvAdr,&recvAdrSz);
		if(hRecvSock == INVALID_SOCKET){
			if (WSAGetLastError() == WSAEWOULDBLOCK) {
				continue;
			}
			else {
				ErrorHandling("Error> accept() error");
			}
		}
		printf("Server> Client connected. IP:%s, Port: %d\n", inet_ntoa(recvAdr.sin_addr), ntohs(recvAdr.sin_port));

		// 6. ���� �Լ� ȣ��
		lpOvLp = (LPWSAOVERLAPPED)malloc(sizeof(WSAOVERLAPPED));
		memset(lpOvLp, 0, sizeof(WSAOVERLAPPED));

		hbInfo = (LPPER_IO_DATA)malloc(sizeof(PER_IO_DATA));
		hbInfo->hClntSock = (DWORD)hRecvSock;
		(hbInfo->wsaBuf).buf = hbInfo->buf;
		(hbInfo->wsaBuf).len = BUF_SIZE;
		lpOvLp->hEvent = (HANDLE)hbInfo;

		WSARecv(hRecvSock, &(hbInfo->wsaBuf), 
			1, &recvBytes, &flagInfo, lpOvLp, ReadCompRoutine);
	}

	closesocket(hRecvSock);
	closesocket(hLisnSock);
	WSACleanup();
	return 0;
}

//
void CALLBACK ReadCompRoutine(
	DWORD dwError, DWORD szRecvBytes, LPWSAOVERLAPPED lpOverlapped, DWORD flags
){
	LPPER_IO_DATA hbInfo	= (LPPER_IO_DATA)(lpOverlapped->hEvent);
	SOCKET hSock			= hbInfo->hClntSock;
	LPWSABUF bufInfo		= &(hbInfo->wsaBuf);
	DWORD sentBytes;

	// 1. ��� Ŭ���̾�Ʈ�� ������ �����ϴ� ��� ó��
	if(szRecvBytes == 0){
		closesocket(hSock);
		free(lpOverlapped -> hEvent);
		free(lpOverlapped);
		puts("Server> Ŭ���̾�Ʈ ������ �����մϴ�.");
	}
	else{ // echo!
		// 2. ������ ������ �Ϸ�� ���, ���ŵ� ������ ��� �� Ŭ���̾�Ʈ�� echo back ����
		bufInfo->buf[szRecvBytes] = 0;
		printf("Server> �����Ͱ� ���ŵǾ����ϴ�(%d): %s\n", szRecvBytes, bufInfo->buf);

		bufInfo->len = szRecvBytes;
		WSASend(hSock, bufInfo, 1, &sentBytes, 0, lpOverlapped, WriteCompRoutine);
	}
}

//
void CALLBACK WriteCompRoutine(
	DWORD dwError, DWORD szSendBytes, LPWSAOVERLAPPED lpOverlapped, DWORD flags
){
	LPPER_IO_DATA hbInfo	= (LPPER_IO_DATA)(lpOverlapped->hEvent);
	SOCKET hSock			= hbInfo->hClntSock;
	LPWSABUF bufInfo		= &(hbInfo->wsaBuf);
	DWORD recvBytes;

	printf("Server> �����͸� echo back(%dB)�߽��ϴ�.\n", szSendBytes);
	// 1. ��� Ŭ���̾�Ʈ�� ������ �۽��� �Ϸ�� ���, ���ο� ������ ���� ����
	int flagInfo = 0;

	bufInfo->len = BUF_SIZE; // ���� Ŭ������ �ٽ� �޾ƾ���
	WSARecv(hSock, bufInfo,
		1, &recvBytes, &flagInfo, lpOverlapped, ReadCompRoutine);
}

void ErrorHandling(char *message){
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
