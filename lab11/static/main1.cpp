#include <iostream>

using namespace std;

extern "C" int intersection_sort(int arr[], int count, int size);

int main()
{
    int count, size;
    cout << "Enter size and count of thread: ";
    cin >> size >> count;
    int arr[size];
    for (int & a : arr) { a = rand() % 900 + 100; cout << a << ' ';}
    intersection_sort(arr, count, size);
    for (int & a : arr)  cout << a << ' ';
    return 0;
}