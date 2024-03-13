#ifndef GAMEINI_H
#define GAMEINI_H

#include "base.h"
#include "GraphEngine.h"
#include "SoundEngine.h"

//#define gameini GameIni::Instance()

class GameIni
{
public:
	GameIni(void) {}
	~GameIni(void) {}
	double delay,playstate_delay,uistate_delay;
	static const int note_standing = 192;
	static const int ORIGIN_X = 25, ORIGIN_Y = 52;
	static const int DELTA_X = 12, DELTA_Y = 12;
	static const int DISTANCE = 500;
	static const int BPM = 120;
	double COOL, FINE, SAFE;
	static const int SCORE_COOL = 500, SCORE_FINE = 300, SCORE_SAFE = 100;
	static const int HP = 90;
	static const int SINGLE_HP = 6;
	static const int HP_DEC = 3, HP_ADD = 1;
	static const double NOTE_BLOWUP;
	double eps;

	void Init();

	//static GameIni& Instance(void) {static GameIni instance; return instance;}
};
extern GameIni gameini;

extern void test();
#endif