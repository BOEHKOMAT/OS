#include <iostream>
#include <chrono>
#include <cmath>

using namespace std;
using namespace std::chrono;

struct DATA {
    int *arr, size;
    DATA(int a[], int s) : arr(a), size(s) {}
};

void *sort_thread(void *data) {
    auto start = high_resolution_clock::now();
    int *arr = ((DATA *) data)->arr;
    int size = ((DATA *) data)->size;
    int key, j;
    for (int i = 0; i < size; i++) {
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
    cout << double(duration.count()) / pow(10, 6) << endl;
    return nullptr;
}

extern "C" int intersection_sort(int *arr, int count, int size) {
    pthread_t threads[count];
    pthread_attr_t attributes[count];
    for (int i = 0; i < count; i++) {
        pthread_attr_init(&attributes[i]);
        pthread_create(&threads[i], &attributes[i], sort_thread, new DATA(arr, size));
    }
    for (int i = 0; i < count; i++) pthread_join(threads[i], nullptr);
    return 0;
}