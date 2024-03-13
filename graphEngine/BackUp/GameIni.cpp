#include "GameIni.h"

const double GameIni::NOTE_BLOWUP = 1.8;

void GameIni::Init()
{
	playstate_delay = base::SECOND*0.5;
	uistate_delay = base::SECOND*0.5;
	delay = base::SECOND*0.2;
	COOL = base::SECOND*0.03;
	FINE = base::SECOND*0.08;
	SAFE = base::SECOND*0.13;
	eps = base::SECOND*0.02;
}