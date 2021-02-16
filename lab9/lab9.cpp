#include <iostream>
#include <chrono>
#include <cmath>
#include <unistd.h>

#define SIZE 30

using namespace std;
using namespace std::chrono;

struct sched_param param;

struct DATA {
    int *arr, begin, end, priority;
    DATA(int a[], int b, int e, int p) : arr(a), begin(b), end(e), priority(p) {}
};

void* sort_thread(void *data) {
    auto start = high_resolution_clock::now();
    int  policy;
    param.sched_priority = ((DATA *) data)->priority;
    pthread_setschedparam(pthread_self(), SCHED_RR, &param);
    pthread_getschedparam(pthread_self(), &policy, &param);
    //cout << policy << endl;
    int *arr = ((DATA *) data)->arr;
    int begin = ((DATA *) data)->begin;
    int end = ((DATA *) data)->end;
    int i, key, j;
    for (i = begin; i < end; i++) {
        key = arr[i];
        j = i - 1;
        while (j >= 0 && arr[j] < key) {
            arr[j + 1] = arr[j];
            j = j - 1;
        }
        arr[j + 1] = key;
    }
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    return nullptr;
}

int main() {
    int count;
    cout << "Enter the count of threads:";
    cin >> count;
    int arr[SIZE];
    for (int &a : arr) { a = rand() % 9000 + 1000; cout << a << ' ';}
    cout << endl;
    int step = SIZE / count;
    pthread_t threads[count];
    pthread_attr_t attributes[count];
    int priorities[] = {};
    /*while(true)
    {
        int priority, number;
        cout << "Enter number of thread and priority: ";
        cin >> number >> priority;
        if(number == 666) break;
        else priorities[number] = priority;
    }*/
    for (int i = 0; i < count; i++) {
        pthread_attr_init(&attributes[i]);
        pthread_create(&threads[i], &attributes[i], sort_thread,
                       new DATA(arr, i * step, i == count - 1 ? SIZE - 1 : i * step + step - 1, i));
    }
    for (int i = 0; i < count; i++) pthread_join(threads[i], nullptr);
    cout << endl;
    for (int &a : arr) cout << a << ' ';
    return 0;
}