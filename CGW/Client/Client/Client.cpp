#include <windows.h> 
#include <stdio.h>
#include <conio.h>
#include <tchar.h>
#include <string>
#include <iostream>

using namespace std;

#define BUFSIZE 512

void copy(LPSTR lpvMessage, string str)
{
	for (int i = 0; i < str.size(); i++)
		lpvMessage[i] = str[i];
	lpvMessage[str.size()] = '\0';
}

int _tmain(int argc, CHAR* argv[])
{
	setlocale(LC_ALL, "Russian");
	HANDLE pipe;
	LPSTR message = (LPSTR)new string(BUFSIZE, '\0');
	CHAR buffer[BUFSIZE];
	BOOL is_success = FALSE;
	DWORD read_bytes, write_bytes, written_bytes, dwMode;
	LPSTR name = (LPSTR)"\\\\.\\pipe\\pipe";

	while (true)
	{
		pipe = CreateFileA(name, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
		if (pipe != INVALID_HANDLE_VALUE)
			break;
		if (GetLastError() != ERROR_PIPE_BUSY)
			{_tprintf(TEXT("Неможливо вiдкрити канал. GLE=%d\n"), GetLastError());return -1;}
		if (!WaitNamedPipeA(name, 20000))
			{printf("Неможливо вiдкрити канал: 20 секунд очiкування.");return -1;}
	}
	
	string str;
	while (true)
	{
		getline(cin, str);
		if (str == "quit") break;
		copy(message, str);
		dwMode = PIPE_READMODE_MESSAGE;
		is_success = SetNamedPipeHandleState(pipe, &dwMode, NULL, NULL);
		if (!is_success)
			{_tprintf(TEXT("Помилка!. GLE=%d\n"), GetLastError());return -1;}

		write_bytes = (lstrlenA(message) + 1) * sizeof(CHAR);
		is_success = WriteFile(pipe, message, write_bytes, &written_bytes, NULL);

		if (!is_success)
			{_tprintf(TEXT("Помилка запису. GLE=%d\n"), GetLastError());return -1;}

		do
		{
			is_success = ReadFile(pipe, buffer, BUFSIZE * sizeof(CHAR), &read_bytes, NULL);
			if (!is_success && GetLastError() != ERROR_MORE_DATA)
				break;
		} while (!is_success);

		if (!is_success)
			{_tprintf(TEXT("ReadFile from pipe failed. GLE=%d\n"), GetLastError());return -1;}
	}
	CloseHandle(pipe);
}
