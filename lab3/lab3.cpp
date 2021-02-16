#include <iostream>
#include <windows.h>
#include <chrono>
#include <conio.h>

using namespace std;

FILETIME ft[3];
u_int64 qwTotalTimeElapsed;

typedef struct THREAD_PARAMS{
    int *ptr, start, end;
    THREAD_PARAMS(int arr[], int s, int e):ptr(arr),start(s),end(e){}
}THREAD_PARAMS;

int priority[7] = {THREAD_PRIORITY_TIME_CRITICAL, THREAD_PRIORITY_HIGHEST, THREAD_PRIORITY_ABOVE_NORMAL,
                   THREAD_PRIORITY_NORMAL, THREAD_PRIORITY_BELOW_NORMAL, THREAD_PRIORITY_LOWEST, THREAD_PRIORITY_IDLE};

u_int64 FileTimeToQuadWord(PFILETIME pft){return(Int64ShllMod32(pft->dwHighDateTime, 32) | pft->dwLowDateTime);}

DWORD WINAPI Thread(LPVOID lpParam)
{
    int *arr = ((THREAD_PARAMS *) lpParam)->ptr;
    int start = ((THREAD_PARAMS *) lpParam)->start;
    int end = ((THREAD_PARAMS *) lpParam)->end;
    int i, key, j;
    for (i = start; i < end; i++) {
        key = arr[i];
        j = i - 1;
        while (j >= 0 && arr[j] < key) {
            arr[j + 1] = arr[j];
            j = j - 1;

        }
        arr[j + 1] = key;
    }
}

int main()
{
    cout << "=== An Array Top-Down Insert Sort ===" << endl;

    int size;
    cout << "Enter array size:";
    cin >> size;

    int *arr = new int[size];

    for (int i = 0; i < size; i++) arr[i] = rand() % 11000 - 1000;

    cout << "Sort by 1 thread. Please wait..." << endl;

    HANDLE hThread = CreateThread(nullptr, 0, Thread, (PVOID)(new THREAD_PARAMS(arr, 0, size - 1)), 0, nullptr);
    WaitForSingleObject(hThread, INFINITE);
    GetThreadTimes(hThread, &ft[2], &ft[2], &ft[0], &ft[1]);
    qwTotalTimeElapsed = FileTimeToQuadWord(&ft[0]) + FileTimeToQuadWord(&ft[1]);
    printf("%.7f",(double)qwTotalTimeElapsed/10000000);

    CloseHandle(hThread);

    for (int i = 0; i < size; i++) arr[i] = rand() % 9000 + 1000;

    int count;
    cout << "\nNow try to sorting by few threads" << endl << "Enter count of threads: ";
    cin >> count;

    int step = size / count;

    cout << "Sort by " << count << " threads. Please wait..." << endl;

    HANDLE threads[count];
    for (int i = 0; i < count; i++)
        threads[i] = CreateThread(nullptr, 0, Thread, (PVOID)(new THREAD_PARAMS(arr, i*step, i == count-1? size-1 : i*step+step-1)), CREATE_SUSPENDED, nullptr);

    while(true)
    {
        int command;
        cout << "1 - Change thread priority" << endl << "2 - Run threads"<< endl<< "->";
        cin >> command;
        switch(command)
        {
            case 1:
                while(_getch() != 's')
                {
                    cout << "Select thread number (1-" << count << "): ";
                    int number;
                    cin >> number;
                    cout << "Select priority(1-critical 2-highest 3-above 4-normal 5-below 6-lowest 7-idle): ";
                    int set;
                    cin >> set;
                    SetThreadPriority(threads[number-1],priority[set - 1]);
                }
                break;
            case 2:
                for(HANDLE i: threads) ResumeThread(i);
                goto link;
            default:break;
        }
    }
    link:
    WaitForMultipleObjects(DWORD(count),threads,true,INFINITE);
    cout << "Threads' work time:";
    for(HANDLE i : threads)
    {
        GetThreadTimes(i, &ft[2], &ft[2], &ft[0], &ft[1]);
        qwTotalTimeElapsed = FileTimeToQuadWord(&ft[0]) + FileTimeToQuadWord(&ft[1]);
        printf("%.7f\n",(double)qwTotalTimeElapsed/10000000);

    }

    for(HANDLE i: threads) CloseHandle(i);

    return 0;
}