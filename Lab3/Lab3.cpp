#include "pch.h"
#include <windows.h>
#include <thread>
#include <iostream>
#pragma comment(lib, "user32.lib")

#define VK_q 113
#define VK_a 97
#define VK_w 119
#define VK_s 115

using namespace std;

CRITICAL_SECTION cs;

const auto data_size = 10;
int arr[data_size];
int data_length = 0;

int writeDelay = 300,
	readDelay = 300;

void view()
{
	system("cls");

	printf("Timeouts:\nWrite: %d\nRead: %d\n\n", writeDelay, readDelay);

	printf("Items (%d):\n", data_length);

	if (data_length) {
		for (int i = 0; i < data_length; i++) {
			printf("%d ", arr[i]);
		}
	}
	printf("\n\n");

	if (GetKeyState(VK_LSHIFT) < 0) {
		writeDelay += 100;
	}
	if (GetKeyState(VK_LCONTROL) < 0) {
		if (writeDelay > 100) {
			writeDelay -= 100;
		}
	}
	if (GetKeyState(VK_RSHIFT) < 0) {
		readDelay += 100;
	}
	if (GetKeyState(VK_RCONTROL) < 0) {
		if (readDelay > 100) {
			readDelay -= 100;
		}
	}
}

void thread_write()
{
	do {
		Sleep(writeDelay);
		EnterCriticalSection(&cs);
		if (data_length < data_size) {
			int d = rand() % 90 + 10;
			arr[data_length++] = d;

			view();
			cout << "Write: " << d << endl;
		}
		else {
			cout << "Buffer full (no write)" << endl;
		}
		LeaveCriticalSection(&cs);
	} while (1);
}

void thread_read()
{
	do {
		Sleep(readDelay);
		EnterCriticalSection(&cs);
		if (data_length > 0) {
			int d = arr[0];

			data_length--;
			for (int i = 0; i < data_length; i++) {
				arr[i] = arr[i + 1];
			}

			view();

			cout << "Read: " << d << endl;
		}
		else {
			cout << "No read (empty)" << endl;
		}
		LeaveCriticalSection(&cs);
	} while (1);
}

int main(void)
{
	InitializeCriticalSection(&cs);

	thread thWrite(thread_write);
	thread thRead(thread_read);

	thWrite.join();
	thRead.join();

    return 0;
}
