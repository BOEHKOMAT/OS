#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <cmath>
#include <chrono>
#include <sys/resource.h>

using namespace std;
using namespace std::chrono;

void tabulation(int priority) {
    id_t id = getpid();
    cout << "PID " << id << " " << getpriority(PRIO_PROCESS, id) << endl;
    setpriority(PRIO_PROCESS, id, priority);

    auto start = high_resolution_clock::now();
    double x = 1, foo = 0;
    for (int n = 0; n < 300000000; n++) {
        foo += pow(x, 2 * n + 1) / (2 * n + 1);
    }
    cout << "Time: " << double(duration_cast<microseconds>(high_resolution_clock::now() - start).count()) / 1000000
         << endl;
    cout << getpriority(PRIO_PROCESS, id) << endl;
}

int main() {
    int count;
    cout << "Enter count of processes: ";
    cin >> count;
    int priorities[] = {};

    while (true) {
        int number, priority;
        cout << "Enter process number(0-" << count - 1 << ") and priority(from -20 to 19): ";
        cin >> number >> priority;
        if (number == 666) break;
        else if (number < 0 || number >= count || priority < -20 || priority > 19) {
            cout << "Incorrect!" << endl;
            continue;
        } else priorities[number] = priority;
    }

    for (int i = 0; i < count; i++) {
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork failed.");
            exit(0);
        } else if (pid == 0) {
            tabulation(priorities[i]);
            // execl("./tabulation", " ", NULL);
            exit(0);
        }
    }

    for (int i = 0; i < count; i++) {
        wait(nullptr);
    }
    return 0;
}
