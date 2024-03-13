#include "EffectMana.h"


EffectMana::EffectMana()
{
	drawTex = graphEngine.AddTexture("pic\\Press.png");
	coolTex = graphEngine.AddTexture("pic\\PressCool.png");
	flameTex = graphEngine.AddTexture("pic\\Flame.png");
	numTex = graphEngine.AddTexture("pic\\num.png");
	memset(flameEF,0,sizeof(flameEF));
	//for(int x=0;x<256;x++)
	//{
	//	pSystem.AddParticle((ParticleBase*)(new ParticleTail(x,x,x,255,0,0,graphEngine.AddTexture("pic\\Block.png"))));
	//}
}

void EffectMana::AddEffectUpdateFlame(int x,int y,int noteType)
{
	if(noteType>=MaxFlameEF)
		noteType=MaxFlameEF-1;
	flameEF[noteType]->SetPos(x,y);
}

void EffectMana::AddEffectResetFlame(int noteType)
{
	if(noteType>=MaxFlameEF)
		noteType=MaxFlameEF-1;
	flameEF[noteType]->Reset();
}


void EffectMana::AddEffectNotePress(int x,int y,PLAYSTATE id)
{
	if(id==PLAYSTATE::SAFE||id==PLAYSTATE::FINE||id==PLAYSTATE::COOL)
	{
		effects.push_back((Effect_Base*)(new Effect_NotePressSafe(x,y,drawTex)));
		effects.push_back((Effect_Base*)(new Effect_NotePressFlame(x,y,flameTex)));
	}
	if(id==PLAYSTATE::COOL)
		effects.push_back((Effect_Base*)(new Effect_NotePressFine(x,y,drawTex)));
	if(id==PLAYSTATE::FINE)
		effects.push_back((Effect_Base*)(new Effect_NotePressCool(x,y,coolTex)));
	
}

void EffectMana::SetNew()
{
	Clear();
	for(int i=0;i<MaxFlameEF;i++)
		flameEF[i] = new Effect_Flame(drawTex);
}

void EffectMana::Clear()
{
	for(vector<Effect_Base*>::iterator ptr = effects.begin();ptr!=effects.end();ptr++)
		delete (*ptr);
	effects.clear();
	numberMana.Clear();
	for(int i=0;i<MaxFlameEF;i++)
		SAFE_DELETE(flameEF[i]);
}

void EffectMana::Draw()
{
	for(vector<Effect_Base*>::iterator ptr = effects.begin();ptr!=effects.end();ptr++)
		(*ptr)->Draw();
	//numberMana.Draw(78915,MakeRect(371,0,0,0),NumberStyle::SCORE,NumberDwFlags::LEFT,D3DCOLOR_ARGB(255,255,255,255),0,7);
	numberMana.DrawCombo();
	for(int i=0;i<MaxFlameEF;i++)
		flameEF[i]->Draw();
}

void EffectMana::Update()
{
	vector<int> needDel;
	for(vector<Effect_Base*>::iterator ptr = effects.begin();ptr!=effects.end();ptr++)
	{
		(*ptr)->Update();
		if((*ptr)->Die())
			needDel.push_back(ptr-effects.begin());
	}
	for(int delCnt=0;delCnt<needDel.size();delCnt++)
	{
		vector<Effect_Base*>::iterator ptr = effects.begin()+(needDel[delCnt]-delCnt);
		delete (*ptr);
		effects.erase(ptr);
	}
	for(int i=0;i<MaxFlameEF;i++)
		flameEF[i]->Update();
	numberMana.Update();
}

void Effect_NotePressSafe::Draw()
{
	graphEngine.Draw(_drawTex,MakeRect(0,0,100,100),MakeRect(_x-delta,_y-delta,delta*2,delta*2),D3DCOLOR_ARGB(int(float(150*(desframe-nowframe)/desframe)),255,255,255));
}

void Effect_NotePressSafe::Update()
{
	if(nowframe<20)
		delta+=2;
	else
		delta+=1;
	nowframe++;
}

void Effect_NotePressFine::Draw()
{
	graphEngine.Draw(_drawTex,MakeRect(100+((nowframe-1)/2%4)*160,((nowframe-1)/2/4)*160,160,160),MakeRect(_x-40+5,_y-40,80,80),D3DCOLOR_ARGB(150,255,255,255));
}

void Effect_NotePressFine::Update()
{
	nowframe++;
}

void Effect_NotePressCool::Draw()
{
	graphEngine.Draw(_drawTex,MakeRect((nowframe-1)/2%3*640,(nowframe-1)/2/3*480,640,480),MakeRect(_x-160,_y-120,320,240),D3DCOLOR_ARGB(150,255,255,255));
}

void Effect_NotePressCool::Update()
{
	nowframe++;
}

void Effect_ExtraScore::Draw()
{
	if(_extraScore!=0)
	{
		graphEngine.Draw(_drawTex,MakeRect(0,58,15,14),_x-29,_y-35);
		numberMana.Draw(_extraScore,MakeRect(_x-14,_y-35,0,0),NumberStyle::EXTRASCORE,NumberDwFlags::LEFT,D3DCOLOR_ARGB(255,255,255,255),0);
	}
}

void Effect_ExtraScore::Update()
{
	nowframe++;
	_y--;
}

void Effect_ChanceTimeEnd::Draw()
{
	int drawAlpha;
	if(nowframe<20)
		drawAlpha = int(float(nowframe)/20.0*255.0);
	else if(nowframe<160)
		drawAlpha = 255;
	else
		drawAlpha = int(float(180-nowframe)/20.0*255.0);
	graphEngine.Draw(_drawTex,MakeRect(0,128,368,24),56,225,D3DCOLOR_ARGB(drawAlpha,255,255,255));
	numberMana.Draw(_chanceTimeScore,MakeRect(56,208,368,0),NumberStyle::CHANCE,NumberDwFlags::MID,D3DCOLOR_ARGB(drawAlpha,255,255,255),0);
}

void Effect_ChanceTimeEnd::Update()
{
	nowframe++;
}

void Effect_ChanceTimeStart::Draw()
{
	int drawAlpha;
	if(nowframe<20)
		drawAlpha = int(float(nowframe)/20.0*255.0);
	else if(nowframe<160)
		drawAlpha = 255;
	else
		drawAlpha = int(float(180-nowframe)/20.0*255.0);
	graphEngine.Draw(_drawTex,MakeRect(0,104,368,24),56,225,D3DCOLOR_ARGB(drawAlpha,255,255,255));
}

void Effect_ChanceTimeStart::Update()
{
	nowframe++;
}

void NumberMana::Draw(int number, RECT drawRect, NumberStyle style, NumberDwFlags flag,D3DCOLOR color,int inc,int fill)
{
	int srcWidth,srcHeight,srcLeft,srcTop,deltaWidth;
	switch(style)
	{
	case NumberStyle::COMBO:
		srcWidth=18,srcHeight=18,srcLeft=0,srcTop=0,deltaWidth=16;
		break;
	case NumberStyle::SCORE:
		srcWidth=18,srcHeight=18,srcLeft=0,srcTop=21,deltaWidth=13;
		break;
	case NumberStyle::CHANCE:
		srcWidth=15,srcHeight=14,srcLeft=0,srcTop=41,deltaWidth=12;
		break;
	case NumberStyle::EXTRASCORE:
		srcWidth=15,srcHeight=14,srcLeft=15,srcTop=58,deltaWidth=12;
		break;
	}
	vector<int> drawNum;
	if(number==0)
		drawNum.push_back(0);
	while(number)
	{
		drawNum.push_back(number%10);
		number/=10;
	}
	fill-=drawNum.size();
	for(int i=0;i<fill;i++)
		drawNum.push_back(0);
	switch(flag)
	{
	case NumberDwFlags::LEFT:
		for(int i=drawNum.size()-1;i>=0;i--)
			graphEngine.Draw(numTex,MakeRect(srcLeft+((drawNum[i]==0?10:drawNum[i])-1)*srcWidth,srcTop,srcWidth,srcHeight),
				MakeRect(drawRect.left+(drawNum.size()-i-1)*deltaWidth-inc,drawRect.top-inc,srcWidth+inc*2,srcHeight+inc*2),color);
		break;
	case NumberDwFlags::RIGHT:
		for(int i=0;i<drawNum.size();i++)
			graphEngine.Draw(numTex,MakeRect(srcLeft+((drawNum[i]==0?10:drawNum[i])-1)*srcWidth,srcTop,srcWidth,srcHeight),
				MakeRect(drawRect.right-(i+1)*deltaWidth-inc,drawRect.top-inc,srcWidth+inc*2,srcHeight+inc*2),color);
		break;
	case NumberDwFlags::MID:
		for(int i=drawNum.size()-1;i>=0;i--)
			graphEngine.Draw(numTex,MakeRect(srcLeft+((drawNum[i]==0?10:drawNum[i])-1)*srcWidth,srcTop,srcWidth,srcHeight),
				MakeRect((drawRect.left+drawRect.right)/2-srcWidth*drawNum.size()/2+(drawNum.size()-i-1)*deltaWidth-inc,drawRect.top-inc,srcWidth+inc*2,srcHeight+inc*2),color);
		break;
	}
}

void NumberMana::DrawScore(int score)
{
	numberMana.Draw(score,MakeRect(371,0,0,0),NumberStyle::SCORE,NumberDwFlags::LEFT,D3DCOLOR_ARGB(255,255,255,255),0,7);
}

void NumberMana::SetCombo(int combo,int x,int y)
{
	SAFE_DELETE(comboDraw);
	comboDraw = new Effect_Combo(x,y,combo,numTex);
}

void NumberMana::SetExtraScore(int extraScore,int x,int y)
{
	SAFE_DELETE(extraScoreDraw);
	extraScoreDraw = new Effect_ExtraScore(x,y,extraScore,numTex);
}

void NumberMana::DrawCombo()
{
	if(comboDraw)
		comboDraw->Draw();
	if(extraScoreDraw)
		extraScoreDraw->Draw();
}

void NumberMana::Update()
{
	if(comboDraw)
	{
		comboDraw->Update();
		if(comboDraw->Die())
			SAFE_DELETE(comboDraw);
	}
	if(extraScoreDraw)
	{
		extraScoreDraw->Update();
		if(extraScoreDraw->Die())
			SAFE_DELETE(extraScoreDraw);
	}
}

void NumberMana::Clear()
{
	SAFE_DELETE(comboDraw);
	SAFE_DELETE(extraScoreDraw);
}

void Effect_Combo::Draw()
{
	if(_combo>4)
	{
		int nowA,nowInc;
		if(nowframe<3)
			nowInc=0,nowA=255;
		else if(nowframe<10)
			nowInc=4,nowA=255;
		else if(nowframe<30)
			nowInc=0,nowA=255;
		else
			nowInc=0,nowA=int(float(255*(desframe-nowframe)/10));
		graphEngine.Draw(_drawTex,MakeRect(4,77,97,15),_x-45,_y-24,D3DCOLOR_ARGB(nowA,255,255,255));
		graphEngine.Draw(_drawTex,MakeRect(157,77,38,9),_x+10,_y-24,D3DCOLOR_ARGB(nowA,255,255,255));
		numberMana.Draw(_combo,MakeRect(_x+10,_y-30,0,0),NumberStyle::COMBO,NumberDwFlags::RIGHT,D3DCOLOR_ARGB(nowA,255,255,255),nowInc);
	}
}

void Effect_Combo::Update()
{
	nowframe++;
}

void Effect_NotePressFlame::Draw()
{
	graphEngine.Draw(_drawTex,MakeRect(0,0,200,200),MakeRect(_x-nowframe*4,_y-nowframe*4,nowframe*8,nowframe*8),D3DCOLOR_ARGB(int(float(255*(desframe-nowframe)/desframe)),255,255,255));
}

void Effect_NotePressFlame::Update()
{
	nowframe++;
}

void Effect_Flame::SetPos(int x, int y)
{
	_x = x;
	_y = y;
	Updated = false;
}

void Effect_Flame::Update()
{
	nowframe++;
}

void Effect_Flame::Draw()
{
	if(Updated)
		return;
	else
	{
		graphEngine.Draw(_drawTex,MakeRect((nowframe-1)/2%3*640,(nowframe-1)/2/3*480,640,480),MakeRect(_x-160,_y-120,320,240),D3DCOLOR_ARGB(150,255,255,255));
		Updated = true;
	}
}

void Effect_Flame::Reset()
{
	nowframe=0;
}