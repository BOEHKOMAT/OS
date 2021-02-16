#include <iostream>
#include <windows.h>
#include <chrono>
#include <vector>

using namespace std;
using namespace std::chrono;
typedef struct SORT_PARAMS {
    int *ptr, size;
    double *time;
    SORT_PARAMS(int arr[], int s, double *t) : ptr(arr), size(s), time(t) {}
} THREAD_PARAMS;

HANDLE mutex = CreateMutexA(nullptr, FALSE, nullptr);

DWORD WINAPI Thread(LPVOID lpParam) {
    auto start = high_resolution_clock::now();
    int *arr = ((THREAD_PARAMS *) lpParam)->ptr;
    int size = ((THREAD_PARAMS *) lpParam)->size;
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
        Sleep(1);
        ReleaseMutex(mutex);
    }
    auto stop = high_resolution_clock::now();
    double *time = ((THREAD_PARAMS *) lpParam)->time;
    double buftime;
    *time = double(duration_cast<microseconds>(stop - start).count()) / 1000000;
    ::InterlockedExchangePointer(reinterpret_cast<void *volatile *>(&buftime),
                                 reinterpret_cast<void *>(duration_cast<microseconds>(stop - start).count() / 1000000));
}

int main()
{
    cout << "=== An Array Top-Down Insert Sort ===" << endl;
    int size;
    cout << "Enter array size:";
    cin >> size;
    int arr[size];

    for (int i = 0; i < size; i++) arr[i] = rand() % 9000 + 1000;

    int count;
    cout << "Enter count of threads: ";
    cin >> count;

    cout << "Sort by " << count << " threads. Please wait..." << endl;

    HANDLE threads[count];
    double times[count];

    for (int i = 0; i < count; i++) {
        threads[i] = CreateThread(nullptr, 0, Thread, (PVOID) new SORT_PARAMS(arr, size, &times[i]), 0, nullptr);
    }

    WaitForMultipleObjects(DWORD(count), threads, true, INFINITE);

    for (HANDLE i: threads) CloseHandle(i);
    for (int i = 0; i < count; i++)
        for (int k = i; k < count; k++)
            if (times[i] > times[k]) {
                double tp = times[i];
                times[i] = times[k];
                times[k] = tp;
            }

    for (double i: times) cout << i << ' ' << endl;
    for (int i : arr) cout << i << ' ';
    return 0;
}