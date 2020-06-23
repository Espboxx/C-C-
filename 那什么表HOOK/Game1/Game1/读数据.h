#pragma once
#include"内存模块.h"
#include<windows.h>
#include "pch.h"
DWORD Buffer = 0;


struct 矩阵数组 // 矩阵数组
{
	float Matrix[4][4] = { 0 };
};
struct 读数据
{

	矩阵数组 Martx;
	void Read(DWORD ModuleEX, HANDLE Process_handle)
	{
		ReadProcessMemory(Process_handle, LPCVOID(ModuleEX + 0x02D92038), &Buffer, sizeof(Buffer), NULL);
		ReadProcessMemory(Process_handle, LPCVOID(Buffer + 0x3c), &Buffer, sizeof(Buffer), NULL);
		ReadProcessMemory(Process_handle, LPCVOID(Buffer + 0x0), &Buffer, sizeof(Buffer), NULL);
		ReadProcessMemory(Process_handle, LPCVOID(Buffer + 0x70), &Martx, sizeof(Martx), NULL);

	}
	void Mydinate(DWORD ModuleEX, HANDLE Process_handle)
	{
		float 自己坐标[3] = { 0 };


		ReadProcessMemory(Process_handle, LPCVOID(ModuleEX + 0x03003AF8), &Buffer, sizeof(Buffer), NULL);
		ReadProcessMemory(Process_handle, LPCVOID(Buffer + 0x34), &Buffer, sizeof(Buffer), NULL);

		ReadProcessMemory(Process_handle, LPCVOID(Buffer + 0x0), &Buffer, sizeof(Buffer), NULL);
		//X
		ReadProcessMemory(Process_handle, LPCVOID(Buffer + 0x1a8), &自己坐标, sizeof(自己坐标), NULL);
	
		////Y
		ReadProcessMemory(Process_handle, LPCVOID(Buffer + 0x1a0), &自己坐标[1], sizeof(自己坐标[1]), NULL);
		////Z
		ReadProcessMemory(Process_handle, LPCVOID(Buffer + 0x198), &自己坐标[2], sizeof(自己坐标[2]), NULL);

	}

}Myplayer;

struct 敌人属性
{
	double 敌人坐标[3] = { 0 };
	//房间敌人数量
	int QuantityEx = 0;


	void ZuoBiao(int i, DWORD ModuleEX, HANDLE Process_handle)
	{



		ReadProcessMemory(Process_handle, LPCVOID(ModuleEX + 0x03003AF8), &Buffer, sizeof(Buffer), NULL);
		ReadProcessMemory(Process_handle, LPCVOID(Buffer + 0x34), &Buffer, sizeof(Buffer), NULL);
		ReadProcessMemory(Process_handle, LPCVOID(Buffer + (i - 1) * 8 + 0x0), &Buffer, sizeof(Buffer), NULL);


		//X
		ReadProcessMemory(Process_handle, LPCVOID(Buffer + 0x198), &敌人坐标, sizeof(敌人坐标), NULL);
		////Y
		ReadProcessMemory(Process_handle, LPCVOID(Buffer + 0x1A0), &敌人坐标[1], sizeof(敌人坐标[1]), NULL);
		////Z
		ReadProcessMemory(Process_handle, LPCVOID(Buffer + 0x1A8), &敌人坐标[2], sizeof(敌人坐标[2]), NULL);

	}
}Mydiren;

