#pragma once
#include "pch.h"
#include <d3dx9.h>
#include <d3d9.h>
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "d3d9.lib")


HRESULT InitD3D(HWND hWnd);

VOID Cleanup();
VOID Render();
