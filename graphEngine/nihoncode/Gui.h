#ifndef __GuiH__
#define __GuiH__

#include<vector>
#include<string>
#include "Base.h"
#include "graphEngine.h"
using namespace std;



//-----------Effect System-------------
class BaseEffect
{
public:
	virtual bool OnWork(){ return false; }
	 virtual void Pause(){paused = true;}
	 virtual void Start(){paused = false;}
protected:
	bool paused;
};


class GUIEF_Alpha : public BaseEffect
{
private:
	int *_pAlpha;
	float _nowAlpha,_speed,_dest;
	int _delay;
public:
	GUIEF_Alpha(int *pAlpha,float speed,float destination,int delay = 0);
	bool OnWork();
};


//----------��˸-------------
class Spark : public BaseEffect
{
private:
	int *_pAlpha;
	float _speed,_end,_minAlpha,_maxAlpha,_nowAlpha;
	int restlife;
	bool isDowning;
public:
	Spark(int *pAlpha,float speed,bool startAtTop,int life,float startAlpha,float endAlpha,float minAlpha = 0.0f,float maxAlpha = 255.0f);

	bool OnWork();
};

//---------����E���----------
class GUIEF_Move : public BaseEffect
{
private:
	RECT *_drawRect;
	int _desx,_desy,_beginx,_beginy,_width,_height;
	int _maxLife,_nowLife,_delay;
public:
	GUIEF_Move(RECT *drawRect, int maxLife, int desx, int desy,int delay = 0);

	bool OnWork();
};

//---------������ɁE---------
class GUIEF_Stretch : public BaseEffect
{
private:
	RECT *_pRect;
	RECT _beginRect,_desRect;
	int _maxLife,_nowLife,_delay;
public:
	GUIEF_Stretch(RECT *pRect, int maxLife, RECT desRect, int delay = 0);

	bool OnWork();
};


#define MAXEFFECTNUM 20
//--------Effect Vector------
class EffectVector :public BaseEffect
{
public:
	EffectVector();
	~EffectVector();
	virtual bool OnWork();
	virtual void setPointer(int *src_a,int *src_r,int *src_g,int *src_b,LPRECT src_drawRect,LPRECT src_srcRect)
	{a=src_a;r=src_r;g=src_g;b=src_b;drawRect=src_drawRect;srcRect=src_srcRect;}

	void AddEffect(BaseEffect *effect);		//Add the effect
	void Pause(){for(int a=0;a<effectnums;a++) effects[a]->Pause();}	//Pause the effects
	void Start(){for(int a=0;a<effectnums;a++) effects[a]->Start();}	//Start the effects
	void ReleaseEffects();	//Delete All effects
	void EffectDie(int id);	//Delete the dead effect 

	int effectnums;
	int *a,*r,*g,*b;
	LPRECT drawRect,srcRect;
	BaseEffect *effects[MAXEFFECTNUM];

};

//-----------End Effect System----------




class GUIControl
{
protected:
	int tab;
	bool visible;
	bool canGetFocus;
	int focus;
	bool focusOn;
	RECT modifyRect;
	vector<GUIControl*> panel;
public:
	RECT drawRect;

	GUIControl();
	~GUIControl();
	virtual void Draw(int Basex = 0,int Basey = 0);
	virtual void Update();
	virtual void OnKeyEvent(unsigned int nChar, bool bKeyDown, bool bAltDown);
	RECT GetDrawRect(){return drawRect;}

	virtual void Focus(){focusOn=true;}
	virtual void LostFocus(){focusOn=false;}

	virtual void SetTab(int _tab) {tab=_tab;}
	virtual void SetPos(int x,int y) {int width=drawRect.right-drawRect.left,height = drawRect.bottom - drawRect.top;drawRect=MakeRect(x,y,width,height);}

	virtual void Register(GUIControl* src);
	virtual void UnRegister(GUIControl* src);
};

typedef GUIControl Panel,Form;

class GUIStaticImage : public GUIControl
{
private:
	int _a,_r,_g,_b;
	Texture *drawTex;
	
public:
	RECT srcRect;
	EffectVector efvct;

	GUIStaticImage(string filename,RECT _srcRect,RECT _drawRect,int texPool = -1);

	virtual void Draw(int Basex = 0,int Basey = 0);
	virtual void Update();

	virtual void SetColor(int a,int r,int g,int b);
};


class GUIVertStretchBlock : public GUIControl
{
private:
	RECT _drawRect;
public:
	GUIStaticImage *lSide,*rSide,*middle;

	GUIVertStretchBlock(string lSideFilename, RECT _lSideSrcRect,
						string rSideFilename, RECT _rSideSrcRect,
						string midFilename,RECT _midSrcRect,
						RECT drawRect,int texPool = -1);

	virtual void Draw(int Basex = 0,int Basey = 0);
	virtual void Update();

	void SetDrawRect(RECT drawRect);
	RECT GetDrawRect();

	virtual void SetColor(int a,int r,int g,int b);
};


#define BTNSTATE_NORMAL 0
#define BTNSTATE_HIGHLIGHT 1
#define BTNSTATE_DOWN 2
#define BTNSTATE_DISABLE 3

class NormalButton : public GUIControl
{
private:
	int btnState;
	string text;
	int _a,_r,_g,_b;
	UserFont font;
	RECT ftDrawRect;
public:
	GUIStaticImage *normal,*highLight,*down,*disable;
	NormalButton(RECT pos, string texNormal, RECT srcRectNormal, string texHighLight, RECT srcRectHighLight,
		string texDown, RECT srcRectDown, string texDisable, RECT srcRectDisable,int texPool = -1);

	NormalButton(RECT pos, string texNormal, RECT srcRectNormal, RECT drawRectNormal, 
							string texHighLight, RECT srcRectHighLight,RECT drawRectHighLight,
							string texDown,RECT srcRectDown,RECT drawRectDown,
							string texDisable,RECT srcRectDisable,RECT drawRectDisable,int texPool = -1);

	~NormalButton();

	virtual void Draw(int Basex = 0,int Basey = 0);
	virtual void Update();

	virtual void SetColor(int a,int r,int g,int b);
	virtual void SetButtonState(int _state);
	virtual void SetModifyRect(RECT _modifyRect);
	virtual void SetText(string _text,UserFont _font,RECT _ftDrawRect);

	int GetButtonState();

	virtual void Focus();
	virtual void LostFocus();

	//Handle
	void (*p_ButtonClick)(GUIControl* sender);
	void (*p_ButtonStateChanged)(GUIControl* sender);
};

class NormalButtonGroup : public GUIControl
{
private:
	D3DCOLOR color;
public:
	NormalButtonGroup(RECT pos);

	virtual void Clear();

	virtual void OnKeyEvent(unsigned int nChar,bool bKeyDown,bool bAltDown);
	virtual void SetFocusOn(int id);

	virtual void AddButton(NormalButton* src);
	NormalButton* GetButton(int id);

	int GetSelectedIndex() {return focus;}
	int GetButtonNum(){return panel.size();}
	
	//Handle
	void (*p_OnButtonClick)(int id);
	void (*p_OnSelectedIndexChanged)(int id);
	void (*p_EscClick)();
	void (*p_OnLeftClick)(int id);
	void (*p_OnRightClick)(int id);
};


class ListBox
{
private:
	string _noItem;
	int btnsYDes;
	GUIStaticImage *highLightExtra,*title;
	NormalButton *up,*down;
	bool sleep;
public:
	RECT _drawRect;
	string titleWords;
	NormalButtonGroup *btns;
	int _getin,_getout;
	
	ListBox(RECT drawRect,string noItem);
	~ListBox();

	void Draw();
	void Update();

	void GetIn();
	void GetOut();

	void (*GetInOver)();
	void (*GetOutOver)();

	void ReplaceItem(int tfocus = 0,bool force = 0);

	void BtnSleep();
	void BtnWakeUp();

	void AddItem(string flag,int style = 0);

	void OnKeyEvent(unsigned int nChar,bool bKeyDown,bool bAltDown);
};


class SplashScreen
{
private:
	GUIStaticImage *_logo;
	GUIStaticImage *_nowLoading;
	int _maxDelay,_delayBuffer;
	

public:
	bool _begin;
	bool isloading;

	SplashScreen(GUIStaticImage *logo,int delay);
	~SplashScreen();
	void Begin();
	void End();

	void Draw();
	void Update();

	void (*InitOver)();
	void (*SplashOut)();
};

#define MAINST_QUIT -1
#define MAINST_MAIN 0
#define MAINST_FREE 1
#define MAINST_CONFIG 2
#define MAINST_SELECTSONG 3
#define MAINST_SCORE 4
#define MAINST_CG 5

#define SONGRESULT_MISSTAKE 1
#define SONGRESULT_CHEAP 2
#define SONGRESULT_STANDARD 3
#define SONGRESULT_GREAT 4
#define SONGRESULT_PERFECT 5

class SongInfo
{
public:
	vector<string> songFNames;
	string songFNameBase;

	void GetInfoFromFile();
	void SaveInfoToFile();
	void GetResult(int score,int combo,int result,int whichHard,bool clear);

	string overviewPic;
	string songName;
	double BPM;
	int hard[5];
	string hardText[5];
	string songFName[5];
	int highScore[5];
	int maxCombo[5];
	int highResult[5];
	int clearNum[5];

	string noter;
	string musician;
};

class SongInfoManager
{
public:
	void ReReadInfo();

	vector<SongInfo> songs;
};

const string HardType[5] = {"Easy","Normal","Hard","Extra","Die"};
const string SongResult[6] = {"N/A","Misstake","Cheap","Standard","Great","Perfect"};

class SelectSong
{
	GUIStaticImage *highLightExtra;
	SongInfo *nowSong;
	bool sleep;
public:
	SelectSong();
	~SelectSong();

	void ResetButton(SongInfo &songInfo);
	void OnKeyEvent(unsigned int nChar,bool bKeyDown,bool bAltDown);

	void Draw();
	void Update();

	void UnsetHighLight();
	void ResetHighLight();

	int *alphaHook;

	int playMode;

	NormalButtonGroup *btns;
	
};

#define SCOREMENUST_NUM 0
#define SCOREMENUST_OVER 1

class ScoreMenu
{
	GUIStaticImage *up,*down,*scoreBack,*resultWord,*clear,*notClear,*clearBack,*highLightExtra;
	int coolcntN,finecntN,safecntN,sadcntN,worstcntN,combocntN,chancetimecntN,scorecntN;
	string _songInfo;

	int menust;
public:
	ScoreMenu();
	~ScoreMenu();

	void OnKeyEvent(unsigned int nChar,bool bKeyDown,bool bAltDown);

	void Draw();
	void Update();

	NormalButtonGroup *btns;
	int result;

	int coolcnt,finecnt,safecnt,sadcnt,worstcnt,combocnt,chancetimecnt,scorecnt;
	bool ifClear;

	void ResetResult(int coolCnt, int fineCnt, int safeCnt, int sadCnt, int worstCnt, int comboCnt, int chancetimeCnt, int scoreCnt, bool ifclear,int maxScoreCanGet,string songInfo);
};

class MainMenuBack_Scroll
{
	int breakPoint;
	int _delay,_delayBuffer,_delta;
public:
	int _a;
	RECT _drawRect;
	GUIStaticImage *p1,*p2;
	MainMenuBack_Scroll(int delay,int delta,RECT drawRect);
	~MainMenuBack_Scroll();
	
	void Draw();
	void Update();
};

class MainMenuBack_ScrollHoriz
{
	int breakPoint;
public:
	GUIStaticImage *p1,*p2;
	MainMenuBack_ScrollHoriz();
	~MainMenuBack_ScrollHoriz();

	void Draw();
	void Update();
};

class MainMenuBack
{
	GUIStaticImage *circleOut,*circleOutout,*backColor,*rightLine,*downLine;
	GUIStaticImage *vertLine1,*vertLine2,*vertLine3,*vertLine4;

	//RightBottom
	GUIStaticImage *block1,*block2;
	//LeftUp
	GUIStaticImage *block3,*block4,*block5;
	//MidUp
	GUIStaticImage *block6;
	//Mid
	GUIStaticImage *block7;

	GUIStaticImage *flyUp;

	MainMenuBack_Scroll *rightScroll,*flyScroll;
	MainMenuBack_ScrollHoriz *downScroll;

	int _downLinedelay,_block4AllDelay,_block5AllDelay;
public:
	MainMenuBack();
	~MainMenuBack();

	void Draw();
	void Update();
};

class BottomBar
{
	GUIStaticImage *bottom1,*bottom2;
	GUIStaticImage *labelMove,*labelSelect,*labelPV,*labelDisplay;
	int MainMenuStateGoTo,bottom2L;
	int BarAlphaState;bool Cango;

	void _UnRegisterAll();
	void _ReleaseAllEffect();
	void _SetAllAlpha(int _a);

public:
	BottomBar();
	~BottomBar();

	void MainMenuStateChanged(int MainMenuState);
	void MainMenuStateChangedEnd();

	void Draw();
	void Update();
};

#define _coreWidth 410
#define _coreHeight 160

#define _picWidth 60
#define _picHeight 34

class CGFileInfo
{
	CGFileInfo(){ReloadCGFileInfo();}
public:
	static CGFileInfo& GetInstance() {static CGFileInfo instance;return instance;}
	vector<string> _cgFilename;
	vector<string> _illusion;
	void ReloadCGFileInfo();
};

#define cgFileInfo CGFileInfo::GetInstance()

class CGMenu
{
	vector<GUIStaticImage*> _cg;

	GUIStaticImage *_cgBack;
	GUIStaticImage *_cgShow;
	GUIStaticImage *_lArrow,*_rArrow,*_title;
	GUIVertStretchBlock *_pageNumBack,*_illusionBack;
	

	int _alphaState;
	
	static const int _PoolUse = 3;

	static const int _coreX = (480-_coreWidth)/2;
	static const int _coreY = (272-_coreHeight)/2;

	
	static const int _picNumHoriz = _coreWidth / _picWidth;
	static const int _picNumVert = _coreHeight / _picHeight;
	static const int _deltaWidth = (_coreWidth % _picWidth) / (_picNumHoriz-1);
	static const int _deltaHeight = (_coreHeight % _picHeight) / (_picNumVert-1);
	static const int _picNumPerPage = _picNumHoriz * _picNumVert;
	

	int nowpage,lastpage;

	void PageChanged(int direc);
	void PageChangedOver();

	RECT CalcSelectRect();

public:

	void (*OnEscClick)();
	CGMenu();
	~CGMenu();

	int nowSelected;

	void PrepareCG();

	void EnterCGMenu();
	void ReleaseCG();

	

	void OnKeyEvent(unsigned int nChar,bool bKeyDown,bool bAltDown);

	void Draw();
	void Update();
};

class GUIMessageBox
{
private:
	vector<string> _option;
	string _basicInfo;
	int _alphaState;
	RECT drawRect, showRect;
	int _drawWidth;
	GUIStaticImage *_leftup,*_rightup,*_leftdown,*_rightdown,*_up,*_left,*_down,*_right,*_mid;
	GUIVertStretchBlock *_highLightBack,*_highLight;

	int prepareState;
	

	void _calcImageRect();
	void _calcHighLight();
public:

	int fontSize;
	int lineSepSize;
	int selectedIndex;

	GUIMessageBox(int texturePool = -1);
	~GUIMessageBox();
	void (*p_OnMenuSelectBegin)(int id);
	void (*p_OnMenuSelectOver)(int id);
	void (*p_OnSelectedIndexChanged)(int id);
	void (*p_OnMenuInitOver)();
	void (*p_OnMenuReleaseOver)();

	void PrepareMenu(string basicInfo, int drawWidth);

	void AddItem(string option);

	void ShowMenu();
	void ShowMenu(int x,int y);

	void OnKeyEvent(unsigned int nChar,bool bKeyDown,bool bAltDown);
	
	void Draw();
	void Update();
};

class MainMenu
{	
	int lastSongSelect;
public:
	//Common
	GUIStaticImage *top1,*top2,*title;
	BottomBar *bottom;
	MainMenuBack *back;
	//Main
	GUIStaticImage *mainDetail,*labelMove,*labelSelect;
	
	//SelectSong
	//...
	GUIStaticImage *songPicBack,*blackBack,*songOverviewPic;
	SelectSong *selectSong;

	//CGMenu
	CGMenu *cgmenu;

	//Score
	ScoreMenu *scoreMenu;


	ListBox *mainlb;
	ListBox *songlb;
	ListBox *configlb;
	SongInfoManager songInfo;
	int MainStGoto,MainMenuState;
	string mainDetailWords,configDetailWords;

	MainMenu();
	~MainMenu();

	void Draw();
	void Update();

	void InitSongLb();

	void OnKeyEvent(unsigned int nChar,bool bKeyDown,bool bAltDown);

	
};




#endif