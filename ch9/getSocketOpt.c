#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>

// 2022. 04. 21~

int main(void) {
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	/* TCP Socket */
	SOCKET tcp_sock = socket(PF_INET, SOCK_STREAM, 0);
	/* UDP Socket */
	SOCKET udp_sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

	/* 특정 소켓 옵션 값 확인 */
	int sock_type = -1;
	int opt_len = sizeof(sock_type);
	getsockopt(tcp_sock, SOL_SOCKET, SO_TYPE, (char*)&sock_type, &opt_len);
	printf("> socket type : %d\n> SOCK_STREAM : %d\n", sock_type, SOCK_STREAM);
	
	getsockopt(udp_sock, SOL_SOCKET, SO_TYPE, (char*)&sock_type, &opt_len);
	printf("> socket type : %d\n> SOCK_DGRAM : %d\n", sock_type, SOCK_DGRAM);

	closesocket(tcp_sock);
	closesocket(udp_sock);
	return 0;
}