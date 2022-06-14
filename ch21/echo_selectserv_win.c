#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#define BUF_SIZE 1024
void ErrorHandling(char *message);

int main(void)
{
	WSADATA			wsaData;
	SOCKET			hServSock, hClntSock;
	SOCKADDR_IN		servAdr, clntAdr, clientaddr;
	int				adrSz, strLen, fdNum, i, flag, addrlen;
	char			buf[BUF_SIZE];

	if(WSAStartup(MAKEWORD(2, 2), &wsaData)!=0)
		ErrorHandling("WSAStartup() error!"); 

	hServSock = socket(PF_INET, SOCK_STREAM, 0);
	memset(&servAdr, 0, sizeof(servAdr));
	servAdr.sin_family		= AF_INET;
	servAdr.sin_addr.s_addr	= inet_addr("127.0.0.1");
	servAdr.sin_port		= htons(9000);
	
	if(bind(hServSock, (SOCKADDR*) &servAdr, sizeof(servAdr))==SOCKET_ERROR)
		ErrorHandling("bind() error");
	if(listen(hServSock, 5)==SOCKET_ERROR)
		ErrorHandling("listen() error");

	fd_set cpyReads, reads;
	TIMEVAL timeout;

	FD_ZERO(&reads);
	FD_SET(hServSock,&reads);

	while(1)
	{
		cpyReads = reads;
		timeout.tv_sec = 5; // 5sec
		timeout.tv_usec = 5000; // 5msec
		// 1. select( , cpyReads, ) // ������ ���� ��� event Ȯ��
		fdNum = select(0, &cpyReads, 0, 0, &timeout);
		if (fdNum == SOCKET_ERROR) {
			printf("<ERROR> select socket error.\n");
		}
		else if (fdNum == 0) {
			continue;
		}

		// 2. ���� ���� ��� Ȯ�ε� event ó��
		// for(��ϵ� ��� ���Ͽ� ���ؼ� loop)
		//  --- event Ȯ��... ��� ���� ��� FD_ISSET(i, cpyReads)
		//     -- read Event ���, ó��(data recv, cnt req ó��)
		for (i = 0; i < reads.fd_count; i++) {

			if (FD_ISSET(reads.fd_array[i], &cpyReads)) {
				// �ش� ���Ͽ� read event �߻�....
				if (reads.fd_array[i] == hServSock) {
					// ���� ������ con. req. �� ����...-> accept ����.
					adrSz = sizeof(clntAdr);
					hClntSock = accept(hServSock, (SOCKADDR*)&clntAdr, &adrSz);
					printf("connected client: Port:%d, IP:%s \n",
						clntAdr.sin_port, inet_ntoa(clntAdr.sin_addr));

					FD_SET(hClntSock, &reads);
				}
				else {
					strLen = recv(reads.fd_array[i], buf, BUF_SIZE - 1, 0);
					if (strLen <= 0)    // close request!
					{
						closesocket(reads.fd_array[i]);
						printf("closed client: %d, StrLen:%d \n", hClntSock, strLen);
						
						FD_CLR(reads.fd_array[i], &reads);
					}
					else
					{
						// Ŭ���̾�Ʈ ���� ���						
						addrlen = sizeof(clientaddr);
						getpeername(reads.fd_array[i], (SOCKADDR*)&clientaddr, &addrlen);

						buf[strLen] = '\0';
						printf("(Port:%d, IP:%s),Msg : %s \n",
							clientaddr.sin_port, inet_ntoa(clientaddr.sin_addr), buf);
						send(reads.fd_array[i], buf, strLen, 0);    // echo!
					}
				}
			}
		}
	}
	closesocket(hServSock);
	WSACleanup();
	return 0;
}

void ErrorHandling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}