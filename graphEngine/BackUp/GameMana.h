#ifndef __GameManaH__
#define __GameManaH__

#include "defines.h"
#include "notemap.h"
#include "GameIni.h"
#include "GraphEngine.h"
#include "VideoEngine.h"

#define GAMEDATA GameData::Instance()

enum PLAYSTATE{SAFE,COOL,FINE,_SAFE,_COOL,_FINE,WORST,NOSTATE};
enum UISTATE{ADD,DEC,NORMAL};

class GameData
{
	static const int TYPENUM = 4;
	RECT pointer_rect;
	RECT note_rect[TYPENUM];
	RECT rhythm_rect[TYPENUM];
	RECT presh_rect[TYPENUM];
	RECT state_rect[7];
	Texture *background, *note_pic;
	friend class Unit;
	friend class NoteMana;
	friend class GameCore;
	friend class GameUI;
	int hp, score, combo, maxScoreCanGet, maxScoreTempCombo, maxScoreTempChanceTimeCombo;
	int cool_cnt,fine_cnt,safe_cnt,worst_cnt,maxcombo;
	int _chanceTimeScore,_chanceTimeCombo; 
	bool chanceTime;
	int gamemode;
	GameData() {}
	~GameData() {}
public:
	static GameData& Instance(void) {static GameData instance; return instance;}
	void Init();
	void SetNew();
	void Release();
	void ModifyHP(int delta);
};

class Unit
{
	int notePos;
	bool preshed,first;
	UNIT type;
	double angle;
	Point note;
	Point rhythm;
	Point bezier_p1,bezier_p2,lastPoint;
	int key;
	double restTime;
	double delayTime;
	double totalTime;
	//double _a,_b,_c;
	friend class NoteMana;
public:
	Unit():type(UNIT_CIRCLE),restTime(0),angle(0),preshed(false) {}
	Unit(Note _note, double standing, int _notePos);
	void Update(double dwTime);
	void Draw();
	void Clear();
	void Presh(PLAYSTATE id);
	Point Bezier(const Point &p0, const Point &p1, const Point &p2, const Point &p3, const double &t);
	void DrawLine(const Point &a, const Point &b);
	void AddParticle(int x, int y);
};

class NoteMana
{
	double singleTime;
	double noteTime;
	double nowTime;
	vector<Unit> unit;
	
public:
	NoteMana() {}
	void SetNew();
	void Clear() {unit.clear();}
	//---------------------------------------------
	void Add(int pos);
	void Run(LpFrame _note);
	void Draw();
	void Update(double dwTime);
	//---------------------------------------------
	void OnKeyEvent(UINT nChar, bool bKeyDown, bool bAltDown);
	//---------------------------------------------
	void Pop() {if(!unit.empty()) unit.erase(unit.begin());}
	int GetSize() {return unit.size();}
	int GetScore() {return GAMEDATA.score;}
};
#define gameui GameUI::Instance()

class GameUI
{
	PLAYSTATE state;
	UISTATE uistate;
	double delayTime, uidelay;
	int spark;
	int moveTime, nowTime;
	int moveState; //0 normal 1 in 2 out
	int deltaTop, deltaBottom;
	Texture *ui;
	RECT top_bar, bottom_bar;
	RECT top_bar_red, bottom_bar_red;
	GameUI() {}
	~GameUI() {}
	friend class GameCore;
public:
	static GameUI& Instance(void) {static GameUI instance; return instance;}
	void Init();
	void Draw();
	void Update(double dwTime);
	void SetNew();
	void SetState(PLAYSTATE _state);
	void SetUIState(UISTATE _state);
	void MoveAway(int time);
	void MoveIn(int time);
};

class GameCore
{
	double singleTime;
	double noteTime;
	double totalTime;
	LpFrame nowFrame;
	NoteMana _notemana;
	bool finish;
	bool paused;
	bool ifclear;
public:
	RESOURCE resource;
	GameCore() {}
	~GameCore();
	void Init();
	void SetNew(string filename,string baseDirec,int gamemode = GAME_MODE_NORMAL);
	//--------------------------------
	void Run(LpFrame _nowFrame);
	void Update(double dwTime);
	void Draw(IDirect3DDevice9 *Device);
	void Finish();
	//--------------------------------
	void OnKeyEvent(UINT nChar, bool bKeyDown, bool bAltDown);
	//--------------------------------
	bool IsFinish() {return (nowFrame==NULL);}
	int GetScore() {return _notemana.GetScore();}
	int GetNoteSize() {return _notemana.GetSize();}
	int GetCombo() {return GAMEDATA.combo;}
};

#endif