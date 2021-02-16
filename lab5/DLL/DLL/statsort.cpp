#include <windows.h>
#include <chrono>
#include <vector>
#include <iostream>
#include "statsort.h"
using namespace std;
using namespace std::chrono;

typedef struct SORT_PARAMS {
	int* ptr, size;
	double* time;
	SORT_PARAMS(int arr[], int s, double* t) : ptr(arr), size(s), time(t) {}
} THREAD_PARAMS;

HANDLE mutex = CreateMutexA(nullptr, FALSE, nullptr);

DWORD WINAPI Thread(LPVOID lpParam) {
	auto start = high_resolution_clock::now();
	int* arr = ((THREAD_PARAMS*)lpParam)->ptr;
	int size = ((THREAD_PARAMS*)lpParam)->size;
	int i, key, j;
	for (i = 0; i < size; i++) {
		WaitForSingleObject(mutex, INFINITE);
		key = arr[i];
		j = i - 1;
		while (j >= 0 && arr[j] < key) {
			arr[j + 1] = arr[j];
			j = j - 1;
		}
		arr[j + 1] = key;
		ReleaseMutex(mutex);
	}
	auto stop = high_resolution_clock::now();
	double* time = ((THREAD_PARAMS*)lpParam)->time;
	*time = double(duration_cast<microseconds>(stop - start).count()) / 1000000;
	return 0;
}

void intersectionSort_topDown(int arr[], int count, int size)
{
	AllocConsole();
	freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
	freopen_s((FILE**)stdin, "CONIN$", "r", stdin);
	HANDLE *threads = new HANDLE[count];
	double *times = new double[count];

	for (int i = 0; i < count; i++) {
		threads[i] = CreateThread(nullptr, 0, Thread, (PVOID) new SORT_PARAMS(arr, size, &times[i]), 0, nullptr);
	}

	WaitForMultipleObjects(DWORD(count), threads, true, INFINITE);


	for (int i = 0; i < count; i++) CloseHandle(threads[i]);
	for (int i = 0; i < count; i++)
		for (int k = i; k < count; k++)
			if (times[i] > times[k]) {
				double tp = times[i];
				times[i] = times[k];
				times[k] = tp;
			}

	for (int i = 0; i < count; i++) cout << times[i] << ' ' << endl;
	for (int i = 0; i < size; i++) cout << arr[i] << ' ';
}