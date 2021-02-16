#include <iostream>
#include <tchar.h>
#include <ctime>
#include <Windows.h>
using namespace std;

PROCESS_INFORMATION processes[32];
DWORD priority[6] = { REALTIME_PRIORITY_CLASS, HIGH_PRIORITY_CLASS, ABOVE_NORMAL_PRIORITY_CLASS, NORMAL_PRIORITY_CLASS, BELOW_NORMAL_PRIORITY_CLASS, IDLE_PRIORITY_CLASS };

int main()
{
	int count;
	cout << "Enter a count of processes: ";
	cin >> count;
	for (int i = 0; i < count; i++)
	{
		STARTUPINFO si;
		PROCESS_INFORMATION pi;
		ZeroMemory(&si, sizeof(STARTUPINFO));
		si.cb = sizeof(STARTUPINFO);
		ZeroMemory(&pi, sizeof(STARTUPINFO));
		TCHAR name[] = "C:/Users/micro/source/repos/Tabulation/Debug/Tabulation.exe";

		if (CreateProcess(name, NULL, NULL, NULL, 0, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi))
		{
			cout << "Process " << i << " was created!" << endl;
			processes[i] = pi;
		}
		else
			cout << "Error! " << GetLastError() << endl;
	}
	while (true)
	{
		int num, action;
		DWORD flag, exit_code = 0;;
		cout << "1 - restore" << endl << "2 - pause" << endl << "3 - stop" << endl << "4 - kill" << endl << "5 - priority"<< endl << "6 - info" << endl << "7 - exit" << endl;
		cin >> action;
		switch (action)
		{
			case 1:
				cout << "Select the process number: ";
				cin >> num;
				flag = ResumeThread(processes[num].hThread);
				if (flag > DWORD(1))
					cout << "The selected process are suspended!" << endl;
				else if (flag == DWORD(1))
					cout << "The selected process was restored!" << endl;
				else if (flag == DWORD(0))
					cout << "The selected process was not stopped!" << endl;
				else if (flag == DWORD(-1))
					cout << "Error! "<< GetLastError() << endl;
				break;
			case 2:
				cout << "Select the process number: ";
				cin >> num;
				if (SuspendThread(processes[num].hThread) != DWORD(-1))
					cout << "The selected process was suspended! " << num << endl;
				else
					cout << "Error! " << GetLastError() << endl;
				break;
			case 3:
				cout << "Select the process number: ";
				cin >> num;		
				TerminateThread(processes[num].hThread, 0);
				break;
			case 4:
				cout << "Select the process number: ";
				cin >> num;
				if (TerminateProcess(processes[num].hProcess, 0) != 0)
					cout << "Successful!" << endl;
				else
					cout << "Error! " << GetLastError() << endl;
				break;
			case 5:
				cout << "Select the process number: ";
				cin >> num;
				int choice;
				cout << "  1 - Real Time priority" << endl << "  2 - High priority" << endl << "  3 - Above Normal priority" << endl << "  4 - Normal priority" << endl << "  5 - Below Normal priority" << endl << "  6 - Idle priority" << endl;
				cin >> choice;
				if(SetPriorityClass(processes[num].hProcess, priority[choice - 1]) != 0)
					cout << "Successful!" << endl;
				else
					cout << "Error! " << GetLastError() << endl;
				break;
			case 6:
				for (int i = 0; i < count; i++)
				{
					FILETIME ft[4];
					DWORD isActive = 0;
					double vars[2];
					GetExitCodeProcess(processes[i].hProcess, &isActive);
					if (GetProcessTimes(processes[i].hProcess, &ft[0], &ft[1], &ft[2], &ft[3]) != -1)
					{
						cout << "Process #" << i << " Active? " << ((isActive == STILL_ACTIVE)?"true":"false");
						for (int k = 0; k < 2; k++)
						{
							LARGE_INTEGER date;
							date.HighPart = ft[k].dwHighDateTime;
							date.LowPart = ft[k].dwLowDateTime;
							vars[k] = (date.QuadPart - 11644473600000 * 10000) / 10000000;
						}
						cout << " Work time: " << ((isActive == STILL_ACTIVE) ? (time(nullptr) - vars[0]) : (vars[1] - vars[0])) << endl;
					}
				}
				break;
			case 7:
				goto link;
				break;
			case 666:
				for (int i = 0; i < count; i++)
					TerminateProcess(processes[i].hProcess, 0);
				break;
		}
	}
	link:
	return 0;
}