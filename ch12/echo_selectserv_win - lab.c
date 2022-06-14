#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#define BUF_SIZE 1024
void ErrorHandling(char *message);
void CompressSockets(SOCKET hSockArr[], int idx, int total);
void CompressEvents(SOCKET hEventArr[], int idx, int total);

int main(int argc, char* argv[]) {
	WSADATA			wsaData;
	SOCKET			hServSock, hClntSock;
	SOCKADDR_IN		servAdr, clntAdr, clientaddr;
	int				adrSz, strLen, fdNum, addrlen, ret;
	char			buf[BUF_SIZE];

	if(WSAStartup(MAKEWORD(2, 2), &wsaData)!=0)
		ErrorHandling("ERROR> WSAStartup() error!"); 

	hServSock = socket(PF_INET, SOCK_STREAM, 0);
	memset(&servAdr, 0, sizeof(servAdr));
	servAdr.sin_family		= AF_INET;
	servAdr.sin_addr.s_addr	= htonl(INADDR_ANY);
	servAdr.sin_port		= htons(9000);
	
	if(bind(hServSock, (SOCKADDR*) &servAdr, sizeof(servAdr))==SOCKET_ERROR)
		ErrorHandling("ERROR> bind() error");
	if(listen(hServSock, 5)==SOCKET_ERROR)
		ErrorHandling("ERROR> listen() error");

	fd_set reads;
	TIMEVAL timeout;

	// 동기식용
	//FD_ZERO(&reads); // 초기화
	//FD_SET(hServSock, &reads); // 서버 소켓 등록

	// 비동기식 windows용 서버 소켓 등록
	WSAEVENT newEvent = WSACreateEvent();
	SOCKET hSockArr[WSA_MAXIMUM_WAIT_EVENTS];
	WSAEVENT hEventArr[WSA_MAXIMUM_WAIT_EVENTS];
	ret = WSAEventSelect(hServSock, newEvent, FD_ACCEPT);
	if (ret == SOCKET_ERROR){
		ErrorHandling("ERROR> WSAEventSelect 호출 오류");
	}
	
	int numOfClntSock = 0;
	hSockArr[numOfClntSock] = hServSock;
	hEventArr[numOfClntSock] = newEvent;
	numOfClntSock++;

	while(1){

		/* 비동기식용 */
		int posInfo = WSAWaitForMultipleEvents(numOfClntSock, hEventArr, FALSE, WSA_INFINITE, FALSE);

		int startIdx = posInfo - WSA_WAIT_EVENT_0;

		/* for 순회하며 확인된 이벤트 처리 loop */
		/* readEvent 
		1. connection request 수신
		2. message recv() */
		for (int i = startIdx; i < numOfClntSock; i++){
			int sigEventIdx = WSAWaitForMultipleEvents(1, &hEventArr[i], TRUE, 0, FALSE);

			if (sigEventIdx == WSA_WAIT_FAILED || sigEventIdx == WSA_WAIT_TIMEOUT){ // 오류나면 넘어가고 다른 이벤트 보자
				continue;
			}else { // 등록된 이벤트 발생한 경우
				sigEventIdx = i;
				WSANETWORKEVENTS netEvents;
				WSAEnumNetworkEvents(hSockArr[sigEventIdx], hEventArr[sigEventIdx], &netEvents);

				if (netEvents.lNetworkEvents & FD_ACCEPT){
					if (netEvents.iErrorCode[FD_ACCEPT_BIT] != 0) {
						printf("ERROR> Accept Error!\n");
						continue;
					}

					adrSz = sizeof(clntAdr);
					hClntSock = accept(hSockArr[sigEventIdx], (SOCKADDR*)&clntAdr, &adrSz);
					printf("connected client: Port:%d, IP:%s \n",
						clntAdr.sin_port, inet_ntoa(clntAdr.sin_addr));

					//FD_SET(hClntSock, &reads);
					newEvent = WSACreateEvent();
					WSAEventSelect(hClntSock, newEvent, FD_READ | FD_CLOSE); // 이벤트 매칭
					// 소켓 등록
					hEventArr[numOfClntSock] = newEvent;
					hSockArr[numOfClntSock] = hClntSock;
					numOfClntSock++;
					puts("Server> Add new client.");
				}
				else if (netEvents.lNetworkEvents & FD_READ) {
					if (netEvents.iErrorCode[FD_READ_BIT] != 0) {
						printf("ERROR> Read Error!\n");
						continue;
					}

					strLen = recv(hSockArr[sigEventIdx], buf, BUF_SIZE - 1, 0);

					// 클라이언트 정보 얻기
					addrlen = sizeof(clientaddr);
					getpeername(hSockArr[sigEventIdx], (SOCKADDR*)&clientaddr, &addrlen);

					buf[strLen] = '\0';
					printf("Server> 수신 메시지: %s (Port:%d, IP:%s) \n",
						buf, clientaddr.sin_port, inet_ntoa(clientaddr.sin_addr));
					send(hSockArr[sigEventIdx], buf, strLen, 0);    // echo!
				}
				else if (netEvents.lNetworkEvents & FD_CLOSE) {
					//strLen = recv(hSockArr[sigEventIdx], buf, BUF_SIZE - 1, 0);

					WSACloseEvent(hEventArr[sigEventIdx]);
					closesocket(hSockArr[sigEventIdx]);

					printf("closed client: %d, StrLen:%d \n", hSockArr[sigEventIdx], strLen);

					//FD_CLR(reads.fd_array[i], &reads);
					numOfClntSock--;
					CompressSockets(hSockArr, sigEventIdx, numOfClntSock);
					CompressEvents(hEventArr, sigEventIdx, numOfClntSock);
				}
			}

		}
	}
	//closesocket(hServSock);
	WSACleanup();
	return 0;
}

void ErrorHandling(char *message){
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}

void CompressSockets(SOCKET hSockArr[], int idx, int total) {
	for (int i = 0; i < total; i++){
		hSockArr[i] = hSockArr[i + 1];
	}
}

void CompressEvents(SOCKET hEventArr[], int idx, int total) {
	for (int i = 0; i < total; i++) {
		hEventArr[i] = hEventArr[i + 1];
	}
}