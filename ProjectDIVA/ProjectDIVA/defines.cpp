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
	{"����ϰ�ߣ�", "Keyboard Style:"},												//0
	{"���ڴ�С��", "Window Size:"},
	{"��Ч������", "Volume:"},
	{"û�м�⵽\n���������Ϸ\n�޷�����", "Cannot locate\ngame file\nCannot play"},
	{"����\n���������Ϸ", "Choose songs to play, preview PV or Demo\n"},
	{"����Diva Online Map Wiki\n�ϲ������µĸ���׷�Ӱ�\n�����ת��Wiki",			//5
	"Search for newest version song packages\nat Diva Online Map Wiki\nClick to goto the Wiki page"},
	{"��Diva OL�������Ժ������ϵ�\n����ڸ���ģʽ��һ����Ϸ\n�����ϸ��Ϣ",
	"In Diva OL, you can play with\nother players through the Internet\nClick for details"},
	{"����������úʹ��ڴ�С\n�Լ���Ч������", "Config keyboard layout,\nwindow size and sound volume"},
	{"���Լ���\n��Ϸ�Ĳ廭��Ӱ��", "View Miku's pictures and\nphoto album"},
	{"GameMaster.\"Project Z1\"\n��Project DIVA PC��\n��ԭ���زİ�Ȩ��SEGA����",
	"GameMaster.\"Project Z1\"\n��Project DIVA PC��\nCopyright belongs to SEGA"},
	{"������Ϸ", "Quit Game"},														//10
	{"���Ե����Ĵ��ڴ�С��", "Available window sizes:"},
	{"\n���Ը���\n��Ч��������������", "\n\nChange the volume\nof sound effects"},
	{"\n\n���Իָ�Ĭ������", "\n\nReset all settings to default"},
	{"��ͣ", "PAUSE"},
	{"����", "CONTINUE"},															//15
	{"����", "RESTART"},
	{"���ز˵�", "BACK TO MENU"},
	{"û�и���", "List is Empty"},
	{"����ģʽ", "Custom Game"},
	{"����ģʽ��û�и�����", "Custom Game (empty list)"},								//20
	{"���ľ���ģʽ��δ������", "Career Mode (not available)"},
	{"�����սģʽ��δ������", "Multiplayer Mode (not available)"},
	{"����", "Settings"},
	{"CG����", "CG Preview"},
	{"������Ϣ", "Other Information"},												//25
	{"�˳���Ϸ", "Exit Game"},
	{"�˵�", "MENU"},
	{"����", "Author"},
	{"����", "Easy"},
	{"��ͨ", "Normal"},																//30
	{"����", "Hard"},
	{"����", "Extra"},
	{"����", "Die"},
	{"��", "Star"},
	{"�Ѷȷ���:%s\n�Ѷ��Ǽ�:%d��\n\n  ��߷�:%.7d\n�������:%.4d\n�������:%s\nͨ�ش���:%d",	//35
	"Difficulty:%s\nStar Level:%d Star\n\nBest score:%.7d\nCombo Max:%.4d\nBest result:%s\n# Times Passed:%d"},
	{"��ȫ����", "(Full Screen)"},
	{"����", "RETRY"},
	{"����", "RETURN"},
	{"�ָ�Ĭ������", "Reset to Default"},
	{"", ""},																				//40
	{"����", "Language"},
	{"����", "English"},
	{"����Ч��", "Particle Effect"},
	{"\n\n������ʾ���ԣ�\n����\nӢ��", "\n\nChange displayed language:\nChinese\nEnglish"},
	{"��������Ч����\n0: ������Ч������죩\n1: ��β���죩\n2: ˫β����ͨ��\n3: ����β�ͣ�������",	//45
	"Change particle system:\n0: no particle (fastest)\n1: single tail (fast)\n2: double tail (normal)\n3: comet tail (slowest)"},
	{	"�𣺣ġ�������\n�����ӡ�������\n��������������\n�����ס�������",
	"��D��������\n����S��������\n����A��������\n����W��������"},
	{	"�𣺡���������\n��������������\n��������������\n��������������",
	"�𣺡�������D\n������������S\n������������A\n������������W"},
	{	"�𣺣ơ�������\n�����ġ�������\n�����ӡ�������\n�����š�������",
	"��F������L\n����D������K\n����S������J\n����E������I"},
	{	"�𣺣̡�������\n�����ˡ�������\n�����ʡ�������\n�����ɡ�������",
	"��L������F\n����K������D\n����J������S\n����I������E"},
	{"���ޣ�", "N/A",},																		//50
	{"ʧ��", "Misstake"},
	{"�ϸ�", "Cheap"},
	{"һ��", "Standard"},
	{"����", "Great"},
	{"����", "Perfect"},																	//55
	{"����", "Times New Roman"},	//��������Ͽ���û������
	{"�ر�", "OFF"},
	{"��", "LOW"},
	{"��", "MEDIUM"},
	{"��", "HIGH"},																			//60
	{"ȫ�ּ��̹�סʧ�ܣ��޷���׼ȷ��ʱ�Ĳ�����̡����з���ǽ������������",
	"Global keyboard hook failed, high-precision keyboard input capture is not available.\r\n"
	"If a firewall notice has pop up, please allow!"},
	{"ȫ�ּ��̹�", "Global KeyHook"},
	{"��ʾ֡Ƶ","Show FPS"},
	{"������", "Enabled"},
	{"�ѽ���", "Disabled"},																	//65
	{"����ʾ", "OFF"},
	{"��ʾ", "ON"},
	{"������ʾ", "ON(OSD)"},
	{"��ʾÿ����֡��\n1.����ʾ\n2.���ڱ�����ʾ\n3.������ʾ",
	"Show FPS\n(Frames per second)\n1. Don't show\n2. Show on window title\n3. On-screen display"},
	{"\n\nʹ��ȫ�ּ��̹���\n��׼ȷ��ʱ�Ĳ����������",
	"\nUse global keyboard hook,\ncapture keyboard input\nmore accurately"},				//70
	{"��ǰFPS=%.2f, ƽ��FPS=%.2f", "CurrFPS=%.2f, AvgFPS=%.2f"},
	{"����ʧ��", "Failed"},
	{"�Զ���ͣ", "Auto Pause"},
	{"�Ӳ�", "Never"},
	{"��ʱ", "PlayOnly"},																	//75
	{"����", "Always"},
	{"�Զ���ͣ��ʽ��\n1. �Ӳ���ͣ\n2. ֻ����Ϸģʽ����ͣ\n3. ������ͣ",
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
///��stringת��ΪLPCWSTR
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
	buffer[realLength] = 0; //��\0��β
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

