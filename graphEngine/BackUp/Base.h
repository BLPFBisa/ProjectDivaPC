#ifndef __BaseH__
#define __BaseH__

#include "defines.h"

namespace base
{
	extern IDirect3DDevice9 *Device;
	extern ID3DXSprite* Sprite;
	extern HWND hwnd;
	extern double FPS, timeDelta;
	extern double SECOND;
	extern int Width, Height;
	extern char Name[];
	extern int fps;

    bool Init(
		HINSTANCE hInstance,     
		bool windowed,           
		D3DDEVTYPE deviceType);

	bool InitD3D(bool windowed,D3DDEVTYPE deviceType);

	bool InitWindow(HINSTANCE hInstance,    
		int width, int height,
		bool windowed);

	bool InitTime();

	void Release();

	void MainLoop();
	int MsgLoop();

	LRESULT CALLBACK WndProc(
		HWND hwnd,
		UINT msg, 
		WPARAM wParam,
		LPARAM lParam);

	//ÓÃ»§º¯Êý
	void BaseSet();
    void Start(IDirect3DDevice9 *Device);

	void Update(double dwTime);
	void Draw(IDirect3DDevice9 *Device);

	void DeleteAll();

	//input
	void OnMouseEvent(bool bLeftButtonDown, bool bRightButtonDown, bool bMiddleButtonDown, bool bSideButton1Down, 
						   bool bSideButton2Down, int nMouseWheelDelta, int xPos, int yPos);
	void OnKeyEvent(UINT nChar, bool bKeyDown, bool bAltDown);
};

#endif