#include <stdio.h>
#include <windows.h>
#include <process.h>
#include <time.h>
#define STR_LEN 100

unsigned WINAPI NumberOfA(void* arg);
unsigned WINAPI NumberOfOthers(void* arg);
char str[STR_LEN];
HANDLE hEvent;

int main(int argc, char *argv[]) {
	time_t start = clock();
	hEvent = CreateEventW(NULL, FALSE, FALSE, NULL); // �Ӱ豸�� ��� ����

	HANDLE hThread1 = (HANDLE)_beginthreadex(NULL, 0, NumberOfA, NULL, 0, NULL);
	HANDLE hThread2 = (HANDLE)_beginthreadex(NULL, 0, NumberOfOthers, NULL, 0, NULL);

	printf("Input String: ");
	fgets(str, STR_LEN, stdin);
	SetEvent(hEvent); // manual signalling

	WaitForSingleObject(hThread1, INFINITE);
	WaitForSingleObject(hThread2, INFINITE);
	CloseHandle(hEvent); // �Ӱ豸�� ��� ����
	time_t stop = clock();
	
	double durtation = (double)(stop - start) / CLOCKS_PER_SEC;

	//printf(">result: %lld \n", num);
	printf(">���� �ð� : %lf\n", durtation);
	getchar(); // VS ���� ����?
	return 0;
}

unsigned WINAPI NumberOfA(void* arg) { // ���� ����: init���� ����
	int cnt = 0;
	WaitForSingleObject(hEvent, INFINITE);

	for (int i = 0; str[i] != 0; i++) {
		if (str[i] == 65){
			cnt++;
		}
		
	}
	printf("number of A: %d\n", cnt);

	SetEvent(hEvent); // Auto Reset�� Release
	return 0;
}

unsigned WINAPI NumberOfOthers(void* arg) { // ���� ����: init���� ����
	int cnt = 0;
	WaitForSingleObject(hEvent, INFINITE);

	for (int i = 0; str[i] != 0; i++) {
		if (str[i] != 65 && str[i] != '\n') {
			cnt++;
		}

	}
	printf("number of Others: %d\n", cnt);

	SetEvent(hEvent); // Auto Reset�� Release
	return 0;
}
