#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <process.h>
#define MAKE_WITH_START 0
#define NUM_THREAD 50

long long num;

unsigned _stdcall ThreadInc(void*);
unsigned _stdcall ThreadDec(void*);

int main(int argc, char* argv[]) {
	int param = 5;
	UINT threadID[NUM_THREAD];
	HANDLE hThread[NUM_THREAD];

	for (int i = 0; i < NUM_THREAD; i++) {
		if (i % 2 == 0){
			hThread[i] = _beginthreadex(NULL, 0, ThreadInc, NULL, MAKE_WITH_START, &threadID[i]);
		} else{
			hThread[i] = _beginthreadex(NULL, 0, ThreadDec, NULL, MAKE_WITH_START, &threadID[i]);
		}
	}
	
	//Sleep(5000);
	WaitForMultipleObjects(NUM_THREAD, hThread, TRUE, INFINITE);
	/*DWORD waitReturn = 
	if (waitReturn == WAIT_FAILED){
		puts("ERROR> wait signal 오류 상황");
		exit(-1);
	} else if (waitReturn == WAIT_TIMEOUT) {
		puts("MAIN> wait result : timeout");
	} else{
		puts("MAIN> wait result : signalled");
	}*/

	printf("MAIN> Main 종료. / Main End. || %lld\n", num);
	return 0;
}

// Thread CallBack - CALLBACK과 WINAPI define 동일(x86)
unsigned CALLBACK ThreadInc(void* arg) {
	for (size_t i = 0; i < 5000000; i++){
		num += 1;
	}

	return 0;
}

unsigned CALLBACK ThreadDec(void* arg) {
	for (size_t i = 0; i < 5000000; i++) {
		num -= 1;
	}

	return 0;
}