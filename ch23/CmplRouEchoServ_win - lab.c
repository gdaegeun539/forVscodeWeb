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

	// 1. Overlapped IO가 가능한 소켓 생성 - 기본적으로는 WSASend, WSARecv만 적용
	hLisnSock = WSASocket(PF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

	// 2. Non-blocking 모드의 IO 및 accept() 수행
	ioctlsocket(hLisnSock, FIONBIO, &mode);
	
	// 3. 서버 주소 설정
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
		// 4. CallBack 함수가 호출될 수 있는 alertable wait 상태로 천이
		SleepEx(100, TRUE); 	// for alertable wait state: CPU에서 다른 작업을 할 수 있게 나를 재워라 + 호출되면 이벤트가 뜨면 나를 콜백으로 해줘라(bool값)

		// 5. 비동기 모드로 accept() 수행.
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

		// 6. 수신 함수 호출
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

	// 1. 상대 클라이언트가 연결을 종료하는 경우 처리
	if(szRecvBytes == 0){
		closesocket(hSock);
		free(lpOverlapped -> hEvent);
		free(lpOverlapped);
		puts("Server> 클라이언트 연결을 종료합니다.");
	}
	else{ // echo!
		// 2. 데이터 수신이 완료된 경우, 수신된 데이터 출력 및 클라이언트로 echo back 수행
		bufInfo->buf[szRecvBytes] = 0;
		printf("Server> 데이터가 수신되었습니다(%d): %s\n", szRecvBytes, bufInfo->buf);

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

	printf("Server> 데이터를 echo back(%dB)했습니다.\n", szSendBytes);
	// 1. 상대 클라이언트로 데이터 송신이 완료된 경우, 새로운 데이터 수신 수행
	int flagInfo = 0;

	bufInfo->len = BUF_SIZE; // 이제 클라한테 다시 받아야지
	WSARecv(hSock, bufInfo,
		1, &recvBytes, &flagInfo, lpOverlapped, ReadCompRoutine);
}

void ErrorHandling(char *message){
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
