// SystemProg_Lab1_by_A.Datskiv.cpp : Defines the entry point for the console application.
//

#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <string>
#include <tchar.h> 
#include <iostream>
#include <conio.h>

using namespace std;

int main(int argc, char* argv[])
{
	SECURITY_ATTRIBUTES sa;
	sa.nLength = sizeof(sa);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;


	/*
	Створюємо іменований мютекс:
	1) Задаємо ім'я
	2) І створюємо об'єкт ядра, нас перший процес
	*/
	const char* MutexName_1 = "MutexNumber_1";
	wchar_t wtext1[20];
	mbstowcs(wtext1, MutexName_1, strlen(MutexName_1) + 1);
	LPWSTR ptr1 = wtext1; //---> записуємо ім'я нашого мютекса в змінну типу LPWSTR

	HANDLE Mutex_1 = CreateMutex(NULL, FALSE, ptr1); //NULL означає, що ми не зможемо унаслідувати наш мютекс
													 //дочірнім процесам
													 //Створюємо перший семафор
	const char* SemaphoreName_1 = "SemaphoreNumber_1";
	mbstowcs(wtext1, SemaphoreName_1, strlen(SemaphoreName_1) + 1);
	LPWSTR ptr2_1 = wtext1;

	HANDLE hSemaphore_1 = CreateSemaphore(NULL, 1, 1, ptr2_1);

	//Створюємо другий семафор
	const char* SemaphoreName_2 = "SemaphoreNUmber_2";
	mbstowcs(wtext1, SemaphoreName_2, strlen(SemaphoreName_2) + 1);
	LPWSTR ptr2_2 = wtext1;

	HANDLE hSemaphore_2 = CreateSemaphore(NULL, 1, 1, ptr2_2);

	//Створюємо масив семафорів
	HANDLE hSemaphores[2];
	hSemaphores[0] = hSemaphore_1;
	hSemaphores[1] = hSemaphore_2;

	//Створюємо таймер
	const char* Timer_Name = "Timer_1";
	mbstowcs(wtext1, Timer_Name, strlen(Timer_Name) + 1);
	LPWSTR ptr3 = wtext1;

	HANDLE hTimer = CreateWaitableTimer(NULL, true, ptr3);

	//Інтервал часу для таймера
	LARGE_INTEGER liDueTime;
	liDueTime.QuadPart = -50000000LL;


	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		puts("Process #2:");
		puts("---------------------------------");
		printf("Mutex:\n `%s` already exists!\n", MutexName_1);
		printf("Semaphores:\n `%s` and `%s` already exists!\n", SemaphoreName_1, SemaphoreName_2);
		printf("Timer:\n `%s` already exists!\n", Timer_Name);

		puts("\nMutex:");
		puts("---------------------------------");

		HANDLE hMyMutexInherited = (HANDLE)atoi(argv[1]);

		printf("%s (%d) : 0x%X\n", argv[0], argc, hMyMutexInherited);

		switch (WaitForSingleObject(Mutex_1, INFINITE))
		{
		case WAIT_OBJECT_0:
		{
			puts("Caught mutex!");
			puts("Mutex is free!");
			ReleaseMutex(Mutex_1);
		} break;

		case WAIT_FAILED:
		{
			puts("Don`t catch mutex!");
		} break;

		case WAIT_ABANDONED:
		{
			puts("Wait abandoned!");
		} break;

		default:
			break;
		}

		puts("\nSemaphore:");
		puts("---------------------------------");

		HANDLE hSemaphoreInherited_1 = (HANDLE)atoi(argv[2]);
		printf("%s (%d) : 0x%X\n", argv[0], argc, hSemaphoreInherited_1);
		HANDLE hSemaphoreInherited_2 = (HANDLE)atoi(argv[3]);
		printf("%s (%d) : 0x%X\n", argv[0], argc, hSemaphoreInherited_2);
		HANDLE hSemaphoresInherited[2];
		hSemaphoresInherited[0] = hSemaphoreInherited_1;
		hSemaphoresInherited[1] = hSemaphoreInherited_2;

		if (WaitForMultipleObjects(2, hSemaphoresInherited, true, INFINITE) == WAIT_OBJECT_0)
		{
			cout << "Caught semaphores." << endl;
			ReleaseSemaphore(hSemaphoreInherited_1, 1, NULL);
			ReleaseSemaphore(hSemaphoreInherited_2, 1, NULL);
		}
		else
		{
			cout << "Error in waiting semaphores." << GetLastError() << endl;
		}


		cout << "\nTimer:" << endl;
		puts("---------------------------------");

		HANDLE hTimerInherited = (HANDLE)atoi(argv[4]);
		printf("%s (%d) : 0x%X\n", argv[0], argc, hTimerInherited);

		cout << "Timer start for " << (long)&liDueTime << endl;

		//Запускаємо таймер
		SetWaitableTimer(hTimerInherited, &liDueTime, 0, NULL, NULL, 0);


		if (WaitForSingleObject(hTimerInherited, INFINITE) != WAIT_OBJECT_0)
		{
			cout << "SetWaitableTimer failed: " << GetLastError() << endl;
		}
		else
		{
			cout << "\nTimer was signaled.\n";
			for (int i = 0; i < 10; ++i)
			{
				Beep(750, 200);
			}
		}

		CloseHandle(hMyMutexInherited);
		CloseHandle(Mutex_1);

		CloseHandle(hSemaphoresInherited[0]);
		CloseHandle(hSemaphoresInherited[1]);
		CloseHandle(hSemaphores[0]);
		CloseHandle(hSemaphores[1]);

		CloseHandle(hTimerInherited);
		CloseHandle(hTimer);

		_getch();
	}
	else
	{
		puts("Process #1:");
		puts("---------------------------------");

		HANDLE hMyMutexInherited = CreateMutex(&sa, FALSE, NULL);

		HANDLE hSemaphoreInherited_1 = CreateSemaphore(&sa, 1, 1, NULL);
		HANDLE hSemaphoreInherited_2 = CreateSemaphore(&sa, 1, 1, NULL);

		HANDLE hSemaphoresInherited[2];
		hSemaphoresInherited[0] = hSemaphoreInherited_1;
		hSemaphoresInherited[1] = hSemaphoreInherited_2;

		HANDLE hTimerInherited = CreateWaitableTimer(&sa, true, NULL);

		if (hMyMutexInherited != NULL || hSemaphoreInherited_1 != NULL || hSemaphoreInherited_2 != NULL || hTimerInherited != NULL)
		{
			printf("Mutex 0x%X was created.\n", hMyMutexInherited);
			printf("Semaphores 0x%X and 0x%X were created.\n", hSemaphoresInherited[0], hSemaphoresInherited[1]);
			printf("Timer 0x%X was created.\n", hTimerInherited);
			puts("---------------------------------");

			STARTUPINFO si = { sizeof(si) };
			PROCESS_INFORMATION pi;
			char cmdLn[255];
			sprintf_s(cmdLn, "SystemProgramming #1 %d %d %d %d\0", hMyMutexInherited, hSemaphoreInherited_1, hSemaphoreInherited_2, hTimerInherited);

			wchar_t wtext[255];
			mbstowcs(wtext, cmdLn, strlen(cmdLn) + 1);
			LPWSTR ptr = wtext;

			_getch();

			if (CreateProcess(TEXT("C:\\Users\\Gesko927\\OneDrive\\Visual Projects\\SystemProg_Lab1_by_A.Datskiv\\Debug\\SystemProg_Lab1_by_A.Datskiv.exe"), ptr, NULL, NULL, TRUE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi))
			{
				puts("Process 2 was created.");
				puts("---------------------------------");
			}
			else
			{
				puts("Failed to create process!");
				puts("---------------------------------");
			}

			puts("Mutex is free");
			puts("Semaphores is free");

			system("pause>>void");

			ReleaseMutex(hMyMutexInherited);

			ReleaseSemaphore(hSemaphoresInherited[0], 1, NULL);
			ReleaseSemaphore(hSemaphoresInherited[1], 1, NULL);

			CloseHandle(hMyMutexInherited);
			CloseHandle(hSemaphoresInherited[0]);
			CloseHandle(hSemaphoresInherited[1]);
			CloseHandle(hTimerInherited);
		}
		else
		{
			puts("Error with creating of Mutex/Semaphore/Timer");
		}

		CloseHandle(Mutex_1);
		CloseHandle(hSemaphores[0]);
		CloseHandle(hSemaphores[1]);
		CloseHandle(hTimer);

	}
	return 0;
}
