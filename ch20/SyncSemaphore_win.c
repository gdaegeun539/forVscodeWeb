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
	//semOne = CreateSemaphore(NULL, 1, 1, NULL); // �Ӱ豸�� ��� ����
	semTwo = CreateSemaphore(NULL, 5, 5, NULL); // �Ӱ豸�� ��� ����

	HANDLE hThread1 = (HANDLE)_beginthreadex(NULL, 0, Func01, NULL, 0, NULL);
	HANDLE hThread2 = (HANDLE)_beginthreadex(NULL, 0, Func02, NULL, 0, NULL);

	WaitForSingleObject(hThread1, INFINITE);
	WaitForSingleObject(hThread2, INFINITE);
	//CloseHandle(semOne); // �Ӱ豸�� ��� ����
	CloseHandle(semTwo); // �Ӱ豸�� ��� ����
	time_t stop = clock();
	
	double durtation = (double)(stop - start) / CLOCKS_PER_SEC;

	printf(">result: %lld \n", num);
	printf(">���� �ð� : %lf\n", durtation);
	getchar(); // VS ���� ����?
	return 0;
}

unsigned WINAPI Func01(void* arg) { // ���� ����: init���� ����

	for (int i = 0; i < 5; i++)	{
		WaitForSingleObject(semTwo, INFINITE);
		num++; // �������� - ����ȭ �ʿ�
		printf("%lld\n", num);
		//ReleaseSemaphore(semOne, 1, NULL);
	}

	return 0;
}

unsigned WINAPI Func02(void* arg) { // ���� ����: init���� ����

	for (int i = 0; i < 5; i++) {
		WaitForSingleObject(semTwo, INFINITE);
		num--; // �������� - ����ȭ �ʿ�
		printf("%lld\n", num);
		//ReleaseSemaphore(semTwo, 1, NULL);
	}

	return 0;
}
