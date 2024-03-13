#include "defines.h"
#include "UIScreen.h"
#include "keyboard.h"
#include "GameMana.h"
#include "resource.hpp"
#include <string.h>

#pragma warning (disable : 4996)

SystemIni systemIni;

// This is the standard 16:9 aspect ratio, 480x272 is not exactly 16:9
// Window sizes: 0-480*270 1-720*405 2-1024*576 3-Fullscreen 4-Custom
const int g_windowSizesN = 3;
int g_windowSizes[g_windowSizesN+1][2] = {{480,270},{720,405},{1024,576},{0,0}};
RECT g_defaultRect = {0,0,480,272};

// Language specific texts
const int g_numLanguages = 2;
int getMaxSkin();
string g_StringTable[MAX_STRING_INDEX][g_numLanguages]={
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
	{"显示游戏帮助\nhttp://www.gamemastercn.com\n非原创素材版权归SEGA所有",
	 "Show Game Manual\nhttp://www.gamemastercn.com\nCopyright belongs to SEGA"},
	{"结束游戏", "Quit Game"},														//10
	{"可以调整的窗口大小：", "Available window sizes:"},
	{"\n可以更改\n音效、按键音的音量\n按<Enter>键听节拍音", "\nChange the volume\nof sound effects\nPress <Enter> to hear hit sound"},
	{"\n\n恢复所有设置到默认", "\n\nReset all settings to default"},
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
	{"游戏帮助", "Game Help"},												//25
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
	{"语言：", "Language:"},
	{"中文", "English"},
	{"粒子效果：", "Particle Effect:"},
	{"\n\n更改显示语言：\n中文\n英文", "\n\nChange displayed language:\nChinese\nEnglish"},
	{"更改粒子效果：\n0: 无粒子效果（最快）\n1: 单尾（快）\n2: 双尾（普通）\n3: 彗星尾巴（最慢）",	//45
	"Change particle system:\n0: no particle (fastest)\n1: single tail (fast)\n2: double tail (normal)\n3: comet tail (slowest)"},
	//↑↓←→△×□○
	{"按<Enter>键开始设置\n\n【同时按下多键可以\n将多键设为同一键】", "Press <Enter> to change\n\n[Holding down multiple keys\nto map multiple keys\nto the same key]"},
	{"宋体", "Times New Roman"},
	{"楷体", "Courier New"},
	{"黑体", "Arial"},
	{"（无）", "N/A"},																		//50
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
	{"按键捕获", "Key Capture"},
	{"显示帧频","Show FPS"},
	{"已启用", "Enabled"},
	{"已禁用", "Disabled"},																	//65
	{"不显示", "OFF"},
	{"显示", "ON"},
	{"画面显示", "ON(OSD)"},
	{"显示每秒钟帧数\n1.不显示\n2.窗口标题显示\n3.画面显示",
	"Show FPS\n(Frames per second)\n1. Don't show\n2. Show on window title\n3. On-screen display"},
	{"\n\n0: 普通按键 WM_KEYDOWN\n1: 全局键盘钩 WH_KEYBOARD_LL\n2: 使用 DirectInput",
	"\n\n0: Normal key input WM_KEYDOWN\n1: Global Keyhook WH_KEYBOARD_LL\n2: Use DirectInput"},		//70
	{"当前FPS=%.2f, 平均FPS=%.2f", "CurrFPS=%.2f, AvgFPS=%.2f"},
	{"启用失败", "Failed"},
	{"自动暂停", "Auto Pause"},
	{"从不", "Never"},
	{"玩时", "PlayOnly"},																	//75
	{"总是", "Always"},
	{"自动暂停方式：\n1. 从不暂停\n2. 只在游戏模式下暂停\n3. 总是暂停",
	"Auto pause method:\n1. Never pause\n2. In play-mode only\n3. Alway pause"},
	{"视频质量","Video Quality"},
	{"将高清MP4视频转为低清AVI\n以节省CPU资源\n0: 高清MP4 (不转换)\n1: 最高质AVI\n9: 最低质AVI",
	"Convert video from MP4 to AVI\nto save CPU\n0: MP4 (no conversion)\n1: highest quality AVI\n9: lowest quality AVI"},
	{"游戏界面","Game UI"},																						//80
	{"\n选择界面皮肤:\n查看 gamedata*.txt\n当前最大范围：1～"+ToString(getMaxSkin())+"\n打开<CapsLock>预览",
	"\nChoose UI skin:\nSee gamedata*.txt\nCurrent Range: 1~"+ToString(getMaxSkin())+"\nTurn on <CapsLock> to preview"},
	{"AVI 分辨率：\n1～3：1024×576\n4～6：640×360\n7～9：480×270",
	"AVI Resolution：\n1~3：1024x576\n4~6：640x360\n7~9：480x270"},
	{"自定义", "Custom"},
	{"请按顺序依次按下↑↓←→△×□○",
		"Please press ↑↓←→△×□○ in order"},
	{"按<ESC>键取消，<Backspace>回到前一个", "Press <ESC> to cancel, <Backspace> to previous key"}, //85
	{"按<Enter>键确定，按<ESC>键取消", "Press <Enter> to confirm, press <ESC> to cancel"},
	{"字体", "Font"},
	{"\n改变界面字体：\n宋体\n楷体\n黑体", "\nChange UI Font:\nTimes New Roman\nCourier New\nArial"},
	{"空格", "Space"},
	{"或", " or "},			// 90
	{"反锯齿", "Antialiasing"},
	{"反锯齿模式画长条：\n0：(关)软件逐点画(最最慢)\n1：(开)硬件最低(最快)\n...\nN：(开)硬件最高(最慢)\n\n当前选项：\n",
	"Anti-aliasing mode:\n0: (OFF) software draw strip (SLOWEST)\n1: (ON) hardware lowest (fastest)\n...\nN: (ON) hardware highest (slowest)\n\nCurrent option:\n"},
	{"判定模式", "Judgement"},
	{"标准", "Standard"},
	{"严格", "Strict"},			// 95
	{"按键判定方式\n标准：按错的键不会\n抵消下一个键\n严格：判定时间内按错的键\n会抵消下一个键",
	"Scoring key method:\nStandard: wrongly pressed key\nWILL NOT cancel the next note\nStrict: wrongly pressed key\nWILL cancel the next note\nif within time frame"},
	{"(关)\n画长条键很慢", "(OFF)\nVery slow in drawing long strips"},
	{"音键延迟调整", "Note Delay Adjust"},
	{"按键延迟补偿", "Keyboard Delay Compensation"},
	{"延迟补偿", "DelayCompen"},		// 100
	{"\n调整按键信号延迟补偿\n单位：毫秒\n不影响按键音延迟\n游戏中按F5/F6也可调整", "\nKeyboard Delay Compensation\nUnit: in milliseconds\nDoes NOT affect key sound delay\nPress F5/F6 during game does same"},
	{"1. 游戏中开着CapsLock (大写指示灯)，掉血不死，但是只要血减到过0，血槽就会变黑，结束之后一律算 MISTAKE\n"
	 "2. 游戏中按F1~F4调整并保存当前map的音键延迟（音乐文件夹中的calibration.inf），按F5/F6调整并保存按键信号延迟，按PageUp/PageDown调整并保存当前歌曲音量\n"
	 "3. 按+/-可以改变音乐节奏（保持音调），按F11/F12可以改变音调（保持节奏），按F7/F8调节尾巴移动速度\n"
	 "4. 当音调计算关闭时，改变音乐节奏的同时也会改变音调，（因为只能改播放器的采样率），但音乐同步率高、延迟小\n"
	 "5. 在config.ini中加入\"songPath = [歌曲文件夹目录]\\\" 可以修改歌曲目录地址\n"
	 "6. 歌曲文件夹中可以包含子文件夹，方便玩家重新整理歌曲\n",
	 "1. During game, turn on CapsLock to prevent 'death'. However, once HP has gone below 0, HP gauge will turn black, final result will always be 'MISTAKE'\n"
	 "2. During game, press F1~F4 to adjust and save note delay to the current map (see calibration.inf in that song's folder), press F5/F6 to adjust and save keyboard delay compensation, press PageUp/PageDown to adjust and save the volume of the current song\n"
	 "3. Press +/- to adjust music tempo (maintain the same pitch), press F11/F12 to adjust music pitch (maintain the same tempo), press F7/F8 to adjust the speed of tail movement\n"
	 "4. When PitchShiftDSP is disabled, the pitch cannot be changed independently from the tempo, (only the sampling rate is changed), but the music latency/delay is minimal\n"
	 "5. To change the song folder address, add the following line to config.ini, \"songPath = [my_song_folder_path]\\\"\n"
	 "6. The song folder may contain sub-folders now so that you can reorganize your collection of songs\n"},
	{"游戏说明", "Game Help"},
	{"音量", "Volume"},
	{"播放速度", "Play Speed"},		//105
	{"尾巴速度", "Tail Speed"},
	{"音调计算", "PitchShiftDSP"},
	{"\n\n\n音乐节奏改变时保持音调\n电脑配置差会导致不同步",
	 "Maintain the same pitch\nwhen music tempo changes\n\nMay cause music delay\non slow computers"},
	{"音调", "Pitch"},
	{"节奏", "Tempo"},				//110
	{"半音", "Semitone(s)"},
	{"声音系统", "SoundSystem"},
	{"更改声音系统：\nFMOD 库: 变速后时间偏差小\n 无法实现两倍以上变调\nBASS 库: 能实现两倍以上变调\n 变速后时间偏差大",
	 "Change sound system:\nFMOD has better temporal accuracy\n pitch-shift ratio cannot exceed 2\nBASS has wider pitch-shift ratio\n but poorer temporal accuracy"},
};

wstring str2wstr(string &s)
{
	wchar_t *buf = new wchar_t[ s.size() ];
	size_t num_chars = mbstowcs( buf, s.c_str(), s.size() );
	wstring ws( buf, num_chars );
	delete[] buf;
	return ws;
}

void null(){}

void ErrorExit(string error_infor,int exit_code)
{
	MessageBox(NULL,error_infor.data(),"DIVA",MB_OK);
	exit(exit_code);
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


///-------------------------------------
///将string转换为LPCWSTR
///-------------------------------------
LPCWSTR SToW(string tex)
{
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
const int cfg_menu_option_idN = 17;
int cfg_menu_option_id[cfg_menu_option_idN]={0,1,4,5,6,10,11,7,8,9,12,13,14,18,17,23,15};	// Add into 2nd last posi

int getMaxSkin(){
	for(int x=1; true; x++){
		ostringstream oss;
		oss << "gamedata" << x << ".txt";
		ifstream fin(oss.str().c_str());
		if(!fin) return x-1;
	}
}
int unused;

int SystemIni::getAntiAliasingInfo()
{
	multisampling.clear();
	multisamplingN.clear();
	IDirect3D9 *d3d9 = NULL;
	d3d9 = Direct3DCreate9(D3D_SDK_VERSION);
	if(!d3d9) return 0;
	for(int x=0; x<=16; x++){
		DWORD nQuality;
		if(SUCCEEDED(d3d9->CheckDeviceMultiSampleType(
				D3DADAPTER_DEFAULT,
				D3DDEVTYPE_HAL,
				D3DFMT_A8R8G8B8,
				base::d3dpp.Windowed,
				(D3DMULTISAMPLE_TYPE)x,
				&nQuality))){
			for(int y=0; y<nQuality; y++)
				multisampling.push_back(make_pair((D3DMULTISAMPLE_TYPE)x,y));
			multisamplingN.push_back(make_pair((D3DMULTISAMPLE_TYPE)x,nQuality));
		}
		
	}
	d3d9->Release();
	options[13].max_val = multisampling.size();
	if(options[13].pValue)
		if(*options[13].pValue>multisampling.size())
			*options[13].pValue=multisampling.size();
	return multisampling.size();
}

void SystemIni::Init()
{
	// Config menu item information
	static Option _options[optionsN]={
		{"GameKeyType", 0, 0, 4, &GameKeyType, &SystemIni::ChangeGameKeyType, &SystemIni::EnterSetKey},
		{"WindowSize", 1, 0, g_windowSizesN, &winSizeType, &SystemIni::ChangeWindowSize, &SystemIni::ChangeWindowSize},
		{"WindowWidth", g_windowSizes[1][0], -1, -1, &winWidth},
		{"WindowHeight", g_windowSizes[1][1], -1, -1, &winHeight},
		{"SoundVolume", 5, 0, 10, &soundVolume, &SystemIni::ChangeSndVolume,&SystemIni::EnterSndVolume},
		{"Language", hasChineseLocale()?0:1, 0, 1, &language, &SystemIni::ChangeLanguage},
		{"Particle", 3, 0, 3, &particle, &SystemIni::ChangeParticleSystem},
		{"KeyHook", 2, 0, 2, &keyHook, &SystemIni::ChangeKeyHook},
		{"AutoPause", 1, 0, 2, &autoPause, &SystemIni::ChangeAutoPause},
		{"FPS", 0, 0, 2, &showFPS, &SystemIni::ChangeFPSstatus},
		{"LowVideo", 0, 0, 9, &lowVideo, &SystemIni::ChangeOption},			// 10
		{"DataSet", getMaxSkin(), 0, getMaxSkin(), &dataSet, &SystemIni::ChangeDataSet},
		{"Font", 0, 0, 2, &fontIndex, &SystemIni::ChangeFont},
		{"Antialiasing", 1, 0, 1, &antialiasing, &SystemIni::ChangeAntialiasing},
		{"Judgement", 0, 0, 1, &judgement, &SystemIni::ChangeOption},
		{"Reset", 0, 0, 0, &unused, NULL, &SystemIni::EnterReset},
		{"GlobalAudioDelay", 0, -1000, 1000, &GlobalAudioDelay},
		{"PitchShiftDSP", 0, 0, 1, &PitchShiftDSP, &SystemIni::ChangeOption},
		{"DelayCompen", 0, 0, 100, &DelayCompen, &SystemIni::ChangeDelayCompen},
		{"GameLevel", 1, 0, GAME_LEVEL_MAX-1, &game_level, &SystemIni::ChangeOption},
		{"AudioDSPBufferSize", 1024, 256, 2048, &AudioDSPBufferSize},		// 20
		{"AudioDSPBufferCount", 2, 2, 4, &AudioDSPBufferCount},
		{"FMOD_DSP_FFT_WINDOW", 1024, 64, 65536, &FMOD_DSP_FFT_WINDOW},
		{"SoundSystem", 0, 0, 1, &sound_system, &SystemIni::ChangeSoundSystem},
		{"BASS_ATTRIB_TEMPO_OPTION_SEQUENCE_MS", 20, 41, 164, &BASS_ATTRIB_TEMPO_OPTION_SEQUENCE_MS},
		{"BASS_ATTRIB_TEMPO_OPTION_SEEKWINDOW_MS", 10, 14, 56, &BASS_ATTRIB_TEMPO_OPTION_SEEKWINDOW_MS},
		{"BASS_ATTRIB_TEMPO_OPTION_OVERLAP_MS", 10, 4, 16, &BASS_ATTRIB_TEMPO_OPTION_OVERLAP_MS},
	};
	if(!options){
		options = _options;
		SetDefaultValue();
	}
	if(!LoadIniFile())
	{
		SaveIniFile();
	}
	getAntiAliasingInfo();
	ApplyAllSettings();
}

void SystemIni::ApplyAllSettings(){
	ChangeGameKeyType(GameKeyType,false);
	ChangeSndVolume(soundVolume,false);
	ChangeLanguage(language,false);
	ChangeParticleSystem(particle,false);
	ChangeKeyHook(keyHook,false);
	ChangeFont(fontIndex,false);
	ChangeAntialiasing(antialiasing,false);
	ChangeDelayCompen(DelayCompen,false);
	ChangeSoundSystem(sound_system, false);
}

void SystemIni::ChangeToDefault()
{
	SetDefaultValue();
	ApplyAllSettings();
	ChangeWindowSize(winSizeType,false);	// this one is special
	SaveIniFile();
}
void SystemIni::ChangeOption(int state, bool bSave)
{
	if(bSave) SaveIniFile();
}
void SystemIni::ChangeSoundSystem(int system, bool bSave)
{
	bool isFirstTime = true;
	if(soundEngine){
		soundEngine->Free();
		delete soundEngine;
		soundEngine = NULL;
		isFirstTime = false;
	}
	if(system==0)
		soundEngine = new FmodSoundEngine();
	else
		soundEngine = new BassSoundEngine();
	soundEngine->Init();
	if(!isFirstTime)
		soundEngine->PlayMusic("gamedata\\mainmenu.mp3",0,true);
	if(bSave) SaveIniFile();
}
void SystemIni::ChangeDataSet(int data_set, bool bSave)
{
	ostringstream oss;
	oss << "gamedata" << data_set << ".txt";
	g_res = ResourceLoader(oss.str().c_str());
	if(bSave)
	{
		SaveIniFile();
		GAMEDATA.Init();
		gameui.Init();
	}
}
void SystemIni::ChangeFont(int font_id, bool bSave)
{
	for(int x=0; x<g_numLanguages; x++)
		g_StringTable[56][x] = g_StringTable[47+font_id][x];
	if(bSave) SaveIniFile();
}
void SystemIni::ChangeDelayCompen(int delay, bool bSave)
{
	Option &option = options[18];
	if(delay<option.min_val)
		delay=option.min_val;
	if(delay>option.max_val)
		delay=option.max_val;
	DelayCompen = delay;
	base::delayCompenTime = (DelayCompen*0.001*base::SECOND+0.5);
	if(bSave) SaveIniFile();
	if(ScreenState==SCREEN_GAMEMAIN){
		char msg[128];
		sprintf(msg," = %d ms",delay);
		ShowMessage(StringTable(99)+msg,1,0,32);
	}
}
void SystemIni::ChangeFPSstatus(int state, bool bSave)
{
	if(state!=1) SetWindowText(base::hwnd,base::Name);
	if(bSave) SaveIniFile();
}
void SystemIni::ChangeKeyHook(int iHook, bool bSave)
{
	switch(iHook){
	case 0:
		if( base::keyboard_thread_id )
			PostThreadMessage(base::keyboard_thread_id, WM_QUIT, 0, 0);
		Sleep(10);
		break;
	case 1:
		if(dInputKeyboard.status==1)
			dInputKeyboard.stop();
		CreateThread(NULL,0,base::KeyboardThreadFunc,NULL,0,&base::keyboard_thread_id);
		break;
	case 2:
		if( base::keyboard_thread_id )
			PostThreadMessage(base::keyboard_thread_id, WM_QUIT, 0, 0);
		Sleep(10);
		dInputKeyboard.start();
		break;
	}
	if(bSave) SaveIniFile();
}

BYTE default_key_maps[key_mapsN][8]={
	{'D', 'A', 'S', 'W', KeyRight, KeyLeft, KeyDown, KeyUp},
	{KeyRight, KeyLeft, KeyDown, KeyUp, 'D', 'A', 'S', 'W'},
	{'F', 'S', 'D', 'E', 'L', 'J', 'K', 'I'},
	{'L', 'J', 'K', 'I', 'F', 'S', 'D', 'E'},
};
char current_key_map[256];	// maps virtual-key code to UNIT_TYPE (0~7), global, currently in use
char *key_map = current_key_map;

void SystemIni::ChangeGameKeyType(int type, bool bSave)
{
	if( type<key_mapsN ){
		key_map = current_key_map;
		ApplyKeyMap(key_map, default_key_maps[type]);
	}else{
		key_map = custom_key_map;
	}
	if(bSave) SaveIniFile();
}
void SystemIni::ChangeSndVolume(int volume, bool bSave)
{
	soundVolume = volume;
	sndVolume = double(soundVolume)/10.0;
	if(bSave) SaveIniFile();
}
void SystemIni::EnterSndVolume(int volume, bool bSave)
{
	soundEngine->PlayHit(1);
}
void SystemIni::ChangeWindowSize(int sizeType, bool bSave)
{
	winWidth = g_windowSizes[sizeType][0];
	winHeight = g_windowSizes[sizeType][1];
	windowed = (sizeType!=g_windowSizesN);
	winSizeType = sizeType;
	base::ResizeWindow(winWidth, winHeight);
	if(bSave) SaveIniFile();
}
void SystemIni::ChangeAntialiasing(int sizeType, bool bSave)
{
	int index = sizeType?sizeType-1:0;
	UnitDraw_Bezier = (sizeType?&(Unit::Draw_Bezier2):&(Unit::Draw_Bezier1));
	d3dpp.MultiSampleType = multisampling[index].first;
	d3dpp.MultiSampleQuality = multisampling[index].second;
	if(bSave){
		if(base::Device->Reset( &d3dpp ) != D3D_OK)
			while(!base::D3DResetDevice());
		SaveIniFile();
	}
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
void SystemIni::EnterReset(int unused, bool bSave)
{
	soundEngine->PlaySound("gamedata\\Select.wav",systemIni.sndVolume);
	ChangeToDefault();
	base::ResizeWindow(systemIni.winWidth,systemIni.winHeight);
	if(bSave) SaveIniFile();
}

string g_button_symbols[8]={
	"↑", "↓", "←", "→", "△", "×", "□", "○"
};
GUIMessageBox *lock_msgbox = NULL, *old_lock_msgbox = NULL;
string printKey(UINT nChar){
	ostringstream oss;
	switch(nChar){
	case VK_UP:			oss << "↑"; break;
	case VK_DOWN:		oss << "↓"; break;
	case VK_LEFT:		oss << "←"; break;
	case VK_RIGHT:		oss << "→"; break;
	case VK_HOME:		oss << "HOME"; break;
	case VK_END:		oss << "END"; break;
	case VK_PRIOR:		oss << "PageUp"; break;
	case VK_NEXT:		oss << "PageDown"; break;
	case VK_SPACE:		oss << StringTable(89); break;
	case VK_OEM_COMMA:	oss << ","; break;
	case VK_OEM_PERIOD:	oss << "."; break;
	case VK_OEM_MINUS:	oss << "-"; break;
	case VK_OEM_PLUS:	oss << "="; break;
	case VK_OEM_2:		oss << "/"; break;
	case VK_OEM_4:		oss << "["; break;
	case VK_OEM_5:		oss << "\\"; break;
	case VK_OEM_6:		oss << "]"; break;
	case VK_OEM_1:		oss << ";"; break;
	case VK_OEM_7:		oss << "'"; break;
	case VK_NUMPAD0:	oss << "NumPad0"; break;
	case VK_NUMPAD1:	oss << "NumPad1"; break;
	case VK_NUMPAD2:	oss << "NumPad2"; break;
	case VK_NUMPAD3:	oss << "NumPad3"; break;
	case VK_NUMPAD4:	oss << "NumPad4"; break;
	case VK_NUMPAD5:	oss << "NumPad5"; break;
	case VK_NUMPAD6:	oss << "NumPad6"; break;
	case VK_NUMPAD7:	oss << "NumPad7"; break;
	case VK_NUMPAD8:	oss << "NumPad8"; break;
	case VK_NUMPAD9:	oss << "NumPad9"; break;
	case VK_MULTIPLY:	oss << "NumPad*"; break;
	case VK_ADD:      	oss << "NumPad+"; break;
	case VK_SEPARATOR:	oss << "NumPad;"; break;
	case VK_SUBTRACT:	oss << "NumPad-"; break;
	case VK_DECIMAL: 	oss << "NumPad."; break;
	case VK_DIVIDE:		oss << "NumPad/"; break;
	default:
		if(nChar>='A'&&nChar<='Z') oss << (char)nChar;
		else if(nChar>='0'&&nChar<='9') oss << (char)nChar;
		else oss<<"#"<<nChar;
	}
	return oss.str();
}
string printKeys(int key, int maxlen){
	int found = 0;
	string out;
	for(int i=0; i<256; i++){
		if(key_map[i]==key){
			if(found)
				out += StringTable(90);
			found ++;
			out += printKey(i);
		}
	}
	if(found>=7)
		found++;
	return out;
}
void CloseSetKeyMsgbox(){
	if(old_lock_msgbox) delete old_lock_msgbox;
	old_lock_msgbox = lock_msgbox;
	lock_msgbox = NULL;
	menu_configlb_RefreshDetail();
	RefreshConfigDetail();
}
void SetKeyHandler( UINT nChar, bool bKeyDown ){
	if( nChar == VK_ESCAPE ){					// quit menu
		if(bKeyDown){
			lock_msgbox->p_OnMenuSelectBegin = pausedMenu_OnSelectedIndexChanged;
			lock_msgbox->OnKeyEvent(KeyEnter,true);
			return;
		}
	}else if( nChar == VK_BACK ){				// back to previous key
		if(bKeyDown)
			if(lock_msgbox->selectedIndex>0)
				lock_msgbox->OnKeyEvent(VK_UP, true);
	}else if( nChar == VK_RETURN ){				// save key map
		if(bKeyDown){
			lock_msgbox->p_OnMenuSelectBegin = pausedMenu_OnMenuSelectBegin;
			systemIni.SaveKeyMap("keyboard.ini");
			lock_msgbox->OnKeyEvent(KeyEnter,true);
		}
	}else if(lock_msgbox->selectedIndex<8){		// normal key
		if(bKeyDown){
			int x=0, index = lock_msgbox->selectedIndex, unit_id=7-index;
			for(int i=0; i<256; i++)
				if(Keys[i]) x++;
			if(x==1){	// pressing down the 1st key, clear all keys associated with current unit
				for(int i=0; i<256; i++)
					if(key_map[i]==unit_id)
						key_map[i] = -1;
			}
			key_map[nChar] = unit_id;
			lock_msgbox->_text[index]=g_button_symbols[index]+" : "+printKeys(unit_id,40);
		}else{
			int x;
			for(x=0; x<256; x++)
				if(Keys[x]) break;
			if(x==256)	// if all keys are released
				lock_msgbox->OnKeyEvent(VK_DOWN, true);
		}
	}
	if(lock_msgbox)
		lock_msgbox->_option[8]=(lock_msgbox->selectedIndex==8)?86:85;
}
void SystemIni::EnterSetKey(int current_keymap_id, bool bSave)
{
	if(current_keymap_id!=key_mapsN)
		return;
	soundEngine->PlaySound("gamedata\\Select.wav",systemIni.sndVolume);
	GUIMessageBox *set_key_menu = new GUIMessageBox();
	set_key_menu->PrepareMenu(84,400);
	for(int x=0; x<8; x++)
		set_key_menu->AddItem(g_button_symbols[x]+" : "+printKeys(7-x,20));
	set_key_menu->AddItem(85);
	set_key_menu->KeyHandler = SetKeyHandler;
	set_key_menu->p_OnMenuSelectOver = NULL;
	set_key_menu->p_OnMenuReleaseOver = CloseSetKeyMsgbox;
	set_key_menu->p_OnSelectedIndexChanged = pausedMenu_OnSelectedIndexChanged;
	set_key_menu->p_OnMenuSelectBegin = pausedMenu_OnMenuSelectBegin;
	set_key_menu->ShowMenu();
	for(int x=0; x<256; x++)
		Keys[x] = false;

	lock_msgbox = set_key_menu;
}
void SystemIni::SaveIniFile()
{
	ofstream fout("config.ini");
	if(fout){
		fout << "[CONFIG]" << endl;
		for( int x=0; x<optionsN; x++ )
			if(options[x].pValue!=NULL)
				fout << options[x].option_name << " = " << *options[x].pValue << endl;
		if(SongInfoManager::songPath != DEFAULT_SONG_PATH)
			fout << "songPath = " << SongInfoManager::songPath << endl;
		fout.close();
	}
}


#include <fstream>
#include <sstream>

void SystemIni::SetDefaultValue(){
	for( int x=0; x<optionsN; x++ )
		if(options[x].pValue!=NULL)
			*options[x].pValue = options[x].default_value;
}

void SystemIni::ApplyKeyMap(char *key_map, BYTE *km){
	memset(key_map,-1,256);
	for(int x=0; x<8; x++)
		key_map[km[x]] = x;
}

void SystemIni::LoadKeyMap(char *filename){
	ifstream fkb(filename);
	if(fkb){
		for(int x=0; x<256; x++){
			int i;
			if(!(fkb>>i)) break;
			custom_key_map[x] = i;
		}
		if(!fkb){	// invalid keymap file, load default key map 0
			fkb.close();
			ApplyKeyMap(custom_key_map, default_key_maps[0]);
		}
	}else
		ApplyKeyMap(custom_key_map, default_key_maps[0]);
}

void SystemIni::SaveKeyMap(char *filename){
	ofstream fout(filename);
	if(fout){
		for(int x=0; x<256; x++)
			fout << (int)custom_key_map[x] << endl;
		fout.close();
	}
}

bool SystemIni::LoadIniFile()
{
	LoadKeyMap("keyboard.ini");
	ifstream fin("config.ini");
	int n_options_set = 0, n_options_need = 0;
	if(fin){
		// build option name to Option map
		map <string,Option*> option_map;
		for(int x=0; x<optionsN; x++)
			if(options[x].pValue!=NULL)
			{
				option_map[options[x].option_name] = &options[x];
				n_options_need++;
			}

		string line;
		while(getline(fin,line)){
			// read options from file
			istringstream iss(line);
			string _name, _null, _data;
			if(iss>>_name>>_null>>_data)
			{
				int _val;
				if(istringstream(_data)>>_val){
					Option *option = option_map[_name];
					if(option!=NULL)
					{
						*(option->pValue) = _val;
						n_options_set ++;
					}
				}else if(_name=="songPath" && _null[0]=='='){
					string path = line.substr(line.find("=")+1);
					while(path.length()>0){
						if(path[0]==' ') path=path.substr(1);
						else break;
					}
					SongInfoManager::songPath = path;
					SongInfoManager::currPath = path;
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

	return (n_options_set>=n_options_need);
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


Point Bezier(const Point &p0, const Point &p1, const Point &p2, const float &t)
{
	float tt = 1-t;
	return p0*tt*tt+p1*2*t*tt+p2*t*t;
}

Point Bezier(const Point &p0, const Point &p1, const Point &p2, const Point &p3, const float &t)
{
	float tt = 1-t;
	return p0*tt*tt*tt+p1*3*t*tt*tt+p2*3*t*t*tt+p3*t*t*t;
}

Point BezierDir(const Point &p0, const Point &p1, const Point &p2, const Point &p3, const float &t)
{
	float tt = 1-t;
	return (p1-p0)*tt*tt*3 + (p2-p1)*t*tt*6 + (p3-p2)*t*t*3;
}

void to_integer_point(double &val)
{
	if(abs(val-(LONGLONG)val)<1e-8)
		val = (LONGLONG)val;
}

void to_integer_point(float &val)
{
	if(abs(val-(LONGLONG)val)<1e-8)
		val = (LONGLONG)val;
}


