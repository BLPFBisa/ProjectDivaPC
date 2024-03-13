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
#include "..\DShowClass\qedit.h"
#include "..\DShowClass\streams.h"
#include <map>
#include <string>
#include <fstream>
using namespace std;

#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")
#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"strmiids.lib")
#pragma comment(lib,"quartz.lib")

#pragma comment(linker, "/NODEFAULTLIB:atlthunk.lib")

//#define MouseEventValue bLeftButtonDown,bRightButtonDown,bMiddleButtonDown,bSideButton1Down,bSideButton2Down,nMouseWheelDelta,xPos,yPos
#define KeyEventValue nChar, bKeyDown
#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }
#define SAFE_RELEASE_ARRAY(p) { if(p) { delete[] (p); (p)=NULL; }}
#define PI 3.1415926f
#define EPS 1e-7
#define WIDTH 480
#define HEIGHT 272

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
#define GAME_MODE_NORMAL	0
#define GAME_MODE_AUTO	1
#define GAME_MODE_PV	2
#define key_mapsN	4
#define	optionsN	10
#define MAX_STRING_INDEX	100

#define  InsideDrawRange(x,y) ( x>=0 && y>=0 && x<=480 && y<=272 )
#define  InsideDrawRangeEx(x,y) ( x>-20 && y>-20 && x<500 && y<292 )


struct fRECT{
	float left;
	float top;
	float right;
	float bottom;
};

void null();
void ErrorExit(string error_infor,int exit_code);
void trimLeft(string &target);
void trimRight(string &target);
RECT MakeRect(int px,int py,int width,int height);
RECT MakeRect(fRECT &rect);
fRECT MakefRect(RECT &rect);
fRECT MakefRect(int px,int py,int width,int height);
void SetLength(float &dx, float &dy, float set_length);
float UnitLength(float &dx, float &dy);
void RandomDir(float &dx, float &dy);
DWORD RandomColor();
DWORD HSL2RGB(float h, float sl, float l);
void SetRectPos(int px,int py, RECT &src);
LONGLONG GetCurrentCount(void);
LPCWSTR SToW(string tex);
wstring s2ws(const string& s);
void checkTail0(char *s,int maxl);
void wcheckTail0(wchar_t *s,int maxl);
typedef IDirect3DTexture9 Tex,Texture;
bool isImage(string filename);
bool operator==(const RECT &rect1,const RECT &rect2);
string ToString(int num);
string ToString(double num,const char *_Format);

extern const int g_windowSizesN;
extern int g_windowSizes[][2];
extern const int g_StringTableN;
extern string g_StringTable[][2];
extern RECT g_defaultRect;
extern const int cfg_menu_option_idN;
extern int cfg_menu_option_id[];


class Point
{
public:
	float x,y;
	Point():x(0),y(0) {}
	Point(const float &_x, const float &_y):x(_x),y(_y) {}
	float mod() {return sqrt(x*x+y*y);}
	Point unit() {float tmp = mod(); return Point(x/tmp,y/tmp);}
	Point normal() {return Point(y,-x);}
	friend Point operator+(const Point &a, const Point &b) {return Point(a.x+b.x,a.y+b.y);}
	friend Point operator-(const Point &a, const Point &b) {return Point(a.x-b.x,a.y-b.y);}
	friend Point operator*(const Point &a, const float &b) {return Point(a.x*b,a.y*b);}
	friend float operator*(const Point &a, const Point &b) {return a.x*b.x+a.y*b.y;}
	friend float cross(const Point &a, const Point &b) {return a.x*b.y-b.x*a.y;}
};

class SystemIni
{
public:
	SystemIni(){}
	~SystemIni(){}

	//static SystemIni& GetInstance() {static SystemIni instance;return instance;}
	unsigned int *GameKeyMap;

	void Init();

	double sndVolume;
	int winWidth,winHeight;
	bool windowed;
	int soundVolume;
	int GameKeyType; //1-PC 2-PSP 3-O2JAMPC 4-O2JAMPSP
	int winSizeType; //1-480*272 2-720*408 3-960*544 4-×Ô¶¨Òå
	int language;
	int particle;
	int keyHook;
	int autoPause;
	int showFPS;	// 0:OFF 1:ON 2:ON(OSD)

	void ChangeGameKeyType(int type, bool bSave=true);
	void ChangeParticleSystem(int type, bool bSave=true);
	void ChangeLanguage(int type, bool bSave=true);
	void ChangeSndVolume(int volume, bool bSave=true);
	void ChangeWindowSize(int sizeType, bool bSave=true);
	void ChangeKeyHook(int bHook, bool bSave=true);
	void ChangeFPSstatus(int state, bool bSave=true);
	void ChangeAutoPause(int pause_method, bool bSave=true);
	void ChangeToDefault();
	void SetDefaultValue();
	void ApplyAllSettings();

	void SaveIniFile();
	bool LoadIniFile();
};

struct Option{
	string	option_name;
	int		default_value;
	int		min_val, max_val;
	int		*pValue;
	void	(SystemIni::*ChangeSettings)(int, bool);
};
extern Option *options;
extern SystemIni systemIni;

//#define systemIni SystemIni::GetInstance()
#define StringTable(p) g_StringTable[p][systemIni.language]

#endif