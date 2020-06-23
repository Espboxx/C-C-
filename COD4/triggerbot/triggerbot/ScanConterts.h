#include <windows.h>
#include <iostream>
#include <ctime>
#include <string>



class ScandContents
{
public	:
	int StartX;
	int StartY;
	int DeductX;
	int DeductY;
	int CompareX;
	int CompareY;
	int ThreadNum;

	HDC Hdc;
	

	ScandContents(
		int startX, int startY, HDC hdc, int compareX = 0,
		int compareY = 0, int deductX = 0, int deductY = 0,int threadNum = 0)
	{
		StartX = startX;
		StartY = startY;
		Hdc = hdc;
		CompareX = compareX;
		CompareY = compareY;
		DeductX = deductX;
		DeductY = deductY;
		Hdc = hdc;
		ThreadNum = threadNum;
	}










	 

};