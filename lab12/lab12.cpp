#include <iostream>
#include <chrono>
#include <pthread.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#define SIZE 20

using namespace std;
using namespace std::chrono;

struct DATA {int *arr;DATA(int *a) : arr(a) {}};

void *sort_thread(void *data) {
    auto start = high_resolution_clock::now();
    int *arr = ((DATA *) data)->arr;
    int i, key, j;
    for (i = 0; i < SIZE; i++) {
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
    for (int &a : arr) {
        a = rand() % 9000 + 1000;
        cout << a << ' ';
    }
    cout << endl;

    pthread_t threads[count];
    pthread_attr_t attributes[count];
    for (int i = 0; i < count; i++) {
        pthread_attr_init(&attributes[i]);
        pthread_create(&threads[i], &attributes[i], sort_thread, new DATA(arr));
    }
    for (int i = 0; i < count; i++) pthread_join(threads[i], nullptr);

    cout << endl;
    for (int &a : arr) cout << a << ' ';

    long size_to_write = SIZE;
    for(int i : arr) size_to_write += to_string(i).length();

    char *map;
    int fd = open("/home/andriy/CLionProjects/lab12/result.txt", O_RDWR | O_CREAT | O_TRUNC, (mode_t)0600);
    lseek(fd, size_to_write, SEEK_SET);
    write(fd, "", 1);
    map = static_cast<char *>(mmap(0, size_to_write, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0));

    int el = 0;
    for (int i : arr) {
        string s = to_string(i);
        for (int j = 0; j < to_string(i).length(); j++)
            map[el++] = s[j];
        map[el++] = ' ';
    }
    munmap(map, size_to_write);
    close(fd);
    return 0;
}