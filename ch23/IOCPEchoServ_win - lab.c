#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include <winsock2.h>
#include <windows.h>
#define BUF_SIZE	100
#define READ		3
#define	WRITE		5

/* MSVC 2022에서 정상작동 안함 */

typedef struct {   // socket info

	SOCKET			hClntSock;
	SOCKADDR_IN		clntAdr;
} PER_HANDLE_DATA, *LPPER_HANDLE_DATA;

typedef struct {   // buffer info
	OVERLAPPED		overlapped;
	WSABUF			wsaBuf;
	char			buffer[BUF_SIZE];
	int				rwMode;    // READ or WRITE
} PER_IO_DATA, *LPPER_IO_DATA;

DWORD WINAPI EchoThreadMain(LPVOID CompletionPortIO);
void ErrorHandling(char *message);

int main(void){
	WSADATA				wsaData;
	HANDLE				hComPort;	
	SYSTEM_INFO			sysInfo;
	LPPER_IO_DATA		ioInfo;
	LPPER_HANDLE_DATA	handleInfo;

	SOCKET				hServSock;
	SOCKADDR_IN			servAdr;
	int					recvBytes, flags=0;

	if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		ErrorHandling("WSAStartup() error!"); 

	// 1. CP ������Ʈ�� ����(������ �Ķ���͸� �߿�)
	hComPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	GetSystemInfo(&sysInfo);

	// 2. CP ������Ʈ�� Ȱ���� IO ó�� ������ ����
	for (int i = 0; i < sysInfo.dwNumberOfProcessors; i++){
		_beginthreadex(NULL, 0, EchoThreadMain, (LPVOID)hComPort, 0, NULL);
	}

	// 3. ���� ������ ���� ����(�񵿱� ���� ����)
	hServSock = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	servAdr.sin_family		= AF_INET;
	servAdr.sin_addr.s_addr	= htonl(INADDR_ANY);
	servAdr.sin_port		= htons(9000);

	bind(hServSock, (SOCKADDR*)&servAdr, sizeof(servAdr));
	listen(hServSock, 5);
	
	while(1){	
		SOCKET		hClntSock;
		SOCKADDR_IN clntAdr;		
		int addrLen = sizeof(clntAdr);
		
		// 4. Ŭ���̾�Ʈ ���� ����
		hClntSock = accept(hServSock, (SOCKADDR*)&clntAdr, &addrLen);
		printf("Server> client(IP:%s, Port:%d) connected.\n", 
			inet_ntoa(clntAdr.sin_addr), ntohs(clntAdr.sin_port));

		// 5. CP ������Ʈ�� ������ ���ϰ��� ����ü(LPPER_HANDLE_DATA) ����
		handleInfo = (LPPER_HANDLE_DATA)malloc(sizeof(PER_HANDLE_DATA));
		handleInfo->hClntSock = hClntSock;
		memcpy(&(handleInfo->clntAdr), &clntAdr, addrLen);

		// 6. CP ������Ʈ�� Ŭ���̾�Ʈ ���� ����(�ش� ���Ͽ� ���� IO�Ϸ� ��Ȳ�� CP ������Ʈ�� ���)
		CreateIoCompletionPort((HANDLE)hClntSock, hComPort, (DWORD)handleInfo, 0);
		printf("Debug>>> %p\n", handleInfo);

		// 7. Ŭ���̾�Ʈ�κ��� ������ ������ ���� �񵿱� �����Լ� ȣ��(CP ������Ʈ�� ���޵� �ڷ� ����)
		ioInfo = (LPPER_IO_DATA)malloc(sizeof(PER_IO_DATA));
		memset(&(ioInfo->overlapped), 0, sizeof(OVERLAPPED));
		ioInfo->wsaBuf.buf = ioInfo->buffer;
		ioInfo->wsaBuf.len = BUF_SIZE;
		ioInfo->rwMode = READ;

		WSARecv(handleInfo->hClntSock,	&(ioInfo->wsaBuf),	
			1, &recvBytes, &flags, &(ioInfo->overlapped), NULL);			
	}
	return 0;
}

DWORD WINAPI EchoThreadMain(LPVOID pComPort){
	HANDLE	hComPort = (HANDLE)pComPort;
	SOCKET	sock;
	DWORD	bytesTrans;
	LPPER_HANDLE_DATA	handleInfo;
	LPPER_IO_DATA		ioInfo;
	DWORD	flags = 0;
	
	while(1){ 
		// 1. CP ������Ʈ�� ���� IO �Ϸ� Ȯ��
		GetQueuedCompletionStatus(hComPort, &bytesTrans, (LPDWORD) &handleInfo, (LPOVERLAPPED*)&ioInfo, INFINITE);
		printf("Debug>>> %p\n", handleInfo);
		sock = handleInfo->hClntSock;

		if(ioInfo->rwMode == READ){
			// 2. ���� IO �Ϸ� �̺�Ʈ ó��. ���ŵ� ������ ȭ�鿡 ���.
			ioInfo->wsaBuf.buf[bytesTrans] = '\0';
			printf("Server> %d����Ʈ �޽����� �����߽��ϴ�.\nServer> ����: %s\n", bytesTrans, ioInfo->wsaBuf.buf);

			// 3. ��� ���� ���� ��Ȳ ó��
			if(bytesTrans == 0) {   // EOF ���� ��
				closesocket(sock);
				free(handleInfo); 
				free(ioInfo);
				continue;
			}

			// 4. ���ŵ� �����͸� �۽� Ŭ���̾�Ʈ���� echo back.
			memset(&(ioInfo->overlapped), 0, sizeof(OVERLAPPED));
			ioInfo->wsaBuf.len = bytesTrans;
			ioInfo->rwMode = WRITE;

			WSASend(sock, &(ioInfo->wsaBuf), 
				1, NULL, 0, &(ioInfo->overlapped), NULL);

			// 5. Ŭ���̾�Ʈ�κ��� ������ ������ ���� �񵿱� �����Լ� ȣ��(CP ������Ʈ�� ���޵� �ڷ� ����)
			ioInfo = (LPPER_IO_DATA)malloc(sizeof(PER_IO_DATA));
			memset(&(ioInfo->overlapped), 0, sizeof(OVERLAPPED));
			ioInfo->wsaBuf.buf = ioInfo->buffer;
			ioInfo->wsaBuf.len = BUF_SIZE;
			ioInfo->rwMode = READ;

			WSARecv(sock, &(ioInfo->wsaBuf), 
				1, NULL, &flags, &(ioInfo->overlapped), NULL);
		}
		else{
			// 3. �۽� IO �Ϸ� �̺�Ʈ ó��
			printf("Server> msg sent. %d bytes\n", bytesTrans);
			//puts("message sent!");
			free(ioInfo);
		}
	}
	return 0;
}

void ErrorHandling(char *message){
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}