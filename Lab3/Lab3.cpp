#include "pch.h"

#include <iostream>
#include <thread>

#include <Windows.h>

using namespace std;

const int writeDelay = 310,
	readDelay = 400;

int buf = 0;

HANDLE hSemWrite,
	hSemRead,
	hMutex;

void consumer()
{
	cout << "[CONSUMER] thread started" << endl;

	while (1)
	{
		Sleep(readDelay);

		WaitForSingleObject(hSemRead, INFINITE);
		WaitForSingleObject(hMutex, INFINITE);

		buf -= 1;
		cout << "[CONSUMER] buf = " << buf << endl;

		ReleaseSemaphore(hSemWrite, 1, NULL);
		ReleaseSemaphore(hSemRead, -1, NULL);
		ReleaseMutex(hMutex);
	}
}

void producer()
{
	cout << "[PRODUCER] thread started" << endl;

	while (1)
	{
		Sleep(writeDelay);

		WaitForSingleObject(hSemWrite, INFINITE);
		WaitForSingleObject(hMutex, INFINITE);

		buf += 1;
		cout << "[PRODUCER] buf = " << buf << endl;

		ReleaseMutex(hMutex);
		ReleaseSemaphore(hSemRead, 1, NULL);
		ReleaseSemaphore(hSemWrite, -1, NULL);
	}
}

int main(int argc, char *argv[])
{
	hSemWrite = CreateSemaphore(NULL, 10, 10, NULL);
	hSemRead = CreateSemaphore(NULL, 0, 10, NULL);
	hMutex = CreateMutex(NULL, FALSE, NULL);

	thread thProducer(producer);
	thread thConsumer(consumer);

	thProducer.join();
	thConsumer.join();

	return 0;
}