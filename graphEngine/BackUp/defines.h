#ifndef __DefineH__
#define __DefineH__

#define __D3DRM_H__

#include <d3dx9.h>
#include <d3d9.h>
#include <windows.h>
#include <mmsystem.h>
#include <atlbase.h>
#include <stdio.h>
#include <d3d9types.h>
#include <tchar.h>
#include <windowsx.h> 
#include <string>
#include <queue>
#include <time.h>
#include <dshow.h>
#include <vmr9.h>
#include <qedit.h>
#include <streams.h>
#include <map>
#include <string>
#include <fstream>
using namespace std;

#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")
#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"strmiids.lib")
#pragma comment(lib,"quartz.lib")
#ifndef _DEBUG
#pragma comment(lib,"strmbase.lib")
#else
#pragma comment(lib,"strmbasd.lib")
#endif
#pragma comment(linker, "/NODEFAULTLIB:atlthunk.lib")

#define MouseEventValue bLeftButtonDown,bRightButtonDown,bMiddleButtonDown,bSideButton1Down,bSideButton2Down,nMouseWheelDelta,xPos,yPos
#define KeyEventValue nChar, bKeyDown, bAltDown
#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }
#define SAFE_RELEASE_ARRAY(p) { if(p) { delete[] (p); (p)=NULL; }}
#define PI 3.1415926f
#define EPS 1e-7

//keyboard
#define KeyUp 38
#define KeyDown 40
#define KeyLeft 37
#define KeyRight 39
#define KeyEnter 13
#define KeySpace 32
#define KeyEsc 27

#define GameCoreRes 2

//GAMEMODE
#define GAME_MODE_NORMAL 0
#define GAME_MODE_AUTO 1
#define GAME_MODE_PV 2

void ErrorExit(string error_infor,int exit_code);
void trimLeft(string &target);
void trimRight(string &target);
RECT MakeRect(int px,int py,int width,int height);
void SetRectPos(int px,int py, RECT &src);
double GetCurrentCount(void);
LPCWSTR SToW(string tex);
wstring s2ws(const string& s);
void checkTail0(char *s,int maxl);
void wcheckTail0(wchar_t *s,int maxl);
typedef IDirect3DTexture9 Tex,Texture;
bool isImage(string filename);
bool operator==(const RECT &rect1,const RECT &rect2);

class Point
{
public:
	double x,y;
	Point():x(0),y(0) {}
	Point(const double &_x, const double &_y):x(_x),y(_y) {}
	double mod() {return sqrt(x*x+y*y);}
	Point unit() {double tmp = mod(); return Point(x/tmp,y/tmp);}
	Point normal() {return Point(y,-x);}
	friend Point operator+(const Point &a, const Point &b) {return Point(a.x+b.x,a.y+b.y);}
	friend Point operator-(const Point &a, const Point &b) {return Point(a.x-b.x,a.y-b.y);}
	friend Point operator*(const Point &a, const double &b) {return Point(a.x*b,a.y*b);}
	friend double operator*(const Point &a, const Point &b) {return a.x*b.x+a.y*b.y;}
	friend double cross(const Point &a, const Point &b) {return a.x*b.y-b.x*a.y;}
};

#endif