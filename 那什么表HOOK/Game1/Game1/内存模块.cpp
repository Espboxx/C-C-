#include "pch.h"
#include <TlHelp32.h>
#include "内存模块.h"

DWORD GetProcessID(char* FileName)
{

	//句柄
	HANDLE hProcess;
	//存放进程信息和调用成员输出信息
	PROCESSENTRY32 pe;
	BOOL bRet;
	//进行进程快照     可以获取系统中正在运行的进程信息，线程信息，等
	hProcess = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	//开始进程查找     
	bRet = ::Process32First(hProcess, &pe);
	//循环比较，得出ProcessID     
	while (bRet)
	{
		if (strcmp(FileName,(char*)pe.szExeFile) == 0)
			return pe.th32ProcessID;
		else
			bRet = ::Process32Next(hProcess, &pe);
	}
	return 99;
}
DWORD GetModuleNamePointer(CONST WCHAR* LPSTRModuleName, DWORD DwPID)
{
	MODULEENTRY32 lpModuleEntry = { 0 };
	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, DwPID);
	if (!hSnapShot)
		return NULL;
	lpModuleEntry.dwSize = sizeof(lpModuleEntry);
	BOOL __RunModule = Module32First(hSnapShot, &lpModuleEntry);
	while (__RunModule)
	{
		if (!wcscmp((const wchar_t*)lpModuleEntry.szModule, LPSTRModuleName))
		{
			CloseHandle(hSnapShot);
			return (DWORD)lpModuleEntry.modBaseAddr;
		}
		__RunModule = Module32Next(hSnapShot, &lpModuleEntry);
	}
	CloseHandle(hSnapShot);
	return NULL;
}
struct handle_data {
	unsigned long process_id;
	HWND best_handle;
};

inline BOOL IsMainWindow(HWND handle)
{
	return GetWindow(handle, GW_OWNER) == (HWND)0 && IsWindowVisible(handle);
}

inline BOOL CALLBACK EnumWindowsCallback(HWND handle, LPARAM lParam)
{
	handle_data& data = *(handle_data*)lParam;
	unsigned long process_id = 0;
	GetWindowThreadProcessId(handle, &process_id);
	if (data.process_id != process_id || !IsMainWindow(handle)) {
		return TRUE;
	}
	data.best_handle = handle;
	return FALSE;

}
HWND FindMainWindow(unsigned long process_id)
{
	handle_data data;
	data.process_id = process_id;
	data.best_handle = 0;
	EnumWindows(EnumWindowsCallback, (LPARAM)& data);
	return data.best_handle;
}

void KH(int* x, int* y, int* width, int* height)
{
	DWORD PID = GetProcessID((char*)"hyxd.exe");
	RECT rect;
	HWND twnd = FindMainWindow(PID);
	GetWindowRect(twnd, &rect);

	rect.top += 25;
	*x = rect.left;
	*y = rect.top;
	*width = rect.right - rect.left;
	*height = rect.bottom - rect.top;
}
