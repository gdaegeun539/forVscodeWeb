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

	// 1. CP 오브젝트만 생성(마지막 파라메터만 중요)
	hComPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	GetSystemInfo(&sysInfo);

	// 2. CP 오브젝트를 활용한 IO 처리 쓰레드 생성
	for (int i = 0; i < sysInfo.dwNumberOfProcessors; i++){
		_beginthreadex(NULL, 0, EchoThreadMain, (LPVOID)hComPort, 0, NULL);
	}

	// 3. 서버 리스팅 소켓 생성(비동기 소켓 생성)
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
		
		// 4. 클라이언트 연결 수락
		hClntSock = accept(hServSock, (SOCKADDR*)&clntAdr, &addrLen);
		printf("Server> client(IP:%s, Port:%d) connected.\n", 
			inet_ntoa(clntAdr.sin_addr), ntohs(clntAdr.sin_port));

		// 5. CP 오브젝트에 전달할 소켓관련 구조체(LPPER_HANDLE_DATA) 설정
		handleInfo = (LPPER_HANDLE_DATA)malloc(sizeof(PER_HANDLE_DATA));
		handleInfo->hClntSock = hClntSock;
		memcpy(&(handleInfo->clntAdr), &clntAdr, addrLen);

		// 6. CP 오브젝트와 클라이언트 소켓 연결(해당 소켓에 대한 IO완료 상황을 CP 오브젝트에 등록)
		CreateIoCompletionPort((HANDLE)hClntSock, hComPort, (DWORD)handleInfo, 0);
		printf("Debug>>> %p\n", handleInfo);

		// 7. 클라이언트로부터 데이터 수신을 위해 비동기 수신함수 호출(CP 오브젝트로 전달될 자료 설정)
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
		// 1. CP 오브젝트를 통해 IO 완료 확인
		GetQueuedCompletionStatus(hComPort, &bytesTrans, (LPDWORD) &handleInfo, (LPOVERLAPPED*)&ioInfo, INFINITE);
		printf("Debug>>> %p\n", handleInfo);
		sock = handleInfo->hClntSock;

		if(ioInfo->rwMode == READ){
			// 2. 수신 IO 완료 이벤트 처리. 수신된 데이터 화면에 출력.
			ioInfo->wsaBuf.buf[bytesTrans] = '\0';
			printf("Server> %d바이트 메시지를 수신했습니다.\nServer> 내용: %s\n", bytesTrans, ioInfo->wsaBuf.buf);

			// 3. 상대 연결 종료 상황 처리
			if(bytesTrans == 0) {   // EOF 전송 시
				closesocket(sock);
				free(handleInfo); 
				free(ioInfo);
				continue;
			}

			// 4. 수신된 데이터를 송신 클라이언트에게 echo back.
			memset(&(ioInfo->overlapped), 0, sizeof(OVERLAPPED));
			ioInfo->wsaBuf.len = bytesTrans;
			ioInfo->rwMode = WRITE;

			WSASend(sock, &(ioInfo->wsaBuf), 
				1, NULL, 0, &(ioInfo->overlapped), NULL);

			// 5. 클라이언트로부터 데이터 수신을 위해 비동기 수신함수 호출(CP 오브젝트로 전달될 자료 설정)
			ioInfo = (LPPER_IO_DATA)malloc(sizeof(PER_IO_DATA));
			memset(&(ioInfo->overlapped), 0, sizeof(OVERLAPPED));
			ioInfo->wsaBuf.buf = ioInfo->buffer;
			ioInfo->wsaBuf.len = BUF_SIZE;
			ioInfo->rwMode = READ;

			WSARecv(sock, &(ioInfo->wsaBuf), 
				1, NULL, &flags, &(ioInfo->overlapped), NULL);
		}
		else{
			// 3.  송신 IO 완료 이벤트 처리
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