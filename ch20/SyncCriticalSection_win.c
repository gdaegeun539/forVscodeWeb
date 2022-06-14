#include <stdio.h>
#include <windows.h>
#include <process.h>
#include <time.h>
#define NUM_THREAD	50

unsigned WINAPI threadInc(void * arg);
unsigned WINAPI threadDes(void * arg);
long long num=0;
CRITICAL_SECTION cs;

int main(int argc, char *argv[]) {
	HANDLE tHandles[NUM_THREAD];
	//int i;

	time_t start = clock();
	InitializeCriticalSection(&cs); // �Ӱ豸�� ��� ����
	for(int i=0; i<NUM_THREAD; i++) {
		if (i % 2) {
			tHandles[i] = (HANDLE)_beginthreadex(NULL, 0, threadInc, NULL, 0, NULL);
		}else {
			tHandles[i] = (HANDLE)_beginthreadex(NULL, 0, threadDes, NULL, 0, NULL);
		}
	}
	WaitForMultipleObjects(NUM_THREAD, tHandles, TRUE, INFINITE);
	DeleteCriticalSection(&cs); // �Ӱ豸�� ��� ����
	time_t stop = clock();
	
	double durtation = (double)(stop - start) / CLOCKS_PER_SEC;

	printf(">result: %lld \n", num);
	printf(">���� �ð� : %lf\n", durtation);
	return 0;
}

unsigned WINAPI threadInc(void * arg) {
	int i;
	// ---�Ӱ豸�� ����
	
	for (i = 0; i < 5000000; i++) {
		EnterCriticalSection(&cs);
		num += 1;
		LeaveCriticalSection(&cs);
	}
	
	// ---�Ӱ豸�� ����
	return 0;
}
unsigned WINAPI threadDes(void * arg) {
	int i;
	// ---�Ӱ豸�� ����
	
	for (i = 0; i < 5000000; i++) {
		EnterCriticalSection(&cs);
		num -= 1;
		LeaveCriticalSection(&cs);
	}
	
	// ---�Ӱ豸�� ����
	return 0;
}