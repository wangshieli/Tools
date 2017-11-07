#include <Windows.h>
#include <stdio.h>
#include <TlHelp32.h>
#include <io.h>

#define VERSION "0.0.6"

BOOL EnableDebugPrivilege()
{
	HANDLE hToken;
	BOOL bOK = FALSE;
	if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken))
	{
		TOKEN_PRIVILEGES tp;
		tp.PrivilegeCount = 1;
		if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tp.Privileges[0].Luid))
		{
			printf("cannt lookup");
		}

		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
		if (!AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL))
		{
			printf("adjust error\n");
		}

		bOK = (GetLastError() == ERROR_SUCCESS);
	}
	CloseHandle(hToken);
	return bOK;
}

BOOL KillProcess(DWORD dwPid)
{
	HANDLE hPrc;

	if (0 == dwPid) return FALSE;

	hPrc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);  // Opens handle to the process.  

	if (!TerminateProcess(hPrc, 0)) // Terminates a process.  
	{
		CloseHandle(hPrc);
		return FALSE;
	}
	else
		WaitForSingleObject(hPrc, 1000 * 20); // At most ,waite 2000  millisecond.  

	CloseHandle(hPrc);
	return TRUE;
}

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		printf("²ÎÊý´íÎó\n");
		getchar();
		return 0;
	}

	if (strcmp(argv[1], "-v") == 0)
	{
		printf("version: %s\n", VERSION);
		getchar();
		return 0;
	}

	if (argc != 4)
		return 0;
	
	if (_access(argv[2], 0) != 0 || _access(argv[3], 0) != 0)
		return 0;

	if (!EnableDebugPrivilege())
		return 0;

	DWORD dwPid = atoi(argv[1]);
	KillProcess(dwPid);

	while (true)
	{
		PROCESSENTRY32 pe32;
		pe32.dwSize = sizeof(pe32);
		HANDLE hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (hProcessSnap == INVALID_HANDLE_VALUE)
		{
			Sleep(1000 * 2);
			continue;
		}
		try
		{
			BOOL bMore = ::Process32First(hProcessSnap, &pe32);
			while (bMore)
			{ 
				if (pe32.th32ProcessID == atoi(argv[1]))
				{
					throw new int(10);
				}
				bMore = ::Process32Next(hProcessSnap, &pe32);
			}
		}
		catch (int *e)
		{
			delete e;
			CloseHandle(hProcessSnap);
			continue;
		}

		CloseHandle(hProcessSnap);
		break;
	}

	FILE* fFileRead;
	FILE* fFileWrite;
	fopen_s(&fFileRead, argv[2], "rb");
	if (fFileRead == NULL)
		return 0;

	fseek(fFileRead, 0, SEEK_END);
	int nFileLen = ftell(fFileRead);
	char* p = new char[nFileLen];
	memset(p, 0x00, nFileLen);
	fseek(fFileRead, 0, SEEK_SET);
	int nRead = 0;
	int nCountRead = 0;
	do
	{
		nCountRead = fread(p, 1, nFileLen, fFileRead);
		if (nCountRead > 0)
		{
			nRead += nCountRead;
		}
	} while (nRead < nFileLen);

	int nWrite = 0;
	int nCountWrite = 0;

	fopen_s(&fFileWrite, argv[3], "wb");
	if (fFileWrite == NULL)
	{
		delete p;
		fclose(fFileRead);
		return 0;
	}
		
	do
	{
		nCountWrite = fwrite(p + nWrite, 1, nFileLen, fFileWrite);
		if (nCountWrite > 0)
		{
			nWrite += nCountWrite;
		}
	} while (nWrite < nFileLen);
	fclose(fFileRead);
	fclose(fFileWrite);

	return 0;
}