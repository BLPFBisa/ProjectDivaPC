#include "defines.h"
#include "GameMana.h"
#include <string.h>

#pragma warning (disable : 4996)

SystemIni systemIni;

// This is the standard 16:9 aspect ratio, 480x272 is not exactly 16:9
// Window sizes: 0-480*270 1-720*405 2-1024*576 3-Fullscreen 4-Custom
const int g_windowSizesN = 3;
int g_windowSizes[g_windowSizesN+1][2] = {{480,270},{720,405},{1024,576},{0,0}};
RECT g_defaultRect = {0,0,480,272};

// Language specific texts
const int g_StringTableN = 2;
string g_StringTable[MAX_STRING_INDEX][2]={
	{"按键习惯：", "Keyboard Style:"},												//0
	{"窗口大小：", "Window Size:"},
	{"音效音量：", "Volume:"},
	{"没有检测到\n演奏节奏游戏\n无法进入", "Cannot locate\ngame file\nCannot play"},
	{"可以\n演奏节奏游戏", "Choose songs to play, preview PV or Demo\n"},
	{"可在Diva Online Map Wiki\n上查找最新的歌曲追加包\n点击跳转到Wiki",			//5
	"Search for newest version song packages\nat Diva Online Map Wiki\nClick to goto the Wiki page"},
	{"在Diva OL中您可以和网络上的\n玩家在各种模式中一起游戏\n点击详细信息",
	"In Diva OL, you can play with\nother players through the Internet\nClick for details"},
	{"变更按键设置和窗口大小\n以及音效的音量", "Config keyboard layout,\nwindow size and sound volume"},
	{"可以鉴赏\n游戏的插画和影像", "View Miku's pictures and\nphoto album"},
	{"GameMaster.\"Project Z1\"\n《Project DIVA PC》\n非原创素材版权归SEGA所有",
	"GameMaster.\"Project Z1\"\n《Project DIVA PC》\nCopyright belongs to SEGA"},
	{"结束游戏", "Quit Game"},														//10
	{"可以调整的窗口大小：", "Available window sizes:"},
	{"\n可以更改\n音效、按键音的音量", "\n\nChange the volume\nof sound effects"},
	{"\n\n可以恢复默认设置", "\n\nReset all settings to default"},
	{"暂停", "PAUSE"},
	{"继续", "CONTINUE"},															//15
	{"重试", "RESTART"},
	{"返回菜单", "BACK TO MENU"},
	{"没有歌曲", "List is Empty"},
	{"自由模式", "Custom Game"},
	{"自由模式（没有歌曲）", "Custom Game (empty list)"},								//20
	{"生涯剧情模式（未开启）", "Career Mode (not available)"},
	{"网络对战模式（未开启）", "Multiplayer Mode (not available)"},
	{"设置", "Settings"},
	{"CG鉴赏", "CG Preview"},
	{"其他信息", "Other Information"},												//25
	{"退出游戏", "Exit Game"},
	{"菜单", "MENU"},
	{"作者", "Author"},
	{"容易", "Easy"},
	{"普通", "Normal"},																//30
	{"困难", "Hard"},
	{"超难", "Extra"},
	{"最难", "Die"},
	{"星", "Star"},
	{"难度分类:%s\n难度星级:%d星\n\n  最高分:%.7d\n最高连击:%.4d\n最高评价:%s\n通关次数:%d",	//35
	"Difficulty:%s\nStar Level:%d Star\n\nBest score:%.7d\nCombo Max:%.4d\nBest result:%s\n# Times Passed:%d"},
	{"（全屏）", "(Full Screen)"},
	{"重试", "RETRY"},
	{"返回", "RETURN"},
	{"恢复默认设置", "Reset to Default"},
	{"", ""},																				//40
	{"语言", "Language"},
	{"中文", "English"},
	{"粒子效果", "Particle Effect"},
	{"\n\n更改显示语言：\n中文\n英文", "\n\nChange displayed language:\nChinese\nEnglish"},
	{"更改粒子效果：\n0: 无粒子效果（最快）\n1: 单尾（快）\n2: 双尾（普通）\n3: 彗星尾巴（最慢）",	//45
	"Change particle system:\n0: no particle (fastest)\n1: single tail (fast)\n2: double tail (normal)\n3: comet tail (slowest)"},
	{	"○：Ｄ　→：→\n×：Ｓ　↓：↓\n□：Ａ　←：←\n△：Ｗ　↑：↑",
	"○：D　→：→\n×：S　↓：↓\n□：A　←：←\n△：W　↑：↑"},
	{	"○：→　→：Ｄ\n×：↓　↓：Ｓ\n□：←　←：Ａ\n△：↑　↑：Ｗ",
	"○：→　→：D\n×：↓　↓：S\n□：←　←：A\n△：↑　↑：W"},
	{	"○：Ｆ　→：Ｌ\n×：Ｄ　↓：Ｋ\n□：Ｓ　←：Ｊ\n△：Ｅ　↑：Ｉ",
	"○：F　→：L\n×：D　↓：K\n□：S　←：J\n△：E　↑：I"},
	{	"○：Ｌ　→：Ｆ\n×：Ｋ　↓：Ｄ\n□：Ｊ　←：Ｓ\n△：Ｉ　↑：Ｅ",
	"○：L　→：F\n×：K　↓：D\n□：J　←：S\n△：I　↑：E"},
	{"（无）", "N/A",},																		//50
	{"失败", "Misstake"},
	{"合格", "Cheap"},
	{"一般", "Standard"},
	{"优秀", "Great"},
	{"完美", "Perfect"},																	//55
	{"宋体", "Times New Roman"},	//老外电脑上可能没宋体字
	{"关闭", "OFF"},
	{"低", "LOW"},
	{"中", "MEDIUM"},
	{"高", "HIGH"},																			//60
	{"全局键盘钩住失败，无法更准确及时的捕获键盘。如有防火墙跳出，请允许！",
	"Global keyboard hook failed, high-precision keyboard input capture is not available.\r\n"
	"If a firewall notice has pop up, please allow!"},
	{"全局键盘钩", "Global KeyHook"},
	{"显示帧频","Show FPS"},
	{"已启用", "Enabled"},
	{"已禁用", "Disabled"},																	//65
	{"不显示", "OFF"},
	{"显示", "ON"},
	{"画面显示", "ON(OSD)"},
	{"显示每秒钟帧数\n1.不显示\n2.窗口标题显示\n3.画面显示",
	"Show FPS\n(Frames per second)\n1. Don't show\n2. Show on window title\n3. On-screen display"},
	{"\n\n使用全局键盘钩，\n更准确及时的捕获键盘输入",
	"\nUse global keyboard hook,\ncapture keyboard input\nmore accurately"},				//70
	{"当前FPS=%.2f, 平均FPS=%.2f", "CurrFPS=%.2f, AvgFPS=%.2f"},
	{"启用失败", "Failed"},
	{"自动暂停", "Auto Pause"},
	{"从不", "Never"},
	{"玩时", "PlayOnly"},																	//75
	{"总是", "Always"},
	{"自动暂停方式：\n1. 从不暂停\n2. 只在游戏模式下暂停\n3. 总是暂停",
	"Auto pause method:\n1. Never pause\n2. In play-mode only\n3. Alway pause"},
	{},
	{},
	{},																						//80
	{},
	{},
	{},
	{},
	{},
	{},
	{},
	{},
	{},
	{},
	{},
	{},
};

void null(){}

void ErrorExit(string error_infor,int exit_code)
{
	MessageBox(NULL,error_infor.data(),"DIVA",MB_OK);
	exit(exit_code);
}

void trimLeft(string &target)
{
    if (target.size() != 0)
    {
        for (string::iterator pos = target.begin();  pos < target.end(); )
        {
            if (*pos == ' ')
            {
                target.erase(pos);
                pos = target.begin();
            }
            else
            {
                return;
            }
        }
    }
}

void trimRight(string &target)
{
    if (target.size() != 0)
    {
        for (string::iterator pos = target.end()-1; pos != target.begin(); )
        {
            if (*pos == ' ')
            {
                target.erase(pos);
                pos = target.end();
            }
            else
            {
                return;
            }
        }
    }
}

LONGLONG GetCurrentCount(void)
{
	LARGE_INTEGER tmp;
	QueryPerformanceCounter(&tmp);
	return tmp.QuadPart;
}
RECT MakeRect(int px,int py,int width,int height)
{
	RECT ret;
	ret.top=py;
	ret.left=px;
	ret.right=px+width;
	ret.bottom=py+height;
	return ret;
}
fRECT MakefRect(int px,int py,int width,int height)
{
	fRECT ret;
	ret.top=py;
	ret.left=px;
	ret.right=px+width;
	ret.bottom=py+height;
	return ret;
}
RECT MakeRect(fRECT &rect)
{
	RECT ret;
	ret.top = rect.top+0.5f;
	ret.left = rect.left+0.5f;
	ret.right = rect.right+0.5f;
	ret.bottom = rect.bottom+0.5f;
	return ret;
}
fRECT MakefRect(RECT &rect)
{
	fRECT ret;
	ret.top = rect.top;
	ret.left = rect.left;
	ret.right = rect.right;
	ret.bottom = rect.bottom;
	return ret;
}
void SetRectPos(int px,int py,RECT &src)
{
	RECT ret;
	ret.left = px;
	ret.top = py;
	ret.right = px + src.right - src.left;
	ret.bottom = py + src.bottom - src.top;
	src = ret;
}

void RandomDir(float &dx, float &dy){
	float angle = (float)rand()/RAND_MAX*6.2831853f;
	dx = cos(angle);
	dy = sin(angle);
}
float UnitLength(float &dx, float &dy){
	float length = sqrt(dx*dx+dy*dy);
	if(length>0){
		dx /= length;
		dy /= length;
	}
	return length;
}
void SetLength(float &dx, float &dy, float set_length){
	float length = sqrt(dx*dx+dy*dy);
	if(length>0){
		length = set_length/length;
		dx *= length;
		dy *= length;
	}
}

DWORD RandomColor(){
	BYTE rgb[4]={rand()&0xff,rand()&0xff,rand()&0xff,rand()&0xff};
	return *(DWORD*)rgb;
}

DWORD HSL2RGB(float h, float sl, float l)
{
	float v;
	float r,g,b;

	r = l;   // default to gray
	g = l;
	b = l; 
	v = (l <= 0.5) ? (l * (1.0 + sl)) : (l + sl - l * sl);
	if (v > 0) 
	{
		float m;
		float sv;
		int sextant;
		float fract, vsf, mid1, mid2;

		m = l + l - v;
		sv = (v - m ) / v;
		h *= 6.0;
		sextant = (int)h; 
		fract = h - sextant;
		vsf = v * sv * fract;
		mid1 = m + vsf;
		mid2 = v - vsf;
		switch (sextant) 
		{
		case 0: 
			r = v; 
			g = mid1; 
			b = m; 
			break;
		case 1: 
			r = mid2; 
			g = v; 
			b = m; 
			break;
		case 2: 
			r = m; 
			g = v; 
			b = mid1; 
			break;
		case 3: 
			r = m; 
			g = mid2; 
			b = v; 
			break;
		case 4: 
			r = mid1; 
			g = m; 
			b = v; 
			break;
		case 5: 
			r = v; 
			g = m; 
			b = mid2; 
			break;
		}
	}
	DWORD rgb[4];
	rgb[3] = 0xff;
	rgb[2] = r * 255.0f;
	rgb[1] = g * 255.0f;
	rgb[0] = b * 255.0f;
	return *(DWORD*)rgb;
}

///-------------------------------------
///将string转换为LPCWSTR
///-------------------------------------
LPCWSTR SToW(string tex)
{
	/*
	size_t size = tex.length();
	wchar_t *buffer = new wchar_t[size+1];
	MultiByteToWideChar( CP_ACP, 0, tex.data(), size, buffer, size * sizeof(wchar_t));
	int realLength=0;
	for(int a=0;a<tex.length();a++)
	{
		realLength++;
		if((int)(tex[a])<0) a++;
	}
	buffer[realLength] = 0; //以\0结尾
	return buffer;
	*/
	LPWSTR pszOut = NULL;
	int nInputStrLen = strlen (tex.c_str()); 
	 // Double NULL Termination 
	 int nOutputStrLen = MultiByteToWideChar(CP_ACP, 0, tex.c_str(), nInputStrLen, NULL, 0) + 2; 
	 pszOut = new WCHAR [nOutputStrLen]; 
	 if (pszOut) 
	 { 
		memset (pszOut, 0x00, sizeof (WCHAR)*nOutputStrLen); 
		MultiByteToWideChar (CP_ACP, 0, tex.c_str(), nInputStrLen, pszOut, nInputStrLen); 
	 }
	 wcheckTail0(pszOut,nOutputStrLen);
	 return pszOut;

}

wstring Ansi2WChar(LPCSTR pszSrc, int nLen)
{
	int nSize = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)pszSrc, nLen, 0, 0);
	if(nSize <= 0) return NULL;
	WCHAR *pwszDst = new WCHAR[nSize+1];
	if( NULL == pwszDst) return NULL;
	MultiByteToWideChar(CP_ACP, 0,(LPCSTR)pszSrc, nLen, pwszDst, nSize);
	pwszDst[nSize] = 0;
	if( pwszDst[0] == 0xFEFF) // skip Oxfeff
	for(int i = 0; i < nSize; i ++) 
	pwszDst[i] = pwszDst[i+1]; 
	wstring wcharString(pwszDst);
	delete pwszDst;
	return wcharString;
}

wstring s2ws(const string& s){ return Ansi2WChar(s.c_str(),s.size());}

void checkTail0(char *s,int maxl)
{
	int l=0;
	while(l<maxl)
	{
		if(s[l]==10)
		{
			s[l]=0;
			break;
		}
		l++;
	}
}

void wcheckTail0(wchar_t *s,int maxl)
{
	int l=0;
	while(l<maxl)
	{
		if(s[l]==10)
		{
			s[l]=0;
			break;
		}
		l++;
	}
}

bool isImage(string filename)
{
	string ret;
	for(int i=filename.length()-1;i>=0;i--)
	{
		if(filename[i]=='.')
			break;

		ret=char((filename[i]>='A'&&filename[i]<='Z')?(filename[i]-('A'-'a')):(filename[i]))+ret;
	}
	
	if(ret=="jpg"||ret=="png"||ret=="bmp"||ret=="dds"||ret=="tga")
		return true;
	else
		return false;
}

bool operator==(const RECT &rect1,const RECT &rect2)
{
	return (rect1.left==rect2.left&&rect1.bottom==rect2.bottom&&rect1.right==rect2.right&&rect1.top==rect2.top);
}


bool hasChineseLocale(){
	char str[LOCALE_NAME_MAX_LENGTH];
	if(!GetLocaleInfo(LOCALE_SYSTEM_DEFAULT,LOCALE_SISO639LANGNAME,str,200)) return 1;
	return !strcmp(str,"zh");
}

Option *options = NULL;
const int cfg_menu_option_idN = 8;
int cfg_menu_option_id[cfg_menu_option_idN]={0,1,4,5,6,7,8,9};

void SystemIni::Init()
{
	// Config menu item information
	static Option _options[optionsN]={
		{"GameKeyType", 0, 0, 3, &GameKeyType, &SystemIni::ChangeGameKeyType},
		{"WindowSize", 0, 0, g_windowSizesN, &winSizeType, &SystemIni::ChangeWindowSize},
		{"WindowWidth", g_windowSizes[0][0], -1, -1, &winWidth},
		{"WindowHeight", g_windowSizes[0][1], -1, -1, &winHeight},
		{"SoundVolume", 5, 0, 10, &soundVolume, &SystemIni::ChangeSndVolume},
		{"Language", hasChineseLocale()?0:1, 0, 1, &language, &SystemIni::ChangeLanguage},
		{"Particle", 3, 0, 3, &particle, &SystemIni::ChangeParticleSystem},
		{"KeyHook", 1, 0, 1, &keyHook, &SystemIni::ChangeKeyHook},
		{"AutoPause", 1, 0, 2, &autoPause, &SystemIni::ChangeAutoPause},
		{"FPS", 1, 0, 2, &showFPS, &SystemIni::ChangeFPSstatus},
	};
	if(!options){
		options = _options;
		SetDefaultValue();
	}
	if(!LoadIniFile())
	{
		SaveIniFile();
	}
	ApplyAllSettings();
}

void SystemIni::ApplyAllSettings(){
	ChangeGameKeyType(GameKeyType,false);
	ChangeSndVolume(soundVolume,false);
	ChangeLanguage(language,false);
	ChangeParticleSystem(particle,false);
	ChangeKeyHook(keyHook,false);
}

void SystemIni::ChangeToDefault()
{
	SetDefaultValue();
	ApplyAllSettings();
	ChangeWindowSize(winSizeType,false);	// this one is special
	SaveIniFile();
}

void SystemIni::ChangeFPSstatus(int state, bool bSave)
{
	showFPS = state;
	if(state!=1) SetWindowText(base::hwnd,base::Name);
	if(bSave) SaveIniFile();
}
void SystemIni::ChangeKeyHook(int bHook, bool bSave)
{
	keyHook = bHook;
	if( !keyHook && base::keyboard_thread_id )	// keyHook is on
		PostThreadMessage(base::keyboard_thread_id, WM_QUIT, 0, 0);
	else if( keyHook && !base::keyboard_thread_id )	// keyHook is off
		CreateThread(NULL,0,base::KeyboardThreadFunc,NULL,0,&base::keyboard_thread_id);
	Sleep(10);
	if(bSave) SaveIniFile();
}
static UINT key_maps[key_mapsN][8]={
	{'D', 'A', 'S', 'W', KeyRight, KeyLeft, KeyDown, KeyUp},
	{KeyRight, KeyLeft, KeyDown, KeyUp, 'D', 'A', 'S', 'W'},
	{'F', 'S', 'D', 'E', 'L', 'J', 'K', 'I'},
	{'L', 'J', 'K', 'I', 'F', 'S', 'D', 'E'},
};
void SystemIni::ChangeGameKeyType(int type, bool bSave)
{
	GameKeyType = type;
	if( type>=0 && type<key_mapsN )
		GameKeyMap = key_maps[type];
	if(bSave) SaveIniFile();
}
void SystemIni::ChangeSndVolume(int volume, bool bSave)
{
	soundVolume = volume;
	sndVolume = double(soundVolume)/10.0;
	if(bSave) SaveIniFile();
}
void SystemIni::ChangeWindowSize(int sizeType, bool bSave)
{
	winWidth = g_windowSizes[sizeType][0];
	winHeight = g_windowSizes[sizeType][1];
	windowed = (sizeType!=g_windowSizesN);
	winSizeType = sizeType;
	if(bSave) SaveIniFile();
}
void (Unit::*drawLineFuncs[4])(const Point &, const Point &) = {
	&(Unit::DrawNullLine),
	&(Unit::DrawSingleLine),
	&(Unit::DrawDoubleLine),
	&(Unit::DrawCometLine)
};
void SystemIni::ChangeParticleSystem(int _particle, bool bSave){
	UnitDrawLine = drawLineFuncs[systemIni.particle];
	particle = _particle;
	if(bSave) SaveIniFile();
}
void SystemIni::ChangeLanguage(int _language, bool bSave){
	language = _language;
	if(bSave) SaveIniFile();
}
void SystemIni::ChangeAutoPause(int pause_method, bool bSave)
{
	autoPause = pause_method;
	if(bSave) SaveIniFile();
}

void SystemIni::SaveIniFile()
{
	ofstream fout("config.ini");
	if(fout){
		fout << "[CONFIG]" << endl;
		for( int x=0; x<optionsN; x++ )
			fout << options[x].option_name << " = " << *options[x].pValue << endl;
	}
	fout.close();
}


#include <fstream>
#include <sstream>

void SystemIni::SetDefaultValue(){
	for( int x=0; x<optionsN; x++ )
		*options[x].pValue = options[x].default_value;
}

bool SystemIni::LoadIniFile()
{
	ifstream fin("config.ini");
	int n_options_set = 0;
	if(fin){
		string line;
		while(getline(fin,line)){
			istringstream iss(line);
			string _name, _null;
			int _val;
			if(! (iss>>_name>>_null>>_val)) continue;
			for(int x=0;x<optionsN;x++){
				if(!_name.compare(options[x].option_name)){
					*options[x].pValue = _val;
					n_options_set ++;
					break;
				}
			}
		}
	}

	// Find window size type, support custom window size
	if( winSizeType <= g_windowSizesN ){
		winWidth = g_windowSizes[winSizeType][0];
		winHeight = g_windowSizes[winSizeType][1];
	}
	windowed = (winSizeType!=g_windowSizesN);

	return (n_options_set>=optionsN);
}


string ToString(int num)
{
	char buffer[20];
	itoa(num,buffer,10);
	return string(buffer);
}

string ToString(double num,const char *_Format)
{
	char buffer[20];
	sprintf(buffer,_Format,num);
	return string(buffer);
}

