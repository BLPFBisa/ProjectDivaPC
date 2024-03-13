#ifndef __EffectMana__
#define __EffectMana__

#include<vector>
#include"Base.h"
#include"GameMana.h"
#include"PSystem.h"
using namespace std;

class Effect_Base
{
protected:
	int nowframe,desframe;
	int _x,_y;
	Texture* _drawTex;
public:
	Effect_Base(int _nowframe,int _desframe,int x,int y, Texture* drawTex):nowframe(_nowframe),desframe(_desframe),_x(x),_y(y),_drawTex(drawTex){}

	virtual void Draw(){}
	virtual void Update(){}
	virtual bool Die(){return nowframe==desframe;}
};

class Effect_NoteIn : public Effect_Base
{
public:
	Effect_NoteIn(int x,int y,Texture* drawTex):Effect_Base(0,30,x,y,drawTex){}

	void Draw(){};
	void Update(){};
};

class Effect_NotePressSafe : public Effect_Base
{
private:
	int delta;
public:
	Effect_NotePressSafe(int x,int y,Texture* drawTex):Effect_Base(0,60,x,y,drawTex),delta(0){}

	void Draw();
	void Update();
};

class Effect_NotePressFine : public Effect_Base
{
public:
	Effect_NotePressFine(int x,int y,Texture* drawTex):Effect_Base(0,16,x,y,drawTex){}

	void Draw();
	void Update();
};

class Effect_NotePressFlame : public Effect_Base
{
public:
	Effect_NotePressFlame(int x,int y,Texture* drawTex):Effect_Base(0,60,x,y,drawTex){}

	void Draw();
	void Update();
};

class Effect_NotePressCool : public Effect_Base
{
public:
	Effect_NotePressCool(int x,int y,Texture* drawTex):Effect_Base(0,24,x,y,drawTex){}

	void Draw();
	void Update();
};

class Effect_Combo : public Effect_Base
{
	int _combo;
public:
	Effect_Combo(int x,int y,int combo,Texture* drawTex):Effect_Base(0,40,x,y,drawTex),_combo(combo){}

	void Draw();
	void Update();
};

class Effect_ExtraScore : public Effect_Base
{
	int _extraScore;
public:
	Effect_ExtraScore(int x,int y,int extraScore,Texture* drawTex):Effect_Base(0,40,x,y,drawTex),_extraScore(extraScore){}

	void Draw();
	void Update();
};

class Effect_ChanceTimeEnd : public Effect_Base
{
	int _chanceTimeScore;
public:
	Effect_ChanceTimeEnd(int chanceTimeScore,Texture* drawTex):Effect_Base(0,180,0,0,drawTex),_chanceTimeScore(chanceTimeScore){}

	void Draw();
	void Update();
};

class Effect_ChanceTimeStart : public Effect_Base
{
public:
	Effect_ChanceTimeStart(Texture* drawTex):Effect_Base(0,180,0,0,drawTex){}

	void Draw();
	void Update();
};

class Effect_Flame : public Effect_Base
{
	bool Updated;
public:
	Effect_Flame(Texture* drawTex):Effect_Base(0,-1,0,0,drawTex){Updated = true;}

	void SetPos(int x,int y);
	void Reset();

	void Draw();
	void Update();
};

enum NumberStyle{
	SCORE,
	COMBO,
	EXTRASCORE,
	CHANCE
};

enum NumberDwFlags{
	LEFT,
	RIGHT,
	MID
};


class NumberMana
{
	NumberMana(){numTex = graphEngine.AddTexture("pic\\num.png"); comboDraw = NULL; extraScoreDraw = NULL;}

	Texture *numTex;
	Effect_Combo *comboDraw;
	Effect_ExtraScore *extraScoreDraw;
public:
	static NumberMana& Instance() {static NumberMana instance;return instance;}
	void Clear();

	void Draw(int number,RECT drawRect,NumberStyle style,NumberDwFlags flag,D3DCOLOR color,int inc,int fill=0);
	
	void DrawScore(int score);
	void DrawCombo();
	
	void Update();

	void SetCombo(int combo,int x,int y);
	void SetExtraScore(int extraScore,int x,int y);
};

#define numberMana NumberMana::Instance()

class EffectMana
{
	EffectMana();
	~EffectMana(){Clear();}

	vector<Effect_Base*> effects;
	Texture *drawTex,*coolTex,*flameTex,*numTex;
	
	static const int MaxFlameEF = 4;
	Effect_Flame* flameEF[MaxFlameEF];

public:
	static EffectMana& Instance() {static EffectMana instance;return instance;}
	
	void SetNew();

	void Clear();

	void AddEffectNoteIn(int x,int y){effects.push_back((Effect_Base*)(new Effect_NoteIn(x,y,drawTex)));}
	void AddEffectNotePress(int x,int y,PLAYSTATE id);
	void AddEffectChanceTimeStart(){effects.push_back((Effect_Base*)(new Effect_ChanceTimeStart(numTex)));}
	void AddEffectChanceTimeEnd(int chanceTimeScore){effects.push_back((Effect_Base*)(new Effect_ChanceTimeEnd(chanceTimeScore,numTex)));}
	void AddEffectUpdateFlame(int x,int y,int noteType);
	void AddEffectResetFlame(int noteType);

	void Draw();

	void DrawCombo(int combo);

	void Update();

};

#define effectMana EffectMana::Instance()

#endif