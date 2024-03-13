#ifndef __BaseH__
#define __BaseH__

#include "resource.h"
#include "defines.h"

namespace base
{
	extern IDirect3DDevice9 *Device;
	extern ID3DXSprite* Sprite;
	extern HWND hwnd;
	extern RECT window_rect;
	extern double FPS, timeDelta;
	extern double SECOND;
	extern int Width, Height;
	extern char Name[];
	extern DWORD keyboard_thread_id;

	extern LONGLONG lastTime;
	extern LONGLONG currTime;

	extern DWORD focus_time;
	extern int totalFramesDrawn;
	extern double meanTimeInterval;
	extern LONGLONG beginTime;

	extern DWORD WINAPI KeyboardThreadFunc( void *data );

    bool Init(	HINSTANCE hInstance, bool windowed, D3DDEVTYPE deviceType);
	bool InitD3D(bool windowed, int width, int height, D3DDEVTYPE deviceType);

	bool InitWindow(HINSTANCE hInstance,    
		int width, int height,
		bool windowed);

	bool InitTime();

	void Release();

	void MainLoop();
	int MsgLoop();

	void ResizeWindow(int width=Width, int height=Height);
	void MakeDrawRect(const fRECT &in_rect, float &left, float &top, float &right, float &bottom);
	RECT MakeDrawRect(int left, int top, int right, int bottom);
	fRECT MakeDrawRect(const fRECT &in_rect);
	RECT MakeDrawRect(const RECT &in_rect);
	void MakeDrawRect(float &x, float &y);
	D3DXVECTOR2& MakeDrawRect(D3DXVECTOR2 &v);
	RECT SetWindowRect(int width, int height, bool windowed);
	string sWindowSize();
	int GetAspectRatio( int width, int height );
	fRECT FitWindowRect(fRECT srcRect, fRECT dstRect, bool bCenter);

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
	void OnMouseEvent(int xPos, int yPos, UINT, WPARAM);
	void OnKeyEvent(UINT nChar, bool bKeyDown);
};
void drawSP();

#include "resource.hpp"
extern ResourceLoader g_res;

#endif