#include "UIScreen.h"
#include "Base.h"
#include "Gui.h"
#include <iostream>
using namespace base;
using namespace std;

SplashScreen *logo1,*logo2,*logo3,*loading;
GameCore core;
bool Keys[256] = {0};
int ScreenState;
void (*LoadingToDo)() = NULL;
void (*LoadingOver)() = NULL;

MainMenu *menu;

void loading_InitOver()
{
	ScreenState = SCREEN_LOADING;
	if(LoadingToDo)
		LoadingToDo();
	loading->End();
}

void loading_SplashOut()
{
	if(LoadingOver)
		LoadingOver();
}


void beginSongInit()
{
	core.SetNew(menu->songInfo.songs[menu->songlb->btns->GetSelectedIndex()].songFName[menu->selectSong->btns->GetSelectedIndex()],
		menu->songInfo.songs[menu->songlb->btns->GetSelectedIndex()].songFNameBase,GAME_MODE_NORMAL);
}

void beginSongOver()
{
	ScreenState = SCREEN_GAMEMAIN;
}


void logo1_SplashOut()
{
	ScreenState = SCREEN_LOGO2;
	logo2->Begin();
}

void logo2_SplashOut()
{
	ScreenState = SCREEN_LOGO3;
	logo3->Begin();
}


void menu_mainlb_OnButtonClick(int id)
{
	if(id==0)
	{
		menu->MainStGoto = MAINST_FREE;
		menu->bottom->MainMenuStateChanged( MAINST_FREE );
		menu->mainDetail->efvct.ReleaseEffects();
		menu->mainDetail->efvct.AddEffect(new GUIEF_Alpha(menu->mainDetail->efvct.a,25,0));
		menu->mainlb->GetOut();
	}
	else if(id==3)
	{
		menu->MainStGoto = MAINST_QUIT;
		menu->bottom->MainMenuStateChanged( MAINST_QUIT );
		menu->mainDetail->efvct.ReleaseEffects();
		menu->mainDetail->efvct.AddEffect(new GUIEF_Alpha(menu->mainDetail->efvct.a,25,0));
		menu->mainlb->GetOut();
	}
}

void menu_mainlb_OnSelectedIndexChanged(int id)
{
	if(id==0)
		menu->mainDetailWords = "可以\n演奏节奏游戏";
	else if(id==1)
		menu->mainDetailWords = "变更BGM和SE的音量";
	else if(id==2)
		menu->mainDetailWords = "可以鉴赏\n游戏的插画和影像";
	else if(id==3)
		menu->mainDetailWords = "结束游戏";
}
void menu_mainlb_GetInOver()
{
	menu->mainDetail->efvct.ReleaseEffects();
	menu->mainDetail->efvct.AddEffect(new GUIEF_Alpha(menu->mainDetail->efvct.a,25,255));
}
void menu_songlb_GetOutOver()
{
	LoadingToDo = beginSongInit;
	LoadingOver = beginSongOver;
	loading->Begin();
}

void menu_songlb_GetInOver()
{
	menu->songPicBack->efvct.ReleaseEffects();
	menu->blackBack->efvct.ReleaseEffects();
	menu->songPicBack->efvct.AddEffect(new GUIEF_Alpha(menu->songPicBack->efvct.a,25,255));
	menu->blackBack->efvct.AddEffect(new GUIEF_Alpha(menu->blackBack->efvct.a,25,255));
}
void menu_songlb_OnButtonClick(int id)
{
	if(id!=-1)
	{
		menu->songlb->BtnSleep();
		menu->selectSong->ResetButton(menu->songInfo.songs[id]);
		menu->MainMenuState = MAINST_SELECTSONG;
	}
}
void menu_selectSong_OnButtonClick(int id)
{
	if(id!=-1)
	{
		menu->songlb->BtnWakeUp();
		menu->songlb->GetOutOver = menu_songlb_GetOutOver;
		menu->songlb->GetOut();
		menu->songPicBack->efvct.ReleaseEffects();
		menu->blackBack->efvct.ReleaseEffects();
		menu->songPicBack->efvct.AddEffect(new GUIEF_Alpha(menu->songPicBack->efvct.a,25,0));
		menu->blackBack->efvct.AddEffect(new GUIEF_Alpha(menu->blackBack->efvct.a,25,0));
		menu->selectSong->UnsetHighLight();
	}
}
void menu_selectSong_OnEscClick()
{
	menu->songlb->BtnWakeUp();
	menu->MainMenuState = MAINST_FREE;
}

void menu_ListBoxGetOutOver()
{
	ScreenState = SCREEN_MAIN;
	menu->MainMenuState = menu->MainStGoto;
	menu->bottom->MainMenuStateChangedEnd();
	if(menu->MainStGoto==MAINST_FREE)
		menu->songlb->GetIn();
	else if(menu->MainStGoto==MAINST_MAIN)
		menu->mainlb->GetIn();
	else if(menu->MainStGoto==MAINST_QUIT)
		PostMessage(base::hwnd,WM_QUIT,0,0);
}

void menu_songlb_EscClick()
{
	menu->songlb->GetOutOver = menu_ListBoxGetOutOver;
	menu->MainStGoto = MAINST_MAIN;
	menu->bottom->MainMenuStateChanged( MAINST_MAIN );
	menu->songlb->GetOut();
	menu->songPicBack->efvct.ReleaseEffects();
	menu->blackBack->efvct.ReleaseEffects();
	menu->songPicBack->efvct.AddEffect(new GUIEF_Alpha(menu->songPicBack->efvct.a,25,0));
	menu->blackBack->efvct.AddEffect(new GUIEF_Alpha(menu->blackBack->efvct.a,25,0));
}

void menu_scoreMenu_OnButtonClick(int id)
{
	if(id==0)
	{
		menu_songlb_GetOutOver();
	}
	else if(id==1)
	{
		menu->MainStGoto = MAINST_FREE;
		menu->bottom->MainMenuStateChanged( MAINST_FREE );
		LoadingToDo = menu_ListBoxGetOutOver;
		LoadingOver = NULL;
		loading->Begin();
	}
}

void MainFirstInit()
{
	menu = new MainMenu();
	menu->mainlb->GetOutOver = menu_ListBoxGetOutOver;
	menu->mainlb->GetInOver = menu_mainlb_GetInOver;
	menu->mainlb->btns->p_OnButtonClick = menu_mainlb_OnButtonClick;
	menu->mainlb->btns->p_OnSelectedIndexChanged = menu_mainlb_OnSelectedIndexChanged;
	menu->songlb->btns->p_EscClick = menu_songlb_EscClick;
	menu->songlb->btns->p_OnButtonClick = menu_songlb_OnButtonClick;
	menu->songlb->GetInOver = menu_songlb_GetInOver;
	menu->selectSong->btns->p_OnButtonClick = menu_selectSong_OnButtonClick;
	menu->selectSong->btns->p_EscClick = menu_selectSong_OnEscClick;
	menu->scoreMenu->btns->p_OnButtonClick = menu_scoreMenu_OnButtonClick;
	ScreenState = SCREEN_MAIN;
	menu->bottom->MainMenuStateChanged( MAINST_MAIN );
	menu->bottom->MainMenuStateChangedEnd();
}

void logo3_SplashOut()
{
	ScreenState = SCREEN_LOADING;
	loading->Begin();
	LoadingToDo = MainFirstInit;
	LoadingOver = NULL;
}

void BackToUIScreen(int coolCnt, int fineCnt, int safeCnt, int sadCnt, int worstCnt, int comboCnt, int chancetimeCnt, int scoreCnt, bool ifclear,int maxScoreCanGet)
{
	graphEngine.ReleaseTex(GameCoreRes);
	menu->scoreMenu->ResetResult(coolCnt,fineCnt,safeCnt,sadCnt,worstCnt,comboCnt,chancetimeCnt,scoreCnt,ifclear,maxScoreCanGet);
	menu->songInfo.songs[menu->songlb->btns->GetSelectedIndex()].GetResult(scoreCnt,comboCnt,menu->scoreMenu->result,
				menu->selectSong->btns->GetSelectedIndex(),ifclear);
	ScreenState = SCREEN_MAIN;
	menu->MainMenuState = MAINST_SCORE;
}

void InitUIScreen()
{
	core.Init();

	loading = new SplashScreen(new GUIStaticImage("pic\\load_22.png",MakeRect(16,120,480,272),MakeRect(0,0,480,272)),10000000);
	loading->InitOver = loading_InitOver;
	loading->SplashOut = loading_SplashOut;

	logo1 = new SplashScreen(new GUIStaticImage("pic\\logo_Crypton.png",MakeRect(16,120,480,272),MakeRect(0,0,480,272)),2);
	logo1->Begin();
	ScreenState = SCREEN_LOGO1;
	logo1->SplashOut = logo1_SplashOut;

	logo2 = new SplashScreen(new GUIStaticImage("pic\\logo_dingo.png",MakeRect(16,120,480,272),MakeRect(0,0,480,272)),2);
	logo2->SplashOut = logo2_SplashOut;

	logo3 = new SplashScreen(new GUIStaticImage("pic\\logo_criware.png",MakeRect(16,120,480,272),MakeRect(0,0,480,272)),2);
	logo3->SplashOut = logo3_SplashOut;
}

void GameMainDraw()
{
	if(ScreenState==SCREEN_GAMEMAIN)
		core.Draw(base::Device);
	else if(ScreenState==SCREEN_LOGO1)
		logo1->Draw();
	else if(ScreenState==SCREEN_LOGO2)
		logo2->Draw();
	else if(ScreenState==SCREEN_LOGO3)
		logo3->Draw();
	else if(ScreenState==SCREEN_MAIN)
		menu->Draw();
	loading->Draw();
}

void GameMainUpdate(double dwTime)
{
	if(ScreenState==SCREEN_GAMEMAIN)
		core.Update(dwTime);
	else if(ScreenState==SCREEN_LOGO1)
		logo1->Update();
	else if(ScreenState==SCREEN_LOGO2)
		logo2->Update();
	else if(ScreenState==SCREEN_LOGO3)
		logo3->Update();
	else if(ScreenState==SCREEN_MAIN)
		menu->Update();
	loading->Update();
}

void DeleteUIScreen()
{
	SAFE_DELETE(logo1);
	SAFE_DELETE(logo2);
	SAFE_DELETE(logo3);
	SAFE_DELETE(loading);
	SAFE_DELETE(menu);
}

void GameMainOnMouseEvent(bool bLeftButtonDown, bool bRightButtonDown, bool bMiddleButtonDown, bool bSideButton1Down, 
						   bool bSideButton2Down, int nMouseWheelDelta, int xPos, int yPos)
{
	
}

void GameMainOnKeyEvent(UINT nChar, bool bKeyDown, bool bAltDown)
{
	static char *pBuffer = NULL;

	if(ScreenState==SCREEN_GAMEMAIN)
	{
		if(bKeyDown&&Keys[nChar]==false)
		{
			Keys[nChar] = true;
			core.OnKeyEvent(nChar,bKeyDown,bAltDown);
		}
	}
	else if(ScreenState==SCREEN_MAIN)
	{
		if(bKeyDown)
			Keys[nChar] = true;
		menu->OnKeyEvent(KeyEventValue);
	}
	if(!bKeyDown)
		Keys[nChar] = false;
}

