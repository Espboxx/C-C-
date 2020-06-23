// dllmain.cpp : 定义 DLL 应用程序的入口点。

#include "pch.h"
#include <stdio.h>
#include <d3dx9.h>
#include <Windows.h>
#include "读数据.h"
#include <string>
#include <sstream>  
using namespace std;

ID3DXFont* pFont = NULL;					//D3D文字接口
ID3DXLine* pLine = NULL;				//线条接口s
DWORD PID;
DWORD ModuleEX;
HANDLE Process_handle;
float SW;
float SH;
int xE, yE, widthE, heightE;
int ZMobjk;
int Autios;

#pragma pack(push)
#pragma pack(1)
#ifndef _WIN64
struct JmpCode
{
private:
	const BYTE jmp;
	DWORD address;

public:
	JmpCode(DWORD srcAddr, DWORD dstAddr)
		: jmp(0xE9)
	{
		setAddress(srcAddr, dstAddr);
	}

	void setAddress(DWORD srcAddr, DWORD dstAddr)
	{
		address = dstAddr - srcAddr - sizeof(JmpCode);
	}
};
#else
struct JmpCode
{
private:
	BYTE jmp[6];
	uintptr_t address;

public:
	JmpCode(uintptr_t srcAddr, uintptr_t dstAddr)
	{
		static const BYTE JMP[] = { 0xFF, 0x25, 0x00, 0x00, 0x00, 0x00 };
		memcpy(jmp, JMP, sizeof(jmp));
		setAddress(srcAddr, dstAddr);
	}

	void setAddress(uintptr_t srcAddr, uintptr_t dstAddr)
	{
		address = dstAddr;
	}
};
#endif
#pragma pack(pop)

void hook(void* originalFunction, void* hookFunction, BYTE* oldCode)
{
	JmpCode code((uintptr_t)originalFunction, (uintptr_t)hookFunction);
	DWORD oldProtect, oldProtect2;
	VirtualProtect(originalFunction, sizeof(code), PAGE_EXECUTE_READWRITE, &oldProtect);
	memcpy(oldCode, originalFunction, sizeof(code));
	memcpy(originalFunction, &code, sizeof(code));
	VirtualProtect(originalFunction, sizeof(code), oldProtect, &oldProtect2);
}

void unhook(void* originalFunction, BYTE* oldCode)
{
	DWORD oldProtect, oldProtect2;
	VirtualProtect(originalFunction, sizeof(JmpCode), PAGE_EXECUTE_READWRITE, &oldProtect);
	memcpy(originalFunction, oldCode, sizeof(JmpCode));
	VirtualProtect(originalFunction, sizeof(JmpCode), oldProtect, &oldProtect2);
}


void* endSceneAddr = NULL;
BYTE endSceneOldCode[sizeof(JmpCode)];
ID3DXFont* g_font = NULL;
//绘制

void DrawL(float x, float y, float w, float h, D3DCOLOR Col, IDirect3DDevice9* g_pd3dDevice)
{
	D3DRECT re = { x,y,w,h };
	g_pd3dDevice->Clear(1, &re, D3DCLEAR_TARGET, Col, 0, 0);
}




void 绘制文本(const char* textname, int size, int x, int y, D3DCOLOR col , IDirect3DDevice9* g_pd3dDevice)
{

	D3DXCreateFontA(g_pd3dDevice, size, 0, FW_HEAVY, 1, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "雅黑", &pFont);
	char buf[1024] = { 0 };
	va_list va_alist;
	RECT FontPos = { x, y, x + 120, y + 12 };
	va_start(va_alist, textname);
	vsprintf_s(buf, textname, va_alist);
	va_end(va_alist);
	pFont->DrawTextA(NULL, buf, -1, &FontPos, DT_NOCLIP, col);
	pFont->Release();
	pFont = nullptr;

}

void 绘制方框(float x, float y, float w, float h, float d, D3DCOLOR col, IDirect3DDevice9* g_pd3dDevice)
{
	float W1 = w / 4;
	float H1 = h / 4;
	DrawL(x, y, x + W1 + d, y + d, col, g_pd3dDevice);
	DrawL(x + W1 * 3, y, x + w, y + d, col, g_pd3dDevice);
	DrawL(x, y + h, x + d + W1, y + h + d, col, g_pd3dDevice);
	DrawL(x + W1 * 3, y + h, x + w, y + h + d, col, g_pd3dDevice);

	DrawL(x, y, x + d, y + H1, col, g_pd3dDevice);
	DrawL(x, y + H1 * 3, x + d, y + h, col, g_pd3dDevice);
	DrawL(x + w, y, x + w + d, y + H1, col, g_pd3dDevice);
	DrawL(x + w, y + H1 * 3, x + w + d, y + h + d, col, g_pd3dDevice);
}

void 绘制方框2(float x, float y, float w, float h, float d, D3DCOLOR col, IDirect3DDevice9* g_pd3dDevice)
{
	DrawL(x, y, x + w, y + d, col, g_pd3dDevice);
	DrawL(x, y + h, x + w, y + h + d, col, g_pd3dDevice);

	DrawL(x, y, x + d, y + h, col, g_pd3dDevice);
	DrawL(x + w - d, y, x + w, y + h, col, g_pd3dDevice);

}

int 取准星最近敌人(float x, float y, float xa, float ya)
{
	float X = fabs(x - xa);
	float Y = fabs(y - ya);

	return sqrt(X * X + Y * Y);

}


bool 矩阵转换(int width, int height, float* Kmare, double* ZBiao)
{

	float Z = Myplayer.Martx.Matrix[0][3] * Mydiren.敌人坐标[0] + Myplayer.Martx.Matrix[1][3] * Mydiren.敌人坐标[1] + Myplayer.Martx.Matrix[2][3] * Mydiren.敌人坐标[2] + Myplayer.Martx.Matrix[3][3];
	float Skalierung = 1 / Z;
	if (Z > 5)
	{

		//X
		Kmare[0] = width + (Myplayer.Martx.Matrix[0][0] * Mydiren.敌人坐标[0] + Myplayer.Martx.Matrix[1][0] * Mydiren.敌人坐标[1] + Myplayer.Martx.Matrix[2][0] * Mydiren.敌人坐标[2] + Myplayer.Martx.Matrix[3][0]) * Skalierung * width;
		//Y
		Kmare[1] = height - (Myplayer.Martx.Matrix[0][1] * Mydiren.敌人坐标[0] + Myplayer.Martx.Matrix[1][1] * (Mydiren.敌人坐标[1] - 1.2) + Myplayer.Martx.Matrix[2][1] * Mydiren.敌人坐标[2] + Myplayer.Martx.Matrix[3][1]) * Skalierung * height;

		//Y1
		Kmare[2] = height - (Myplayer.Martx.Matrix[0][1] * Mydiren.敌人坐标[0] + Myplayer.Martx.Matrix[1][1] * (Mydiren.敌人坐标[1] + 0.6) + Myplayer.Martx.Matrix[2][1] * Mydiren.敌人坐标[2] + Myplayer.Martx.Matrix[3][1]) * Skalierung * height;


	}


	return true;
}

bool 自瞄(int i)
{

	Mydiren.ZuoBiao(i, ModuleEX, Process_handle);
	float Z = Myplayer.Martx.Matrix[0][3] * Mydiren.敌人坐标[0] + Myplayer.Martx.Matrix[1][3] * Mydiren.敌人坐标[1] + Myplayer.Martx.Matrix[2][3] * Mydiren.敌人坐标[2] + Myplayer.Martx.Matrix[3][3];
	float Skalierung = 1 / Z;
	if (Z > 5)
	{

		//X
		float KmareX = SW + (Myplayer.Martx.Matrix[0][0] * Mydiren.敌人坐标[0] + Myplayer.Martx.Matrix[1][0] * Mydiren.敌人坐标[1] + Myplayer.Martx.Matrix[2][0] * Mydiren.敌人坐标[2] + Myplayer.Martx.Matrix[3][0]) * Skalierung * SW;
		//Y
		float KmareY = SH - (Myplayer.Martx.Matrix[0][1] * Mydiren.敌人坐标[0] + Myplayer.Martx.Matrix[1][1] * Mydiren.敌人坐标[1] + Myplayer.Martx.Matrix[2][1] * Mydiren.敌人坐标[2] + Myplayer.Martx.Matrix[3][1]) * Skalierung * SH;
		//float KmareY1= SH - (Myplayer.Martx.Matrix[0][1] * Mydiren.敌人坐标[0] + Myplayer.Martx.Matrix[1][1] * (Mydiren.敌人坐标[1] - 1.2) + Myplayer.Martx.Matrix[2][1] * Mydiren.敌人坐标[2] + Myplayer.Martx.Matrix[3][1]) * Skalierung * SH;
		////Y1
		//float KmareY2= SH - (Myplayer.Martx.Matrix[0][1] * Mydiren.敌人坐标[0] + Myplayer.Martx.Matrix[1][1] * (Mydiren.敌人坐标[1] + 0.6) + Myplayer.Martx.Matrix[2][1] * Mydiren.敌人坐标[2] + Myplayer.Martx.Matrix[3][1]) *Skalierung * SH;
		//float FKh = KmareY1 - KmareY2;

		KmareX = (KmareX - SW) / 2 * 0.2;
		KmareY = (KmareY - SH) / 2 * 0.2;
		mouse_event(1, KmareX, KmareY, 0, 0);

	}


	return true;
}


void Start(IDirect3DDevice9* g_pd3dDevice)
{
	float Autiostest = 100000;
	int i;
	//窗口宽
	float FKw = 0;
	//窗口高
	float FKh = 0;
	//窗口宽一半的量
	float FKw1 = 0;
	float KAMER[4] = { 0 };
	char pbuf[255];

	for (i = 1; i < 10; i++)
	{
		char pbuf[255];
		
		

		Myplayer.Read(ModuleEX, Process_handle);
		Mydiren.ZuoBiao(i, ModuleEX, Process_handle);

		snprintf(pbuf, 255, "敌人%d:X:%d,Y:%d,Z:%d", i,Mydiren.敌人坐标[0] , Mydiren.敌人坐标[1], Mydiren.敌人坐标[2] );
		绘制方框(100, 100, 100, 100, 10, D3DCOLOR_XRGB(255, 0, 0), g_pd3dDevice);
		

		绘制文本(pbuf,20,0 ,i*21, D3DCOLOR_XRGB(255, 0, 0), g_pd3dDevice);


		if (矩阵转换(SW, SH, KAMER, Mydiren.敌人坐标))
		{

			FKh = KAMER[1] - KAMER[2];
			FKw = FKh * 0.5;
			FKw1 = FKw / 2;

			绘制方框2(KAMER[0] - FKw1, KAMER[2], FKw, FKh, 1, D3DCOLOR_XRGB(0, 255, 0), g_pd3dDevice);


			Autios = 取准星最近敌人(SW, SH, KAMER[0], KAMER[2]);
			if (Autios < Autiostest && GetAsyncKeyState(VK_RBUTTON) == 0)
			{
				{
					Autiostest = Autios;
					ZMobjk = i;
				}

			}
			if (GetAsyncKeyState(VK_RBUTTON) != 0)
			{
				自瞄(ZMobjk);
			}
		}



	}
}

HRESULT STDMETHODCALLTYPE MyEndScene(IDirect3DDevice9* thiz)
{
	

	//绘制文本("123123",20,0,0, D3DCOLOR_XRGB(255, 0, 0),thiz);
	
	//绘制方框(100,100,100,100,10, D3DCOLOR_XRGB(255, 0, 0),thiz);
	绘制方框(100, 100, 100, 100, 10, D3DCOLOR_XRGB(255, 0, 0), thiz);
	Start(thiz);

	unhook(endSceneAddr, endSceneOldCode);
	HRESULT hr = thiz->EndScene();
	hook(endSceneAddr, MyEndScene, endSceneOldCode);

	return hr;
}

DWORD WINAPI initHookThread(LPVOID dllMainThread)
{
	// 等待DllMain（LoadLibrary线程）结束
	WaitForSingleObject(dllMainThread, INFINITE);
	CloseHandle(dllMainThread);

	// 创建一个窗口用于初始化D3D

	

	WNDCLASSEX wc = {};
	wc.cbSize = sizeof(wc);
	wc.style = CS_OWNDC;
	wc.hInstance = GetModuleHandle(NULL);
	wc.lpfnWndProc = DefWindowProc;
	wc.lpszClassName = L"DummyWindow";
	if (RegisterClassEx(&wc) == 0)
	{
		MessageBox(NULL, L"注册窗口类失败", L"", MB_OK);
		return 0;
	}

	HWND hwnd = CreateWindowEx(0, wc.lpszClassName, L"", WS_OVERLAPPEDWINDOW, 0, 0, 640, 480, NULL, NULL, wc.hInstance, NULL);
	if (hwnd == NULL)
	{
		MessageBox(NULL, L"创建窗口失败", L"", MB_OK);
		return 0;
	}

	// 初始化D3D

	IDirect3D9* d3d9 = Direct3DCreate9(D3D_SDK_VERSION);
	if (d3d9 == NULL)
	{
		MessageBox(NULL, L"创建D3D失败", L"", MB_OK);
		DestroyWindow(hwnd);
		return 0;
	}

	D3DPRESENT_PARAMETERS pp = {};
	pp.Windowed = TRUE;
	pp.SwapEffect = D3DSWAPEFFECT_COPY;

	IDirect3DDevice9* device;
	if (FAILED(d3d9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING, &pp, &device)))
	{
		MessageBox(NULL, L"创建设备失败", L"", MB_OK);
		d3d9->Release();
		DestroyWindow(hwnd);
		return 0;
	}

	// hook EndScene
	endSceneAddr = (*(void***)device)[42]; // EndScene是IDirect3DDevice9第43个函数
	hook(endSceneAddr, MyEndScene, endSceneOldCode);
	
	// 释放
	d3d9->Release();
	device->Release();
	DestroyWindow(hwnd);
	return 0;
}


BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)

{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		// 取当前线程句柄
		HANDLE curThread;
		if (!DuplicateHandle(GetCurrentProcess(), GetCurrentThread(), GetCurrentProcess(), &curThread, SYNCHRONIZE, FALSE, 0))
			return FALSE;
		// DllMain中不能使用COM组件，所以要在另一个线程初始化
		CloseHandle(CreateThread(NULL, 0, initHookThread, curThread, 0, NULL));

		KH(&xE, &yE, &widthE, &heightE);
		PID = GetProcessID((char*)"hyxd.exe");
		ModuleEX = GetModuleNamePointer((const WCHAR*)"hyxd.exe", PID);
		Process_handle = OpenProcess(PROCESS_ALL_ACCESS, 1, GetProcessID((char*)"hyxd.exe"));
		SW = (float)widthE / 2;
		SH = (float)heightE / 2;


		break;

	case DLL_PROCESS_DETACH:
		if (endSceneAddr != NULL)
			unhook(endSceneAddr, endSceneOldCode);
		break;

	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	}
	return TRUE;
}