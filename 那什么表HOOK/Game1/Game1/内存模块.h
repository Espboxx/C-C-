#pragma once
#include "pch.h"
//取进程ID
DWORD GetProcessID(char* FileName);
//取窗口句柄
HWND FindMainWindow(unsigned long process_id);
void KH(int* x, int* y, int* width, int* height);
DWORD GetModuleNamePointer(CONST WCHAR* LPSTRModuleName, DWORD DwPID);
