#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>

void ShowSockBufSize(SOCKET, int*, int*);

int main() {
	WSADATA wsadata;
	SOCKET tcp_sock, udp_sock;
	int sendBuf, recvBuf, state;

	WSAStartup(MAKEWORD(2, 2), &wsadata);
	tcp_sock = socket(PF_INET, SOCK_STREAM, 0);
	udp_sock = socket(PF_INET, SOCK_DGRAM, 0);
	/*
	Socket Options - send/recv buffer size 설정하기
	*/
	// 1. 현재 버퍼 확인
	ShowSockBufSize(tcp_sock, &sendBuf, &recvBuf);
	// 2. buffer 크기 조정
	sendBuf *= 3;
	recvBuf *= 3;
	state = setsockopt(tcp_sock, SOL_SOCKET, SO_SNDBUF, (char*)&sendBuf, sizeof(sendBuf));
	if (state == SOCKET_ERROR) {
		printf("<ERROR> setsockopt execute error.\n");
		exit(-1);
	}

	state = setsockopt(tcp_sock, SOL_SOCKET, SO_RCVBUF, (char*)&recvBuf, sizeof(recvBuf));
	if (state == SOCKET_ERROR) {
		printf("<ERROR> setsockopt execute error.\n");
		exit(-1);
	}

	ShowSockBufSize(tcp_sock, &sendBuf, &recvBuf);

	closesocket(tcp_sock);
	closesocket(udp_sock);
	WSACleanup();
	return 0;
}

void ShowSockBufSize(SOCKET sock, int* sendBuf, int* recvBuf) {
	int tmpSndBuf, tmpRcvBuf;

	// socket buffer size 출력
	int len = sizeof(tmpSndBuf);
	int state = getsockopt(sock, SOL_SOCKET, SO_SNDBUF, (char*)&tmpSndBuf, &len);

	if (state == SOCKET_ERROR){
		printf("<ERROR> getsockopt execute error\n");
		exit(-1);
	}else{
		printf("> send buffer size = %dB\n", tmpSndBuf);
	}

	len = sizeof(tmpRcvBuf);
	state = getsockopt(sock, SOL_SOCKET, SO_RCVBUF, (char*)&tmpRcvBuf, &len);
	if (state == SOCKET_ERROR) {
		printf("<ERROR> getsockopt execute error\n");
		exit(-1);
	}
	else {
		printf("> recv buffer size = %dB\n", tmpRcvBuf);
	}

	*sendBuf = tmpSndBuf, * recvBuf = tmpRcvBuf;

	return;
}