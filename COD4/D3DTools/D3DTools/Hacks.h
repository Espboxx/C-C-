#pragma once

#include <Windows.h>
#include "d3d9.h"
#include <ctime> //for the clock
#include <iostream>

#define D3DHOOK_TEXTURES //comment this to disable texture hooking
#define MAX_MENU_ITEMS 6 //so you limit up to make sure everything else run smoothly

//MOVE THESE to the top
//these are just examples you can do your own
#define WALLHACK 0
#define CUSTOM_CROSSHAIR 1
#define NO_RECOIL 2
#define UNLIM_AMMO 3
#define AUTO_FIRE 4
#define HIDE_MENU 5

class Hacks
{
public:
	int m_Stride;
	/*Function prototypes*/
	void Hacks::CreateFont(IDirect3DDevice9 *d3dDevice, std::string choiceFont);
	void Hacks::InitializeMenuItems();
	void Hacks::Draw_Text(LPCSTR TextToDraw, int x, int y, D3DCOLOR Colour);
	void Hacks::DrawMenu(IDirect3DDevice9 *d3dDevice);
	void Hacks::DrawFilledRect(int x, int y, int w, int h, D3DCOLOR color, IDirect3DDevice9* d3dDevice);
	void Hacks::DrawBorderBox(int x, int y, int w, int h, int thickness, D3DCOLOR Colour, IDirect3DDevice9 *d3dDevice);
	void Hacks::KeyboardInput();

	//our colours For the chams textures
	//Spetznas and opfor
	LPDIRECT3DTEXTURE9 texRed;
	LPDIRECT3DTEXTURE9 texGreen;

	//Marines and SAS
	LPDIRECT3DTEXTURE9 texBlue;
	LPDIRECT3DTEXTURE9 texWhite;

	//used to get window size and other info
	D3DVIEWPORT9 g_ViewPort;

	//our font
	LPD3DXFONT m_font;

	//everything each hack needs bay bay
	struct d3dMenuHack
	{
		bool on;
		std::string name;
	};

	d3dMenuHack hack[MAX_MENU_ITEMS];
};