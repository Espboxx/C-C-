#pragma once
#include"�ڴ�ģ��.h"
#include<windows.h>
#include "pch.h"
DWORD Buffer = 0;


struct �������� // ��������
{
	float Matrix[4][4] = { 0 };
};
struct ������
{

	�������� Martx;
	void Read(DWORD ModuleEX, HANDLE Process_handle)
	{
		ReadProcessMemory(Process_handle, LPCVOID(ModuleEX + 0x02D92038), &Buffer, sizeof(Buffer), NULL);
		ReadProcessMemory(Process_handle, LPCVOID(Buffer + 0x3c), &Buffer, sizeof(Buffer), NULL);
		ReadProcessMemory(Process_handle, LPCVOID(Buffer + 0x0), &Buffer, sizeof(Buffer), NULL);
		ReadProcessMemory(Process_handle, LPCVOID(Buffer + 0x70), &Martx, sizeof(Martx), NULL);

	}
	void Mydinate(DWORD ModuleEX, HANDLE Process_handle)
	{
		float �Լ�����[3] = { 0 };


		ReadProcessMemory(Process_handle, LPCVOID(ModuleEX + 0x03003AF8), &Buffer, sizeof(Buffer), NULL);
		ReadProcessMemory(Process_handle, LPCVOID(Buffer + 0x34), &Buffer, sizeof(Buffer), NULL);

		ReadProcessMemory(Process_handle, LPCVOID(Buffer + 0x0), &Buffer, sizeof(Buffer), NULL);
		//X
		ReadProcessMemory(Process_handle, LPCVOID(Buffer + 0x1a8), &�Լ�����, sizeof(�Լ�����), NULL);
	
		////Y
		ReadProcessMemory(Process_handle, LPCVOID(Buffer + 0x1a0), &�Լ�����[1], sizeof(�Լ�����[1]), NULL);
		////Z
		ReadProcessMemory(Process_handle, LPCVOID(Buffer + 0x198), &�Լ�����[2], sizeof(�Լ�����[2]), NULL);

	}

}Myplayer;

struct ��������
{
	double ��������[3] = { 0 };
	//�����������
	int QuantityEx = 0;


	void ZuoBiao(int i, DWORD ModuleEX, HANDLE Process_handle)
	{



		ReadProcessMemory(Process_handle, LPCVOID(ModuleEX + 0x03003AF8), &Buffer, sizeof(Buffer), NULL);
		ReadProcessMemory(Process_handle, LPCVOID(Buffer + 0x34), &Buffer, sizeof(Buffer), NULL);
		ReadProcessMemory(Process_handle, LPCVOID(Buffer + (i - 1) * 8 + 0x0), &Buffer, sizeof(Buffer), NULL);


		//X
		ReadProcessMemory(Process_handle, LPCVOID(Buffer + 0x198), &��������, sizeof(��������), NULL);
		////Y
		ReadProcessMemory(Process_handle, LPCVOID(Buffer + 0x1A0), &��������[1], sizeof(��������[1]), NULL);
		////Z
		ReadProcessMemory(Process_handle, LPCVOID(Buffer + 0x1A8), &��������[2], sizeof(��������[2]), NULL);

	}
}Mydiren;

