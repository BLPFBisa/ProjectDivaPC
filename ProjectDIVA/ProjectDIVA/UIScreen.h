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

void GameMainOnMouseEvent(int xPos, int yPos, UINT msg, WPARAM wParam);
void GameMainOnKeyEvent(UINT nChar, bool bKeyDown);

void DeleteUIScreen();
void BackToUIScreen(int coolCnt, int fineCnt, int safeCnt, int sadCnt, int worstCnt, int comboCnt, int chancetimeCnt, int scoreCnt, bool ifclear,int maxScoreCanGet);

void menu_configlb_RefreshDetail();

#endif