#ifndef __GuiH__
#define __GuiH__

#include<vector>
#include<string>
#include "Base.h"
#include "GameMana.h"
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


//----------闪烁-------------
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

//---------坐标平移----------
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

//---------矩形拉伸----------
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
	RECT srcRect;
public:
	EffectVector efvct;

	GUIStaticImage(string filename,RECT _srcRect,RECT _drawRect,int texPool = -1);

	virtual void Draw(int Basex = 0,int Basey = 0);
	virtual void Update();

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

	void AddItem(string flag);

	void OnKeyEvent(unsigned int nChar,bool bKeyDown,bool bAltDown);
};


class SplashScreen
{
private:
	GUIStaticImage *_logo;
	int _maxDelay,_delayBuffer;
	bool _begin;
public:
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

const string HardType[5] = {"Easy","Normal","Hard","Extra","Fuck"};
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

	NormalButtonGroup *btns;
	
};

#define SCOREMENUST_NUM 0
#define SCOREMENUST_OVER 1

class ScoreMenu
{
	GUIStaticImage *up,*down,*scoreBack,*resultWord,*clear,*notClear,*clearBack,*highLightExtra;
	int coolcntN,finecntN,safecntN,sadcntN,worstcntN,combocntN,chancetimecntN,scorecntN;

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

	void ResetResult(int coolCnt, int fineCnt, int safeCnt, int sadCnt, int worstCnt, int comboCnt, int chancetimeCnt, int scoreCnt, bool ifclear,int maxScoreCanGet);
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

class CGMenu
{
	vector<GUIStaticImage*> _cg;
	vector<string> _illusion;
	int nowSelected;

public:
	CGMenu();
	~CGMenu();

	void PrepareCG();
	void ReleaseCG();

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
	SongInfoManager songInfo;
	int MainStGoto,MainMenuState;
	string mainDetailWords;

	MainMenu();
	~MainMenu();

	void Draw();
	void Update();

	void InitSongLb();

	void OnKeyEvent(unsigned int nChar,bool bKeyDown,bool bAltDown);

	
};


#endif