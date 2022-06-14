#include <stdio.h>
#include <windows.h>
#include <process.h>
#include <time.h>

unsigned WINAPI Func01(void* arg);
unsigned WINAPI Func02(void* arg);
long long num=0;
HANDLE semOne, semTwo;

int main(int argc, char *argv[]) {

	time_t start = clock();
	//semOne = CreateSemaphore(NULL, 1, 1, NULL); // 임계구역 사용 시작
	semTwo = CreateSemaphore(NULL, 5, 5, NULL); // 임계구역 사용 시작

	HANDLE hThread1 = (HANDLE)_beginthreadex(NULL, 0, Func01, NULL, 0, NULL);
	HANDLE hThread2 = (HANDLE)_beginthreadex(NULL, 0, Func02, NULL, 0, NULL);

	WaitForSingleObject(hThread1, INFINITE);
	WaitForSingleObject(hThread2, INFINITE);
	//CloseHandle(semOne); // 임계구역 사용 종료
	CloseHandle(semTwo); // 임계구역 사용 종료
	time_t stop = clock();
	
	double durtation = (double)(stop - start) / CLOCKS_PER_SEC;

	printf(">result: %lld \n", num);
	printf(">실행 시간 : %lf\n", durtation);
	getchar(); // VS 설정 꼬임?
	return 0;
}

unsigned WINAPI Func01(void* arg) { // 실행 순서: init값과 관련

	for (int i = 0; i < 5; i++)	{
		WaitForSingleObject(semTwo, INFINITE);
		num++; // 공유변수 - 동기화 필요
		printf("%lld\n", num);
		//ReleaseSemaphore(semOne, 1, NULL);
	}

	return 0;
}

unsigned WINAPI Func02(void* arg) { // 실행 순서: init값과 관련

	for (int i = 0; i < 5; i++) {
		WaitForSingleObject(semTwo, INFINITE);
		num--; // 공유변수 - 동기화 필요
		printf("%lld\n", num);
		//ReleaseSemaphore(semTwo, 1, NULL);
	}

	return 0;
}
