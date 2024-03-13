#include "UIScreen.h"
#include "Base.h"
#include "Gui.h"
#include <iostream>
using namespace base;
using namespace std;

SplashScreen *logo1,*logo2,*logo3,*logo4,*loading;

GameCore core;
GUIMessageBox *pausedmenu;

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
		menu->songInfo.songs[menu->songlb->btns->GetSelectedIndex()].songFNameBase,menu->selectSong->playMode);
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
void logo3_SplashOut()
{
	ScreenState = SCREEN_LOGO4;
	logo4->Begin();
	soundEngine.PlaySound("gamedata\\SEGA.wav");
}

void menu_configlb_RefreshDetail()
{
	string KeyType;
	if(systemIni.GameKeyType==2)
		KeyType="PSP";
	else if(systemIni.GameKeyType==3)
		KeyType="o2jam-PC";
	else if(systemIni.GameKeyType==4)
		KeyType="o2jam-PSP";
	else
		KeyType="PC";
		
	menu->configlb->btns->GetButton(0)->SetText("按键习惯：" + KeyType,UserFont("宋体",12,false,FW_BOLD),MakeRect(16,8,0,0));
	menu->configlb->btns->GetButton(1)->SetText("窗口大小：" + ToString(systemIni.winWidth) + "*" + ToString(systemIni.winHeight),UserFont("宋体",12,false,FW_BOLD),MakeRect(16,8,0,0));
	menu->configlb->btns->GetButton(2)->SetText("音效音量：      " + ToString(systemIni.soundVolume) + "/10",UserFont("宋体",12,false,FW_BOLD),MakeRect(16,8,0,0));

}

void menu_mainlb_OnButtonClick(int id)
{
	if(id==0)
	{
		if(menu->songlb->btns->GetButtonNum()>0)
		{
			menu->MainStGoto = MAINST_FREE;
			menu->bottom->MainMenuStateChanged( MAINST_FREE );
			menu->mainDetail->efvct.ReleaseEffects();
			menu->mainDetail->efvct.AddEffect(new GUIEF_Alpha(menu->mainDetail->efvct.a,25,0));
			menu->mainlb->GetOut();
			soundEngine.StopMusic(0,true);
			soundEngine.PlaySound("gamedata\\Select.wav",systemIni.sndVolume);
			soundEngine.PlaySound("gamedata\\Sweep.wav",systemIni.sndVolume);
		}
	}
	else if(id==3)
	{
		menu_configlb_RefreshDetail();
		menu->MainStGoto = MAINST_CONFIG;
		menu->mainDetail->efvct.ReleaseEffects();
		menu->mainDetail->efvct.AddEffect(new GUIEF_Alpha(menu->mainDetail->efvct.a,25,0));
		menu->mainlb->GetOut();
		soundEngine.PlaySound("gamedata\\Select.wav",systemIni.sndVolume);
		soundEngine.PlaySound("gamedata\\Sweep.wav",systemIni.sndVolume);
	}
	else if(id==4)
	{
		menu->MainStGoto = MAINST_CG;
		menu->bottom->MainMenuStateChanged( MAINST_CG );
		menu->mainDetail->efvct.ReleaseEffects();
		menu->mainDetail->efvct.AddEffect(new GUIEF_Alpha(menu->mainDetail->efvct.a,25,0));
		menu->mainlb->GetOut();
		soundEngine.StopMusic(0,true);
		soundEngine.PlaySound("gamedata\\Select.wav",systemIni.sndVolume);
		soundEngine.PlaySound("gamedata\\Sweep.wav",systemIni.sndVolume);
	}
	else if(id==6)
	{
		menu->MainStGoto = MAINST_QUIT;
		menu->bottom->MainMenuStateChanged( MAINST_QUIT );
		menu->mainDetail->efvct.ReleaseEffects();
		menu->mainDetail->efvct.AddEffect(new GUIEF_Alpha(menu->mainDetail->efvct.a,25,0));
		menu->mainlb->GetOut();
		soundEngine.StopMusic(0,true);
		soundEngine.PlaySound("gamedata\\Select.wav",systemIni.sndVolume);
		soundEngine.PlaySound("gamedata\\Sweep.wav",systemIni.sndVolume);
	}
	
}

void menu_mainlb_OnSelectedIndexChanged(int id)
{
	if(id==0)
	{
		if(menu->songlb->btns->GetButtonNum()<=0)
			menu->mainDetailWords = "没有检测到\n演奏节奏游戏\n无法进入";
		else
			menu->mainDetailWords = "可以\n演奏节奏游戏";
	}
	else if(id==1)
		menu->mainDetailWords = "开始ミク的歌姬生涯\n与ミク一起完成梦想\n目前版本未开放";
	else if(id==2)
		menu->mainDetailWords = "可以\n和网络上的朋友对战\n目前版本未开放";
	else if(id==3)
		menu->mainDetailWords = "变更按键设置和窗口大小\n以及音效的音量";
	else if(id==4)
		menu->mainDetailWords = "可以鉴赏\n游戏的插画和影像";
	else if(id==5)
		menu->mainDetailWords = "GameMaster.\"Project Z1\"\n《Project DIVA PC》\n非原创素材版权归SEGA所有";
	else if(id==6)
		menu->mainDetailWords = "结束游戏";
	soundEngine.PlaySound("gamedata\\Button.wav",systemIni.sndVolume);
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
void menu_songlb_OnSelectedIndexChanged(int id)
{
	soundEngine.PlaySound("gamedata\\Button.wav",systemIni.sndVolume);
}

void menu_songlb_GetInOver()
{
	menu->songPicBack->efvct.ReleaseEffects();
	menu->blackBack->efvct.ReleaseEffects();
	menu->songPicBack->efvct.AddEffect(new GUIEF_Alpha(menu->songPicBack->efvct.a,25,255));
	menu->blackBack->efvct.AddEffect(new GUIEF_Alpha(menu->blackBack->efvct.a,25,255));
}

bool songLbButtonFirstClick=false;
void menu_songlb_OnButtonClick(int id)
{
	if(id>=0)
	{
		songLbButtonFirstClick=true;
		menu->songlb->BtnSleep();
		menu->selectSong->ResetButton(menu->songInfo.songs[id]);
		menu->MainMenuState = MAINST_SELECTSONG;
		soundEngine.PlaySound("gamedata\\Select.wav",systemIni.sndVolume);
		songLbButtonFirstClick=false;
		menu->bottom->MainMenuStateChanged( MAINST_SELECTSONG );
		menu->bottom->MainMenuStateChangedEnd();
	}
}
void menu_selectSong_OnButtonClick(int id)
{
	if(id!=-3)
	{
		soundEngine.PlaySound("gamedata\\Select.wav",systemIni.sndVolume);
		soundEngine.PlaySound("gamedata\\Fly.wav",systemIni.sndVolume);
		if(id==-2)
			menu->selectSong->playMode = GAME_MODE_AUTO;
		else if(id==-1)
			menu->selectSong->playMode = GAME_MODE_PV;
		else
			menu->selectSong->playMode = GAME_MODE_NORMAL;
		menu->songlb->BtnWakeUp();
		menu->songlb->GetOutOver = menu_songlb_GetOutOver;
		menu->songlb->GetOut();
		menu->songPicBack->efvct.ReleaseEffects();
		menu->blackBack->efvct.ReleaseEffects();
		menu->songPicBack->efvct.AddEffect(new GUIEF_Alpha(menu->songPicBack->efvct.a,25,0));
		menu->blackBack->efvct.AddEffect(new GUIEF_Alpha(menu->blackBack->efvct.a,25,0));
		menu->selectSong->UnsetHighLight();
		soundEngine.StopMusic(0,true);
	}
}
void menu_selectSong_OnEscClick()
{
	menu->songlb->BtnWakeUp();
	menu->MainMenuState = MAINST_FREE;
	menu->bottom->MainMenuStateChanged( MAINST_FREE );
	menu->bottom->MainMenuStateChangedEnd();

}
void menu_selectSong_OnSelectedIndexChanged(int id)
{
	if(!songLbButtonFirstClick)
		soundEngine.PlaySound("gamedata\\Button.wav",systemIni.sndVolume);
}

void RefreshConfigDetail()
{
	if(menu->configlb->btns->GetSelectedIndex()==0)
	{
		if(systemIni.GameKeyType==2)
			menu->configDetailWords = "○：→　→：Ｄ\nⅩ：↓　↓：Ｓ\n□：←　←：Ａ\n△：↑　↑：Ｗ";
		else if(systemIni.GameKeyType==3)
			menu->configDetailWords = "○：Ｆ　→：Ｌ\nⅩ：Ｄ　↓：Ｋ\n□：Ｓ　←：Ｊ\n△：Ｅ　↑：Ｉ";
		else if(systemIni.GameKeyType==4)
			menu->configDetailWords = "○：Ｌ　→：Ｆ\nⅩ：Ｋ　↓：Ｄ\n□：Ｊ　←：Ｓ\n△：Ｉ　↑：Ｅ";
		else
			menu->configDetailWords = "○：Ｄ　→：→\nⅩ：Ｓ　↓：↓\n□：Ａ　←：←\n△：Ｗ　↑：↑";
	}
	else if(menu->configlb->btns->GetSelectedIndex()==1)
	{
		menu->configDetailWords = "可以调整的窗口大小：\n480*272\n720*408\n960*544";
	}
	else if(menu->configlb->btns->GetSelectedIndex()==2)
	{
		menu->configDetailWords = "\n可以更改\n音效、按键音的音量";
	}
	else if(menu->configlb->btns->GetSelectedIndex()==3)
	{
		menu->configDetailWords = "\n\n可以恢复默认设置";
	}
}
void menu_configlb_OnLeftClick(int id)
{
	if(id==2)
	{
		systemIni.soundVolume--;
		if(systemIni.soundVolume<0)
			systemIni.soundVolume=10;
		systemIni.ChangeSndVolume(systemIni.soundVolume);
		menu_configlb_RefreshDetail();
		soundEngine.PlaySound("gamedata\\Select.wav",systemIni.sndVolume);
	}
}
void menu_configlb_OnRightClick(int id)
{
	if(id==2)
	{
		systemIni.soundVolume++;
		if(systemIni.soundVolume>10)
			systemIni.soundVolume=0;
		systemIni.ChangeSndVolume(systemIni.soundVolume);
		menu_configlb_RefreshDetail();
		soundEngine.PlaySound("gamedata\\Select.wav",systemIni.sndVolume);
	}
}
void menu_configlb_OnButtonClick(int id)
{
	if(id==0)
	{
		systemIni.GameKeyType++;
		if(systemIni.GameKeyType>=5)
			systemIni.GameKeyType=1;
		systemIni.ChangeGameKeyType(systemIni.GameKeyType);
		menu_configlb_RefreshDetail();
		soundEngine.PlaySound("gamedata\\Select.wav",systemIni.sndVolume);
		RefreshConfigDetail();
	}
	else if(id==1)
	{
		systemIni.winSizeType++;
		if(systemIni.winSizeType>=4)
			systemIni.winSizeType=1;
		systemIni.ChangeWindowSize(systemIni.winSizeType);
		base::SetWindowSize(systemIni.winWidth,systemIni.winHeight);
		menu_configlb_RefreshDetail();
		soundEngine.PlaySound("gamedata\\Select.wav",systemIni.sndVolume);
	}
	else if(id==2)
	{
		systemIni.soundVolume++;
		if(systemIni.soundVolume>10)
			systemIni.soundVolume=0;
		systemIni.ChangeSndVolume(systemIni.soundVolume);
		menu_configlb_RefreshDetail();
		soundEngine.PlaySound("gamedata\\Select.wav",systemIni.sndVolume);
	}
	else if(id==3)
	{
		systemIni.ChangeToDefault();
		base::SetWindowSize(systemIni.winWidth,systemIni.winHeight);
		menu_configlb_RefreshDetail();
		soundEngine.PlaySound("gamedata\\Select.wav",systemIni.sndVolume);
	}
}
void menu_configlb_OnSelectedIndexChanged(int id)
{
	RefreshConfigDetail();
	soundEngine.PlaySound("gamedata\\Button.wav",systemIni.sndVolume);
}
void menu_configlb_GetInOver()
{
	menu->mainDetail->efvct.ReleaseEffects();
	menu->mainDetail->efvct.AddEffect(new GUIEF_Alpha(menu->mainDetail->efvct.a,25,255));
}



void menu_cgmenu_load()
{
	menu->cgmenu->PrepareCG();
	menu->cgmenu->EnterCGMenu();
	ScreenState = SCREEN_MAIN;
	soundEngine.PlayMusic("gamedata\\cg.mp3",2,true);
}

void menu_ListBoxGetOutOver()
{
	ScreenState = SCREEN_MAIN;
	int lastMenuState = menu->MainMenuState;
	menu->MainMenuState = menu->MainStGoto;
	menu->bottom->MainMenuStateChangedEnd();
	if(menu->MainStGoto==MAINST_FREE)
	{
		menu->songlb->GetIn();
		soundEngine.PlayMusic("gamedata\\selectSong.mp3",1,true);
		soundEngine.PlaySound("gamedata\\Sweep.wav",systemIni.sndVolume);
	}
	else if(menu->MainStGoto==MAINST_MAIN)
	{
		menu->mainlb->GetIn();
		if(lastMenuState!=MAINST_CONFIG)
			soundEngine.PlayMusic("gamedata\\mainmenu.mp3",0,true);
		soundEngine.PlaySound("gamedata\\Sweep.wav",systemIni.sndVolume);
	}
	else if(menu->MainStGoto==MAINST_CG)
	{
		//ScreenState = SCREEN_LOADING;
		loading->Begin();
		LoadingToDo = menu_cgmenu_load;
		LoadingOver = NULL;
	}
	else if(menu->MainStGoto==MAINST_CONFIG)
	{
		menu->configlb->GetIn();
		soundEngine.PlaySound("gamedata\\Sweep.wav",systemIni.sndVolume);
	}
	else if(menu->MainStGoto==MAINST_QUIT)
	{
		soundEngine.StopMusic(0,true);
		PostMessage(base::hwnd,WM_QUIT,0,0);
	}
}

void menu_cgmenu_BackToMain()
{
	menu->cgmenu->ReleaseCG();
	menu->MainStGoto = MAINST_MAIN;
	menu->bottom->MainMenuStateChanged( MAINST_MAIN );
	menu_ListBoxGetOutOver();
}
void menu_cgmenu_OnEscClick()
{
	loading->Begin();
	LoadingToDo = menu_cgmenu_BackToMain;
	LoadingOver = NULL;
	soundEngine.StopMusic(2,true);
}

void menu_configlb_EscClick()
{
	menu->songlb->GetOutOver = menu_ListBoxGetOutOver;
	menu->MainStGoto = MAINST_MAIN;
	menu->bottom->MainMenuStateChanged( MAINST_MAIN );
	menu->mainDetail->efvct.ReleaseEffects();
	menu->mainDetail->efvct.AddEffect(new GUIEF_Alpha(menu->mainDetail->efvct.a,25,0));
	menu->configlb->GetOut();

	soundEngine.PlaySound("gamedata\\Sweep.wav",systemIni.sndVolume);
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
	soundEngine.StopMusic(1,true);
	soundEngine.PlaySound("gamedata\\Sweep.wav",systemIni.sndVolume);
}

bool scoreMenuButtonFirstClick=false;
void menu_scoreMenu_OnButtonClick(int id)
{
	if(id==0)
	{
		if(!loading->_begin)
		{
			menu_songlb_GetOutOver();
			soundEngine.PlaySound("gamedata\\Select.wav",systemIni.sndVolume);
		}
	}
	else if(id==1)
	{
		menu->MainStGoto = MAINST_FREE;
		menu->bottom->MainMenuStateChanged( MAINST_FREE );
		LoadingToDo = menu_ListBoxGetOutOver;
		LoadingOver = NULL;
		loading->Begin();
		soundEngine.PlaySound("gamedata\\Select.wav",systemIni.sndVolume);
	}
}

void menu_scoreMenu_OnSelectedIndexChanged(int id)
{
	if(!scoreMenuButtonFirstClick)
	{
		soundEngine.PlaySound("gamedata\\Button.wav",systemIni.sndVolume);
	}
	else
		scoreMenuButtonFirstClick = false;
}

void PausedMenuSelectOver(int id)
{
	if(id==0)
		core.PauseGame(false);
	else if(id==1)
	{
		core.Clear();
		menu_songlb_GetOutOver();
	}
	else
	{
		core.Clear();
		menu->MainStGoto = MAINST_FREE;
		menu->bottom->MainMenuStateChanged( MAINST_FREE );
		LoadingToDo = menu_ListBoxGetOutOver;
		LoadingOver = NULL;
		loading->Begin();
	}
}

void pausedMenu_OnSelectedIndexChanged(int id)
{
	soundEngine.PlaySound("gamedata\\Button.wav",systemIni.sndVolume);
}

void pausedMenu_OnMenuSelectBegin(int id)
{
	soundEngine.PlaySound("gamedata\\Select.wav",systemIni.sndVolume);
}

void MainFirstInit()
{
	graphEngine.ReleaseTex(2);
	//_notemap.Init();
	menu = new MainMenu();
	RefreshConfigDetail();
	menu->mainlb->GetOutOver = menu_ListBoxGetOutOver;
	menu->mainlb->GetInOver = menu_mainlb_GetInOver;
	menu->mainlb->btns->p_OnButtonClick = menu_mainlb_OnButtonClick;
	menu->mainlb->btns->p_OnSelectedIndexChanged = menu_mainlb_OnSelectedIndexChanged;

	menu->songlb->btns->p_EscClick = menu_songlb_EscClick;
	menu->songlb->btns->p_OnButtonClick = menu_songlb_OnButtonClick;
	menu->songlb->GetInOver = menu_songlb_GetInOver;
	menu->songlb->btns->p_OnSelectedIndexChanged = menu_songlb_OnSelectedIndexChanged;

	menu->configlb->GetOutOver = menu_ListBoxGetOutOver;
	menu->configlb->GetInOver = menu_configlb_GetInOver;
	menu->configlb->btns->p_EscClick = menu_configlb_EscClick;
	menu->configlb->btns->p_OnButtonClick = menu_configlb_OnButtonClick;
	menu->configlb->btns->p_OnSelectedIndexChanged = menu_configlb_OnSelectedIndexChanged;
	menu->configlb->btns->p_OnLeftClick = menu_configlb_OnLeftClick;
	menu->configlb->btns->p_OnRightClick = menu_configlb_OnRightClick;

	menu->selectSong->btns->p_OnButtonClick = menu_selectSong_OnButtonClick;
	menu->selectSong->btns->p_EscClick = menu_selectSong_OnEscClick;
	menu->selectSong->btns->p_OnSelectedIndexChanged = menu_selectSong_OnSelectedIndexChanged;

	menu->scoreMenu->btns->p_OnButtonClick = menu_scoreMenu_OnButtonClick;
	menu->scoreMenu->btns->p_OnSelectedIndexChanged = menu_scoreMenu_OnSelectedIndexChanged;
	menu->cgmenu->OnEscClick = menu_cgmenu_OnEscClick;
	ScreenState = SCREEN_MAIN;
	menu->bottom->MainMenuStateChanged( MAINST_MAIN );
	menu->bottom->MainMenuStateChangedEnd();
	soundEngine.PlayMusic("gamedata\\mainmenu.mp3",0,true);

	pausedmenu = new GUIMessageBox();
	pausedmenu->PrepareMenu("PAUSE",120);
	pausedmenu->AddItem("继续");
	pausedmenu->AddItem("重试");
	pausedmenu->AddItem("返回菜单");
	pausedmenu->p_OnMenuSelectOver = PausedMenuSelectOver;
	pausedmenu->p_OnSelectedIndexChanged = pausedMenu_OnSelectedIndexChanged;
	pausedmenu->p_OnMenuSelectBegin = pausedMenu_OnMenuSelectBegin;

	core.SetPausedMenu(pausedmenu);
}

void logo4_SplashOut()
{
	ScreenState = SCREEN_LOADING;
	loading->Begin();
	LoadingToDo = MainFirstInit;
	LoadingOver = NULL;
}

void BackToUIScreen(int coolCnt, int fineCnt, int safeCnt, int sadCnt, int worstCnt, int comboCnt, int chancetimeCnt, int scoreCnt, bool ifclear,int maxScoreCanGet)
{
	graphEngine.ReleaseTex(GameCoreRes);
	scoreMenuButtonFirstClick = true;
	menu->scoreMenu->ResetResult(coolCnt,fineCnt,safeCnt,sadCnt,worstCnt,comboCnt,chancetimeCnt,scoreCnt,ifclear,maxScoreCanGet,
		menu->songInfo.songs[menu->songlb->btns->GetSelectedIndex()].songName+" - "+HardType[menu->selectSong->btns->GetSelectedIndex()] + 
			(menu->selectSong->playMode==GAME_MODE_AUTO?"\nAuto":""));

	if(menu->selectSong->playMode == GAME_MODE_NORMAL)
		menu->songInfo.songs[menu->songlb->btns->GetSelectedIndex()].GetResult(scoreCnt,comboCnt,menu->scoreMenu->result,
			menu->selectSong->btns->GetSelectedIndex(),menu->scoreMenu->ifClear);

	if(menu->selectSong->playMode != GAME_MODE_PV)
	{
		ScreenState = SCREEN_MAIN;
		menu->MainMenuState = MAINST_SCORE;
	}
	else
	{
		ScreenState = SCREEN_LOADING;
		menu->MainStGoto = MAINST_FREE;
		menu->bottom->MainMenuStateChanged( MAINST_FREE );
		LoadingToDo = menu_ListBoxGetOutOver;
		LoadingOver = NULL;
		loading->Begin();
	}
}

void InitUIScreen()
{
	srand(time(0));
	systemIni.Init();
	base::SetWindowSize(systemIni.winWidth,systemIni.winHeight);
	core.Init();
	loading = new SplashScreen(NULL,10000000);
	loading->isloading = true;
	loading->InitOver = loading_InitOver;
	loading->SplashOut = loading_SplashOut;

	logo1 = new SplashScreen(new GUIStaticImage("pic\\logo_gamemaster.png",MakeRect(16,120,480,272),MakeRect(0,0,480,272),2),60);
	soundEngine.PlaySound("gamedata\\gamemaster.wav");
	logo1->Begin();
	ScreenState = SCREEN_LOGO1;
	logo1->SplashOut = logo1_SplashOut;

	logo2 = new SplashScreen(new GUIStaticImage("pic\\logo_tieba.png",MakeRect(16,120,480,272),MakeRect(0,0,480,272),2),2);
	logo2->SplashOut = logo2_SplashOut;

	logo3 = new SplashScreen(new GUIStaticImage("pic\\logo_iVocaloid.png",MakeRect(16,120,480,272),MakeRect(0,0,480,272),2),2);
	logo3->SplashOut = logo3_SplashOut;

	logo4 = new SplashScreen(new GUIStaticImage("pic\\logo_SEGA.png",MakeRect(16,120,480,272),MakeRect(0,0,480,272),2),2);
	logo4->SplashOut = logo4_SplashOut;

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
	else if(ScreenState==SCREEN_LOGO4)
		logo4->Draw();
	else if(ScreenState==SCREEN_MAIN)
		menu->Draw();
	loading->Draw();
}

void GameMainUpdate(double dwTime)
{
	soundEngine.Update();
	if(ScreenState==SCREEN_GAMEMAIN)
		core.Update(dwTime);
	else if(ScreenState==SCREEN_LOGO1)
		logo1->Update();
	else if(ScreenState==SCREEN_LOGO2)
		logo2->Update();
	else if(ScreenState==SCREEN_LOGO3)
		logo3->Update();
	else if(ScreenState==SCREEN_LOGO4)
		logo4->Update();
	else if(ScreenState==SCREEN_MAIN)
		menu->Update();
	loading->Update();
}

void DeleteUIScreen()
{
	SAFE_DELETE(logo1);
	SAFE_DELETE(logo2);
	SAFE_DELETE(logo3);
	SAFE_DELETE(logo4);
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
	{
		Keys[nChar] = false;
		if(ScreenState==SCREEN_GAMEMAIN)
			core.OnKeyEvent(nChar,bKeyDown,bAltDown);
	}
}

