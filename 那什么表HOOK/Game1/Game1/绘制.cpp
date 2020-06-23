#include"绘制.h"
#include"读数据.h"
#include <iostream>
#include<math.h>
#include<stdio.h>
#include<stdarg.h>
using namespace std;
LPDIRECT3D9  g_pD3D = NULL;
LPDIRECT3DDEVICE9   g_pd3dDevice = NULL; // Our rendering device
ID3DXFont* pFont = NULL;					//D3D文字接口
ID3DXLine* pLine = NULL;				//线条接口
DWORD PID;
DWORD ModuleEX;
HANDLE Process_handle;
float SW;
float SH;
int xE, yE, widthE, heightE;
int ZMobjk;
int Autios;
void DrawL(float x, float y, float w, float h, D3DCOLOR Col)
{
	D3DRECT re = { x,y,w,h };
	g_pd3dDevice->Clear(1, &re, D3DCLEAR_TARGET, Col, 0, 0);
}


void 绘制线条(float x, float y, float x1, float y1, D3DCOLOR col)
{
	D3DXVECTOR2 dLine[2];
	pLine->SetWidth(2.f);//线宽
	dLine[0].x = x;
	dLine[0].y = y;

	dLine[1].x = x1;
	dLine[1].y = y1;

	pLine->Draw(dLine, 2, col);

}
void 绘制文本(const char* textname, int size, int x, int y, D3DCOLOR col)
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
void 绘制方框(float x, float y, float w, float h, float d, D3DCOLOR col)
{
	float W1 = w / 4;
	float H1 = h / 4;
	DrawL(x, y, x + W1 + d, y + d, col);
	DrawL(x + W1 * 3, y, x + w, y + d, col);
	DrawL(x, y + h, x + d + W1, y + h + d, col);
	DrawL(x + W1 * 3, y + h, x + w, y + h + d, col);

	DrawL(x, y, x + d, y + H1, col);
	DrawL(x, y + H1 * 3, x + d, y + h, col);
	DrawL(x + w, y, x + w + d, y + H1, col);
	DrawL(x + w, y + H1 * 3, x + w + d, y + h + d, col);
}
void 绘制方框2(float x, float y, float w, float h, float d, D3DCOLOR col)
{
	DrawL(x, y, x + w, y + d, col);
	DrawL(x, y + h, x + w, y + h + d, col);

	DrawL(x, y, x + d, y + h, col);
	DrawL(x + w - d, y, x + w, y + h, col);

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



void Start()
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
	Mydiren.Quantity(ModuleEX, Process_handle);

	for (i = 1; i < 100; i++)
	{
		Myplayer.Read(ModuleEX, Process_handle);
		Mydiren.ZuoBiao(i, ModuleEX, Process_handle);

		if (矩阵转换(SW, SH, KAMER, Mydiren.敌人坐标))
		{

			FKh = KAMER[1] - KAMER[2];
			FKw = FKh * 0.5;
			FKw1 = FKw / 2;

			绘制方框2(KAMER[0] - FKw1, KAMER[2], FKw, FKh, 1, D3DCOLOR_XRGB(0, 255, 0));


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



HRESULT InitD3D(HWND hWnd)
{
	KH(&xE, &yE, &widthE, &heightE);
	PID = GetProcessID((char*)"hyxd.exe");
	ModuleEX = GetModuleNamePointer((const wchar_t*)TEXT("hyxd.exe"), PID);
	Process_handle = OpenProcess(PROCESS_ALL_ACCESS, 1, GetProcessID((char*)"hyxd.exe"));
	SW = widthE / 2;
	SH = heightE / 2;
	// Create the D3D object, which is needed to create the D3DDevice.
	if (NULL == (g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)))
		return E_FAIL;

	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = hWnd;
	d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
	d3dpp.BackBufferWidth = widthE;
	d3dpp.BackBufferHeight = heightE;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

	if (FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp, &g_pd3dDevice)))
	{
		return E_FAIL;
	}
	if (!pLine)
		D3DXCreateLine(g_pd3dDevice, &pLine);
	return S_OK;
}


VOID Cleanup()
{
	if (g_pd3dDevice != NULL)
		g_pd3dDevice->Release();

	if (g_pD3D != NULL)
		g_pD3D->Release();
}


VOID Render()
{
	if (NULL == g_pd3dDevice)
		return;

	// Clear the backbuffer to a blue color
	g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);

	// Begin the scene
	if (SUCCEEDED(g_pd3dDevice->BeginScene()))
	{
		// Rendering of scene objects can happen here


		Start();
		// End the scene
		g_pd3dDevice->EndScene();
	}

	// Present the backbuffer contents to the display
	g_pd3dDevice->Present(NULL, NULL, NULL, NULL);
}