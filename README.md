# C/C++
关于一些C++代码


原贴：https://blog.csdn.net/xfgryujk/article/details/52739486
##D3DHOOK原理
虚函数表hook
要在D3D程序里绘制自己的东西一般要hook IDirect3DDevice9的EndScene函数，调用这个函数时原程序的绘制已经完成，可以轮到我们绘制了

可以用inline hook来hook这个函数，但是流行的做法是虚函数表hook

先来看看EndScene是怎么被调用的：
游戏修改器制作教程八：D3D函数hook

虚函数表hook的原理就是修改虚函数表中的函数地址，使它指向我们的函数（是不是很像IAT hook）

这里说明一下一般成员函数的调用约定默认是__thiscall，也就是从ecx寄存器传入this指针

但是也可以手动声明成其他调用约定，比如__stdcall，这时就从堆栈传入this指针，相当于this指针是函数的第一个参数

D3D所有成员函数的调用约定都是__stdcall，这就方便了我们获取this指针，直接声明成第一个参数就行了（否则要用内联汇编取ecx的值）

虚函数表hook实现
BOOL hookVTable(void* pInterface, int index, void* hookFunction, void** oldAddress)
{
	void** address = &(*(void***)pInterface)[index];
	if (address == NULL)
		return FALSE;

	// 保存原函数地址
	if (oldAddress != NULL)
		*oldAddress = *address;

	// 修改虚函数表中地址为hookFunction
	DWORD oldProtect, oldProtect2;
	VirtualProtect(address, sizeof(DWORD), PAGE_READWRITE, &oldProtect);
	*address = hookFunction;
	VirtualProtect(address, sizeof(DWORD), oldProtect, &oldProtect2);

	return TRUE;
}

BOOL unhookVTable(void* pInterface, int index, void* oldAddress)
{
	// 修改回原函数地址
	return hookVTable(pInterface, index, oldAddress, NULL);
}

（其实就是拿IATHook改的）

hook EndScene
方法1
为了在程序初始化前hook我们要在主线程运行前远线程注入，首先hook Direct3DCreate9，然后是CreateDevice和EndScene

typedef IDirect3D9* (WINAPI* Direct3DCreate9Type)(UINT SDKVersion);
typedef HRESULT(STDMETHODCALLTYPE* CreateDeviceType)(IDirect3D9* thiz, UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DDevice9** ppReturnedDeviceInterface);
typedef HRESULT(STDMETHODCALLTYPE* EndSceneType)(IDirect3DDevice9* thiz);
Direct3DCreate9Type RealDirect3DCreate9 = NULL;
CreateDeviceType RealCreateDevice = NULL;
EndSceneType RealEndScene = NULL;

IDirect3D9* g_d3d9 = NULL;
IDirect3DDevice9* g_device = NULL;


HRESULT STDMETHODCALLTYPE MyEndScene(IDirect3DDevice9* thiz)
{
	// 这里放我们的绘制代码
	return RealEndScene(thiz);
}

HRESULT STDMETHODCALLTYPE MyCreateDevice(IDirect3D9* thiz, UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DDevice9** ppReturnedDeviceInterface)
{
	unhookVTable(g_d3d9, 16, RealCreateDevice);
	HRESULT res = RealCreateDevice(thiz, Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, ppReturnedDeviceInterface);
	g_device = *ppReturnedDeviceInterface;
	hookVTable(g_device, 42, MyEndScene, &RealEndScene); // EndScene是IDirect3DDevice9第43个函数
	return res;
}

IDirect3D9* WINAPI MyDirect3DCreate9(UINT SDKVersion)
{
	unhookIAT(GetModuleHandle(NULL), "d3d9.dll", "Direct3DCreate9");
	g_d3d9 = RealDirect3DCreate9(SDKVersion);
	hookVTable(g_d3d9, 16, MyCreateDevice, &RealCreateDevice); // CreateDevice是IDirect3D9第17个函数
	return g_d3d9;
}


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		hookIAT(GetModuleHandle(NULL), "d3d9.dll", "Direct3DCreate9", MyDirect3DCreate9, &RealDirect3DCreate9);
		break;

	case DLL_PROCESS_DETACH:
		if (g_device != NULL && RealEndScene != NULL)
			unhookVTable(g_device, 42, RealEndScene);
		break;

	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	}
	return TRUE;
}


方法2
这个方法不需要在主线程运行前注入，CE和直播软件OBS也是用了这个方法，所以推荐使用

理论上同一个类的虚函数地址是一样的，所以我们可以自己创建一个IDirect3DDevice9，然后就能获取虚函数地址了

经测试不同IDirect3DDevice9的虚函数表指针不一样，所以只能用inline hook，不能用虚函数表hook

void* endSceneAddr = NULL;
BYTE endSceneOldCode[sizeof(JmpCode)];


HRESULT STDMETHODCALLTYPE MyEndScene(IDirect3DDevice9* thiz)
{
	// 这里放我们的绘制代码

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
	wc.lpszClassName = _T("DummyWindow");
	if (RegisterClassEx(&wc) == 0)
	{
		MessageBox(NULL, _T("注册窗口类失败"), _T(""), MB_OK);
		return 0;
	}

	HWND hwnd = CreateWindowEx(0, wc.lpszClassName, _T(""), WS_OVERLAPPEDWINDOW, 0, 0, 640, 480, NULL, NULL, wc.hInstance, NULL);
	if (hwnd == NULL)
	{
		MessageBox(NULL, _T("创建窗口失败"), _T(""), MB_OK);
		return 0;
	}

	// 初始化D3D

	IDirect3D9* d3d9 = Direct3DCreate9(D3D_SDK_VERSION);
	if (d3d9 == NULL)
	{
		MessageBox(NULL, _T("创建D3D失败"), _T(""), MB_OK);
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
		MessageBox(NULL, _T("创建设备失败"), _T(""), MB_OK);
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

绘制文字
这部分就直接用D3D的API了，不懂的去看看D3D教程...

HRESULT STDMETHODCALLTYPE MyEndScene(IDirect3DDevice9* thiz)
{
	static RECT rect = { 0, 0, 200, 200 };
	g_font->DrawText(NULL, _T("Hello World"), -1, &rect, DT_TOP | DT_LEFT, D3DCOLOR_XRGB(255, 0, 0));

	return RealEndScene(thiz);
}

HRESULT STDMETHODCALLTYPE MyCreateDevice(IDirect3D9* thiz, UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DDevice9** ppReturnedDeviceInterface)
{
	unhookVTable(g_d3d9, 16, RealCreateDevice);
	HRESULT res = RealCreateDevice(thiz, Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, ppReturnedDeviceInterface);
	g_device = *ppReturnedDeviceInterface;

	D3DXFONT_DESC d3dFont = {};
	d3dFont.Height = 25;
	d3dFont.Width = 12;
	d3dFont.Weight = 500;
	d3dFont.Italic = FALSE;
	d3dFont.CharSet = DEFAULT_CHARSET;
	wcscpy_s(d3dFont.FaceName, L"Times New Roman");
	D3DXCreateFontIndirect(g_device, &d3dFont, &g_font);

	// 测试中不知道为什么第一次调用DrawText后device的虚函数表会恢复，没办法只好在hook前调用一次
	static RECT rect = { 0, 0, 200, 200 };
	g_font->DrawText(NULL, _T("Hello World"), -1, &rect, DT_TOP | DT_LEFT, D3DCOLOR_XRGB(255, 0, 0));

	hookVTable(g_device, 42, MyEndScene, &RealEndScene); // EndScene是IDirect3DDevice9第43个函数

	return res;
}
hook D3D函数可以实现在游戏中显示自己的UI（播放Bad Apple），或者在绘制某些东西的时候禁用Z轴缓冲实现透视
