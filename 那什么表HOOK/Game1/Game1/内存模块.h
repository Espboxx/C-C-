#pragma once
#include "pch.h"
//ȡ����ID
DWORD GetProcessID(char* FileName);
//ȡ���ھ��
HWND FindMainWindow(unsigned long process_id);
void KH(int* x, int* y, int* width, int* height);
DWORD GetModuleNamePointer(CONST WCHAR* LPSTRModuleName, DWORD DwPID);
