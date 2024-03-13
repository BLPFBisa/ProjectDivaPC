#ifndef __UIScreenH__
#define __UIScreenH__

#include "base.h"
#include "Gui.h"
#include "GameMana.h"

using namespace base;

#define SCREEN_LOGO1 0
#define SCREEN_LOGO2 1
#define SCREEN_LOGO3 2
#define SCREEN_MAIN 3
#define SCREEN_GAMEMAIN 4
#define SCREEN_LOADING 5
#define SCREEN_LOGO4 6

void InitUIScreen();
void GameMainDraw();

void GameMainUpdate(double dwTime);

void DeleteUIScreen();
void GameMainOnMouseEvent(bool bLeftButtonDown, bool bRightButtonDown, bool bMiddleButtonDown, bool bSideButton1Down, 
						   bool bSideButton2Down, int nMouseWheelDelta, int xPos, int yPos);

void GameMainOnKeyEvent(UINT nChar, bool bKeyDown, bool bAltDown);

void BackToUIScreen(int coolCnt, int fineCnt, int safeCnt, int sadCnt, int worstCnt, int comboCnt, int chancetimeCnt, int scoreCnt, bool ifclear,int maxScoreCanGet);

#endif