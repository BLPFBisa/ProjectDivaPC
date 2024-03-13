#include "GameMana.h"
#include "EffectMana.h"
#include "Gui.h"
#include "UIScreen.h"

int nowDistance;
GUIStaticImage *white;
GUIStaticImage *black_top,*black_bottom,*chanceTimeLabel;

void GameData::Init()
{
	note_pic = graphEngine.AddTexture("pic\\note.png");
	background = NULL;
	pointer_rect = MakeRect(0,24,24,32);

	//note
	note_rect[0] = MakeRect(0,0,24,24);
	note_rect[1] = MakeRect(25,0,24,24);
	note_rect[2] = MakeRect(50,0,24,24);
	note_rect[3] = MakeRect(75,0,24,24);

	note_rect[4] = MakeRect(100,0,25,26);
	note_rect[5] = MakeRect(125,0,25,26);
	note_rect[6] = MakeRect(150,0,26,25);
	note_rect[7] = MakeRect(180,0,26,25);

	//rhythm
	rhythm_rect[0] = MakeRect(0,70,28,28);
	rhythm_rect[1] = MakeRect(30,70,28,28);
	rhythm_rect[2] = MakeRect(60,70,28,28);
	rhythm_rect[3] = MakeRect(90,70,28,28);

	rhythm_rect[4] = MakeRect(125,70,24,26);
	rhythm_rect[5] = MakeRect(150,70,24,26);
	rhythm_rect[6] = MakeRect(175,70,24,26);
	rhythm_rect[7] = MakeRect(200,70,24,26);

	//note_strip
	note_rect_strip[0] = MakeRect(0,185,24,24);
	note_rect_strip[1] = MakeRect(25,185,24,24);
	note_rect_strip[2] = MakeRect(50,185,24,24);
	note_rect_strip[3] = MakeRect(75,185,24,24);

	note_rect_strip[4] = MakeRect(100,185,25,26);
	note_rect_strip[5] = MakeRect(125,185,25,26);
	note_rect_strip[6] = MakeRect(150,185,26,25);
	note_rect_strip[7] = MakeRect(180,185,26,25);

	//rhythm_strip
	rhythm_rect_strip[0] = MakeRect(0,100,40,40);
	rhythm_rect_strip[1] = MakeRect(40,100,38,38);
	rhythm_rect_strip[2] = MakeRect(80,100,42,40);
	rhythm_rect_strip[3] = MakeRect(125,100,43,40);

	rhythm_rect_strip[4] = MakeRect(0,140,38,38);
	rhythm_rect_strip[5] = MakeRect(40,140,37,37);
	rhythm_rect_strip[6] = MakeRect(80,140,36,37);
	rhythm_rect_strip[7] = MakeRect(120,140,36,37);

	//rhythm_rect[0] = MakeRect(239,0,40,40), rhythm_rect[1] = MakeRect(327,0,40,40),rhythm_rect[2] = MakeRect(283,0,40,40),rhythm_rect[3] = MakeRect(195,0,40,40);
	presh_rect[0] = presh_rect[1] = presh_rect[2] = presh_rect[3] = MakeRect(279,0,40,40);
	state_rect[0] = MakeRect(440,120,65,19), state_rect[1] = MakeRect(301,120,65,19), state_rect[2] = MakeRect(376,120,65,19);
	state_rect[3] = MakeRect(440,139,47,19), state_rect[4] = MakeRect(301,139,65,19), state_rect[5] = MakeRect(376,139,65,19);
	state_rect[6] = MakeRect(301,158,79,18);
	hp = gameini.HP/2, score = 0, combo = 0;
	cool_cnt = fine_cnt = safe_cnt = worst_cnt = maxcombo = 0;
	_chanceTimeScore = _chanceTimeCombo = chanceTime = 0;
	maxScoreCanGet = maxScoreTempCombo = maxScoreTempChanceTimeCombo = 0;
}

void GameData::SetNew()
{
	hp = gameini.HP/2, score = 0, combo = 0;
	cool_cnt = fine_cnt = safe_cnt = worst_cnt = maxcombo = 0;
	_chanceTimeScore = _chanceTimeCombo = chanceTime = 0;
	maxScoreCanGet = maxScoreTempCombo = maxScoreTempChanceTimeCombo = 0;
	background = graphEngine.AddTexture("pic\\black.png");
}

void GameData::ModifyHP(int delta)
{
	if(delta>0&&hp<gameini.HP)
	{
		hp = min(hp+delta,gameini.HP);
		gameui.SetUIState(ADD);
	}
	if(delta<0&&hp>0)
	{
		hp = max(hp+delta,0);
		gameui.SetUIState(DEC);
	}
}

Unit::Unit(Note _note,double standing, int _notePos)
{
	delayTime = 0;
	noteType = NOTETYPE_NORMAL;
	note = Point(gameini.ORIGIN_X+_note._x*gameini.DELTA_X+12,gameini.ORIGIN_Y+_note._y*gameini.DELTA_Y+12);
	rhythm = Point(_note._tailx,_note._taily);
    type = (UNIT)_note._type, restTime = standing+gameini.delay, totalTime = standing, notePos = _notePos, key = _note._key;
    angle = 0, preshed = false, first = true;
	//---------------------------bezier------------------------------
	/*while(true)
	{
		double x = rand()%gameini.DISTANCE+1, y = rand()%gameini.DISTANCE+1;
		double p1 = x*note.x*(x-note.x), p2 = note.x-x, p3 = y*note.x-note.y*x;
		double p4 = x*rhythm.x*(x-rhythm.x), p5 = rhythm.x-x, p6 = y*rhythm.x-rhythm.y*x;
		if(fabs(p1*p4)<EPS||fabs(p2*p4-p1*p5)<EPS)
			continue;
		_c = (p3*p4-p1*p6)/(p2*p4-p1*p5);
		_a = (p3*p4-_c*p2*p4)/(p1*p4);
		_b = (y-_c-_a*x*x)/x;
		break;
	}*/
}

Unit::Unit(Note _note, double standing, int _notePos, double _duration)
{
	delayTime = 0, totalduration = _duration, duration = _duration+gameini.delay;
	noteType = NOTETYPE_STRIP;
	note = Point(gameini.ORIGIN_X+_note._x*gameini.DELTA_X+12,gameini.ORIGIN_Y+_note._y*gameini.DELTA_Y+12);
	rhythm = Point(_note._tailx,_note._taily);
	type = (UNIT)(_note._type-NOTENum), restTime = standing+gameini.delay, totalTime = standing, notePos = _notePos, key = _note._key;
	angle = 0, preshed = false, first = true, start = false;
}

bool Unit::finish()
{
	if(noteType==NOTETYPE_NORMAL) 
		return restTime<EPS; 
	else if(start&&fabs(duration)<EPS) 
		return true;
	else if(!start&&fabs(restTime)<EPS)
		return true;
	else
		return false;
}

void Unit::Update(double dwTime)
{
	if(!preshed)
	{
		if(noteType==NOTETYPE_NORMAL)
		{
			restTime -= dwTime;
			if(GAMEDATA.gamemode==GAME_MODE_PV&&restTime<=gameini.delay)
				Presh(COOL);
			if(restTime<0) restTime = 0;
		}
		else
		{
			restTime -= dwTime;
			if(restTime<gameini.delay)
				duration -= dwTime;
			if(GAMEDATA.gamemode==GAME_MODE_PV&&!start&&restTime<=gameini.delay)
				Presh(COOL);
			if(GAMEDATA.gamemode==GAME_MODE_PV&&start&&duration<=gameini.delay)
				Presh(COOL);
			if(restTime<0) restTime = 0;
			if(duration<0) duration = 0;
		}
		if(noteType==NOTETYPE_NORMAL)
		{
			double percent = (restTime-gameini.delay)<0?0:(restTime-gameini.delay)/totalTime;
			angle = -(PI+PI)*percent;
		}
		else
		{
			double percent = (((duration-gameini.delay)<0?0:(duration-gameini.delay))+((restTime-gameini.delay)<0?0:(restTime-gameini.delay)))/(totalTime+totalduration);
			angle = -(PI+PI)*percent;
		}
	}
	else
	{
		delayTime -= dwTime;
		if(delayTime<0)
			delayTime = 0;
	}
}

void Unit::AddMetafile(int x, int y, D3DCOLOR color, int flag)
{
	RECT rect;
	if(flag==0)
	{
		rect = MakeRect(0,215,15,15);
		graphEngine.Draw(GAMEDATA.note_pic,rect,x-7,y-7,color);
	}
	else if(flag==1)
	{
		rect = MakeRect(15*type,235,13,13);
		graphEngine.Draw(GAMEDATA.note_pic,rect,x-6,y-6,color);
	}
}

void Unit::AddParticle(int x, int y)
{
	int a = 100,r,g,b;
	switch(type)
	{
	case UNIT_CIRCLE:
	case UNIT_LEFT:
		r = 246, g = 127, b = 167;
		break;
	case UNIT_RECT:
	case UNIT_RIGHT:
		r = 240, g = 135, b = 230;
		break;
	case UNIT_CROSS:
	case UNIT_UP:
		r = 98, g = 191, b = 225;
	    break;
	case UNIT_TRIANGLE:
	case UNIT_DOWN:
		r = 234,g = 254, b = 183;
		break;
	}
	pSystem.AddParticle((ParticleBase*)(new ParticleTail(x,y,a,r,g,b,graphEngine.AddTexture("pic\\Block.png"))),notePos*4+type);
}

Point Unit::Bezier(const Point &p0, const Point &p1, const Point &p2, const double &t)
{
	double tt = 1-t;
	return p0*tt*tt+p1*2*t*tt+p2*t*t;
}

Point Unit::Bezier(const Point &p0, const Point &p1, const Point &p2, const Point &p3, const double &t)
{
	double tt = 1-t;
	return p0*tt*tt*tt+p1*3*t*tt*tt+p2*3*t*t*tt+p3*t*t*t;
}

void Unit::DrawLine(const Point &a, const Point &b)
{
	Point tmp;
	int dx = (int)abs(b.x-a.x), dy = (int)abs(b.y-a.y),x = 0,y = 0,error = 0;
	if(dx>=dy)
	{
		for(int  i = 0; i <= dx;i ++)
		{
			if(a.x<=b.x)
			{
				if(a.y<=b.y)
					AddParticle(a.x+x,a.y+y);
				else
					AddParticle(a.x+x,a.y-y);
			}
			else
			{
				if(a.y<=b.y)
					AddParticle(a.x-x,a.y+y);
				else
					AddParticle(a.x-x,a.y-y);
			}
			error += dy;
			if(error>dx)
			{
			error -= dx; y++;
			}
			x++;
		}
	}
	else
	{
		for(int  i = 0; i <= dy; i++)
		{
			if(a.x<=b.x)
			{
				if(a.y<=b.y)
					AddParticle(a.x+x,a.y+y);
				else
					AddParticle(a.x+x,a.y-y);
			}
			else
			{
				if(a.y<=b.y)
					AddParticle(a.x-x,a.y+y);
				else
					AddParticle(a.x-x,a.y-y);
			}
			error += dx;
			if(error>dy)
			{
			error -= dy; x++;
			}
			y++;
		}
	}
}

void Unit::DrawBar(const Point &a, const Point &b, D3DCOLOR color, int flag)
{
	Point tmp;
	int dx = (int)abs(b.x-a.x), dy = (int)abs(b.y-a.y),x = 0,y = 0,error = 0;
	if(dx>=dy)
	{
		for(int  i = 0; i <= dx;i ++)
		{
			if(a.x<=b.x)
			{
				if(a.y<=b.y)
					AddMetafile(a.x+x,a.y+y,color,flag);
				else
					AddMetafile(a.x+x,a.y-y,color,flag);
			}
			else
			{
				if(a.y<=b.y)
					AddMetafile(a.x-x,a.y+y,color,flag);
				else
					AddMetafile(a.x-x,a.y-y,color,flag);
			}
			error += dy;
			if(error>dx)
			{
			error -= dx; y++;
			}
			x++;
		}
	}
	else
	{
		for(int  i = 0; i <= dy; i++)
		{
			if(a.x<=b.x)
			{
				if(a.y<=b.y)
					AddMetafile(a.x+x,a.y+y,color,flag);
				else
					AddMetafile(a.x+x,a.y-y,color,flag);
			}
			else
			{
				if(a.y<=b.y)
					AddMetafile(a.x-x,a.y+y,color,flag);
				else
					AddMetafile(a.x-x,a.y-y,color,flag);
			}
			error += dx;
			if(error>dy)
			{
			error -= dy; x++;
			}
			y++;
		}
	}
}

void Unit::Draw_Normal()
{
	double realTime = (restTime-gameini.delay)<0?0:(restTime-gameini.delay);
	int width,height;
	//draw note
	D3DXMATRIX mat,mat2,ptransform;
	base::Sprite->GetTransform(&ptransform);
	D3DXMatrixTransformation2D(&mat, NULL, 0.0f, NULL,
    &D3DXVECTOR2(note.x,note.y), angle, NULL);
	width = (GAMEDATA.note_rect[type].right-GAMEDATA.note_rect[type].left);
	height = (GAMEDATA.note_rect[type].bottom-GAMEDATA.note_rect[type].top);
	if(realTime<totalTime*0.875)
	{
		graphEngine.Draw(GAMEDATA.note_pic,GAMEDATA.note_rect[type],note.x-(width>>1),note.y-(height>>1)/*,D3DCOLOR_ARGB(int((restTime/gameini.standing)*255),255,255,255)*/);
		base::Sprite->SetTransform(&mat);
		graphEngine.Draw(GAMEDATA.note_pic,GAMEDATA.pointer_rect,note.x-(width>>1),note.y-(height>>1)-gameini.ARROW_HEAD/*,D3DCOLOR_ARGB(int((restTime/gameini.standing)*255),255,255,255)*/);
	}
	else
	{
		double percent = 1.0f+(gameini.NOTE_BLOWUP-1.0f)*((realTime-totalTime*0.875)/(totalTime*0.125));
		D3DXMatrixScaling(&mat2,percent,percent,0);
		mat = mat2*mat;
		base::Sprite->SetTransform(&mat2);
		width = width*percent, height =  (height+gameini.ARROW_HEAD)*percent;
		graphEngine.Draw(GAMEDATA.note_pic,GAMEDATA.note_rect[type],(note.x-width/2)/percent,(note.y-width/2)/percent/*,D3DCOLOR_ARGB(int((restTime/gameini.standing)*255),255,255,255)*/);
		base::Sprite->SetTransform(&mat);
		graphEngine.Draw(GAMEDATA.note_pic,GAMEDATA.pointer_rect,(note.x-width/2)/percent,(note.y-width/2-(height-width))/percent/*,D3DCOLOR_ARGB(int((restTime/gameini.standing)*255),255,255,255)*/);
	}
	base::Sprite->SetTransform(&ptransform);

	//draw rhythm
	double percent = (restTime-gameini.delay)/totalTime;
	Point _rhythm;
	//Point source = note+(rhythm-note).unit()*gameini.DISTANCE;
    //_rhythm.y = _a*_rhythm.x*_rhythm.x+_b*_rhythm.x+_c;
	width = (GAMEDATA.rhythm_rect[type].right-GAMEDATA.rhythm_rect[type].left);
	height = (GAMEDATA.rhythm_rect[type].bottom-GAMEDATA.rhythm_rect[type].top);
	if(restTime<gameini.delay)
	{
		_rhythm = note+(rhythm-note).unit()*(nowDistance*2/3);
		bezier_p1 = note+(_rhythm-note)*0.25+(_rhythm-note).normal()*0.15;
		bezier_p2 = note+(_rhythm-note)*0.75-(_rhythm-note).normal()*0.15;
		_rhythm = Bezier(note,bezier_p1,bezier_p2,_rhythm,percent);
		percent = restTime/gameini.delay;
		base::Sprite->GetTransform(&ptransform);
		D3DXMatrixScaling(&mat,percent,percent,0);
		base::Sprite->SetTransform(&mat);
		width = width*percent;
		graphEngine.Draw(GAMEDATA.note_pic,preshed?GAMEDATA.presh_rect[type]:GAMEDATA.rhythm_rect[type],(_rhythm.x-width/2)/percent,(_rhythm.y-width/2)/percent);
		base::Sprite->SetTransform(&ptransform);
	}
	else
	{
		_rhythm = note+(rhythm-note).unit()*(nowDistance);
		bezier_p1 = note+(_rhythm-note)*0.25+(_rhythm-note).normal()*0.15;
		bezier_p2 = note+(_rhythm-note)*0.75-(_rhythm-note).normal()*0.15;
		_rhythm = Bezier(note,bezier_p1,bezier_p2,_rhythm,percent);
		graphEngine.Draw(GAMEDATA.note_pic,preshed?GAMEDATA.presh_rect[type]:GAMEDATA.rhythm_rect[type],_rhythm.x-(width>>1),_rhythm.y-(height>>1));
		if(first)
		{
			AddParticle(_rhythm.x,_rhythm.y);
			first = false;
		}
		else
		{
			DrawLine(lastPoint,_rhythm);
			/*for(int i = int(lastPoint.x); i != (int)_rhythm.x; i=(i<(int)_rhythm.x?(i+1):(i-1)))
			{
				tmp = Point(i,lastPoint.y+(_rhythm.y-lastPoint.y)*(i-lastPoint.x)/(_rhythm.x-lastPoint.x));
				pSystem.AddParticle((ParticleBase*)(new ParticleTail(tmp.x,tmp.y,255,234,254,183,graphEngine.AddTexture("pic\\Block.png"))));
			}*/
		}
		lastPoint = _rhythm;
	}
}

void Unit::Draw_Strip()
{
	double realTime = (restTime-gameini.delay)<0?0:(restTime-gameini.delay);
	int width,height;
	//draw note
	D3DXMATRIX mat,mat2,ptransform;
	base::Sprite->GetTransform(&ptransform);
	D3DXMatrixTransformation2D(&mat, NULL, 0.0f, NULL,
    &D3DXVECTOR2(note.x,note.y), angle, NULL);
	width = (GAMEDATA.note_rect_strip[type].right-GAMEDATA.note_rect_strip[type].left);
	height = (GAMEDATA.note_rect_strip[type].bottom-GAMEDATA.note_rect_strip[type].top);
	if(realTime<totalTime*0.875)
	{
		graphEngine.Draw(GAMEDATA.note_pic,GAMEDATA.note_rect_strip[type],note.x-(width>>1),note.y-(height>>1)/*,D3DCOLOR_ARGB(int((restTime/gameini.standing)*255),255,255,255)*/);
		base::Sprite->SetTransform(&mat);
		graphEngine.Draw(GAMEDATA.note_pic,GAMEDATA.pointer_rect,note.x-(width>>1),note.y-(height>>1)-gameini.ARROW_HEAD/*,D3DCOLOR_ARGB(int((restTime/gameini.standing)*255),255,255,255)*/);
	}
	else
	{
		double percent = 1.0f+(gameini.NOTE_BLOWUP-1.0f)*((realTime-totalTime*0.875)/(totalTime*0.125));
		D3DXMatrixScaling(&mat2,percent,percent,0);
		mat = mat2*mat;
		base::Sprite->SetTransform(&mat2);
		width = width*percent, height =  (height+gameini.ARROW_HEAD)*percent;
		graphEngine.Draw(GAMEDATA.note_pic,GAMEDATA.note_rect_strip[type],(note.x-width/2)/percent,(note.y-width/2)/percent/*,D3DCOLOR_ARGB(int((restTime/gameini.standing)*255),255,255,255)*/);
		base::Sprite->SetTransform(&mat);
		graphEngine.Draw(GAMEDATA.note_pic,GAMEDATA.pointer_rect,(note.x-width/2)/percent,(note.y-width/2-(height-width))/percent/*,D3DCOLOR_ARGB(int((restTime/gameini.standing)*255),255,255,255)*/);
	}
	base::Sprite->SetTransform(&ptransform);

	//draw rhythm
	double percent = realTime/totalTime, realdura = duration-gameini.delay<0?0:(duration-gameini.delay);
	Point _rhythm,_tail;
	width = (GAMEDATA.rhythm_rect_strip[type].right-GAMEDATA.rhythm_rect_strip[type].left);
	height = (GAMEDATA.rhythm_rect_strip[type].bottom-GAMEDATA.rhythm_rect_strip[type].top);
	//Point source = note+(rhythm-note).unit()*gameini.DISTANCE;
    //_rhythm.y = _a*_rhythm.x*_rhythm.x+_b*_rhythm.x+_c;

		_rhythm = note+(rhythm-note).unit()*(nowDistance);

		_tail = Bezier(note,bezier_p1,bezier_p2,_rhythm,percent+realdura/totalTime);
		//Draw_Bezier(note,bezier_p1,bezier_p2,_rhythm,percent,percent+realdura/totalTime,9);
		//Draw_Bezier(note,bezier_p1,bezier_p2,_rhythm,percent,percent+realdura/totalTime,8);
		//Draw_Bezier(note,bezier_p1,bezier_p2,_rhythm,percent,percent+realdura/totalTime,7);
		//Draw_Bezier(note,bezier_p1,bezier_p2,_rhythm,percent,percent+realdura/totalTime,6);
		//Draw_Bezier(note,bezier_p1,bezier_p2,_rhythm,percent,percent+realdura/totalTime,5);
		Draw_Bezier(note,bezier_p1,bezier_p2,_rhythm,percent,percent+realdura/totalTime,0);
		Draw_Bezier(note,bezier_p1,bezier_p2,_rhythm,percent,percent+realdura/totalTime,1);
		//Draw_Bezier(note,bezier_p1,bezier_p2,_rhythm,percent,percent+realdura/totalTime,1);
		//Draw_Bezier(note,bezier_p1,bezier_p2,_rhythm,percent,percent+realdura/totalTime,0);
		//Draw_Bezier(note,bezier_p1,bezier_p2,_rhythm,percent,percent+realdura/totalTime,2);
		
		_rhythm = note+(rhythm-note).unit()*(nowDistance);
		bezier_p1 = note+(_rhythm-note)*0.25+(_rhythm-note).normal()*0.15;
		bezier_p2 = note+(_rhythm-note)*0.75-(_rhythm-note).normal()*0.15;
		_rhythm = Bezier(note,bezier_p1,bezier_p2,_rhythm,percent);
		if(fabs(restTime)>EPS&&!start)
			graphEngine.Draw(GAMEDATA.note_pic,preshed?GAMEDATA.presh_rect[type]:GAMEDATA.rhythm_rect_strip[type],_rhythm.x-(width>>1),_rhythm.y-(height>>1));

		graphEngine.Draw(GAMEDATA.note_pic,preshed?GAMEDATA.presh_rect[type]:GAMEDATA.rhythm_rect_strip[type],_tail.x-(width>>1),_tail.y-(height>>1));
	//}
}

void Unit::Draw_Bezier(const Point &p0, const Point &p1, const Point &p2, const Point &p3, double l, double r, int flag, D3DCOLOR color)
{
	const int TOTAL = 100;
	int NUM = TOTAL*r;
	double t = 0;
	Point p = Bezier(p0,p1,p2,p3,l), q;
	for(int i = TOTAL*l; i <= NUM; i++)
	{
		t = i/double(TOTAL);
		q = Bezier(p0,p1,p2,p3,t);
		DrawBar(p,q,color,flag), p = q;
	}
	q = Bezier(p0,p1,p2,p3,r);
	DrawBar(p,q,color,flag);
}

void Unit::Draw_Frame(const Point &p0, const Point &p1, const Point &p2, D3DCOLOR color)
{
	const int TOTAL = 100;
	int NUM = TOTAL;
	double t = 0;
	Point p = p0, q;
	for(int i = 0; i <= NUM; i++)
	{
		t = i/double(TOTAL);
		q = Bezier(p0,p1,p2,t);
		DrawBar(p,q,color,1), p = q;
	}
}

void Unit::Draw()
{
	if(noteType==NOTETYPE_NORMAL)
		Draw_Normal();
	else
		Draw_Strip();
}

void Unit::Clear()
{
	pSystem.Clear(notePos*4+type);
}

void Unit::Presh(PLAYSTATE id)
{
	if(noteType==NOTETYPE_NORMAL)
	{
		preshed = true;

		delayTime = 0;
	}
	else
	{
		if(!start)
			start = true;
		else
		{
			preshed = true;

			delayTime = 0;
		}
	}


	if(id!=WORST)
	{
		if(noteType==NOTETYPE_NORMAL)
		{
			EffectMana::Instance().AddEffectNotePress(note.x,note.y,id);
			numberMana.SetCombo(GAMEDATA.combo,note.x,note.y);
		}
		else
		{
			EffectMana::Instance().AddEffectNotePress(note.x,note.y,id);
			numberMana.SetCombo(GAMEDATA.combo,note.x,note.y);
		}

	if(key!=NOWAV)
	{
		string filename = _notemap.GetWav(key);
		if(filename!="")
			soundEngine.PlayMusic(filename);
	}

	}
}

void NoteMana::Run(LpFrame _note)
{
	if(_note==NULL)
		return;
	if(_note->_IsSetBPM)
		singleTime = _notemap.Cal_Single(_note->_BPM);
	for(int i = 0; i < _note->_noteNum; i++)
		if(_note->_note[i]._type<NOTENum)
			unit.push_back(Unit(_note->_note[i],noteTime-nowTime>0?noteTime-nowTime:0,_note->_note[i].notePos));
		else
			unit.push_back(Unit(_note->_note[i],noteTime-nowTime>0?noteTime-nowTime:0,_note->_note[i].notePos,_note->_note[i].duration*singleTime));
}

void NoteMana::SetNew()
{
	singleTime = noteTime = nowTime = 0;
	Clear();
}

void NoteMana::Draw()
{
	for(vector<Unit>::iterator ptr = unit.begin(); ptr != unit.end(); ptr++)
		ptr->Draw();
}

void NoteMana::Add(int pos)
{
	if(pos>=_notemap.GetTotalPos())
		pos = _notemap.GetTotalPos();
	while(_notemap.GetNotesPos()<pos) Run(_notemap.GetNotes()), noteTime += singleTime,_notemap.NextNotes();
}

void NoteMana::Update(double dwTime)
{
	nowTime += dwTime;

	for(vector<Unit>::iterator ptr = unit.begin(); ptr != unit.end(); ptr++)
		ptr->Update(dwTime);
	for(vector<Unit>::iterator ptr = unit.begin(); ptr != unit.end(); ptr++)
		if(ptr->preshed)
		{
			if(fabs(ptr->delayTime)<EPS)
			{
				ptr->Clear();
				unit.erase(ptr);
			}
			break;
		}

	if(GAMEDATA.gamemode==GAME_MODE_PV)
		return;

	bool findNew = true;

	while(findNew)
	{
		findNew = false;
		for(vector<Unit>::iterator ptr = unit.begin(); ptr != unit.end(); ptr++)
		{
			if(!ptr->preshed)
			{
				if(ptr->noteType==NOTETYPE_NORMAL)
				{
					if(fabs(ptr->restTime)<EPS)
					{
						GAMEDATA.ModifyHP(-gameini.HP_DEC), GAMEDATA.combo = 0, GAMEDATA.worst_cnt++, numberMana.SetCombo(0,ptr->note.x,ptr->note.y), gameui.SetState(WORST);
						GAMEDATA.maxScoreTempCombo++;
						GAMEDATA.maxScoreCanGet += gameini.SCORE_COOL + ((GAMEDATA.maxScoreTempCombo>50?50:GAMEDATA.maxScoreTempCombo)/10)*50;
						if(GAMEDATA.chanceTime)
						{
							GAMEDATA._chanceTimeCombo = 0;
							numberMana.SetExtraScore((GAMEDATA._chanceTimeCombo>50?50:GAMEDATA._chanceTimeCombo)*100,ptr->note.x,ptr->note.y);
							GAMEDATA.maxScoreTempChanceTimeCombo++;
							GAMEDATA.maxScoreCanGet += (GAMEDATA.maxScoreTempChanceTimeCombo>50?50:GAMEDATA.maxScoreTempChanceTimeCombo)*100;
						}
						ptr->Clear();
						unit.erase(ptr);
						findNew = true;
					}
					else if(GAMEDATA.gamemode==GAME_MODE_AUTO&&fabs(ptr->restTime-gameini.delay)<=gameini.eps)
					{
						switch(ptr->type)
						{
						case UNIT_TRIANGLE:
							OnKeyEvent(systemIni.GameKeyTriangle,true,false);
							break;
						case UNIT_CROSS:
							OnKeyEvent(systemIni.GameKeyCross,true,false);
							break;
						case UNIT_RECT:
							OnKeyEvent(systemIni.GameKeyRectangle,true,false);
							break;
						case UNIT_CIRCLE:
							OnKeyEvent(systemIni.GameKeyCircle,true,false);
							break;
						case UNIT_LEFT:
							OnKeyEvent(systemIni.GameKeyLeft,true,false);
							break;
						case UNIT_RIGHT:
							OnKeyEvent(systemIni.GameKeyRight,true,false);
							break;
						case UNIT_UP:
							OnKeyEvent(systemIni.GameKeyUp,true,false);
							break;
						case UNIT_DOWN:
							OnKeyEvent(systemIni.GameKeyDown,true,false);
							break;
						default:
							break;
						}
						findNew = true;
					}
					break;
				}
				else
				{
					if(ptr->finish())
					{
						GAMEDATA.ModifyHP(-gameini.HP_DEC), GAMEDATA.combo = 0, GAMEDATA.worst_cnt++, numberMana.SetCombo(0,ptr->note.x,ptr->note.y), gameui.SetState(WORST);
						GAMEDATA.maxScoreTempCombo++;
						GAMEDATA.maxScoreCanGet += gameini.SCORE_COOL + ((GAMEDATA.maxScoreTempCombo>50?50:GAMEDATA.maxScoreTempCombo)/10)*50;
						if(GAMEDATA.chanceTime)
						{
							GAMEDATA._chanceTimeCombo = 0;
							numberMana.SetExtraScore((GAMEDATA._chanceTimeCombo>50?50:GAMEDATA._chanceTimeCombo)*100,ptr->note.x,ptr->note.y);
							GAMEDATA.maxScoreTempChanceTimeCombo++;
							GAMEDATA.maxScoreCanGet += (GAMEDATA.maxScoreTempChanceTimeCombo>50?50:GAMEDATA.maxScoreTempChanceTimeCombo)*100;
						}
						ptr->Presh(WORST);
						ptr->Presh(WORST);
						//ptr->Clear();
						//unit.erase(ptr);
						findNew = true;
						break;
					}
					if(GAMEDATA.gamemode==GAME_MODE_AUTO)
					{
						if(!ptr->start&&fabs(ptr->restTime-gameini.delay)<gameini.eps)
						{
							switch(ptr->type)
							{
							case UNIT_TRIANGLE:
								OnKeyEvent(systemIni.GameKeyTriangle,true,false);
								break;
							case UNIT_CROSS:
								OnKeyEvent(systemIni.GameKeyCross,true,false);
								break;
							case UNIT_RECT:
								OnKeyEvent(systemIni.GameKeyRectangle,true,false);
								break;
							case UNIT_CIRCLE:
								OnKeyEvent(systemIni.GameKeyCircle,true,false);
								break;
							case UNIT_LEFT:
								OnKeyEvent(systemIni.GameKeyLeft,true,false);
								break;
							case UNIT_RIGHT:
								OnKeyEvent(systemIni.GameKeyRight,true,false);
								break;
							case UNIT_UP:
								OnKeyEvent(systemIni.GameKeyUp,true,false);
								break;
							case UNIT_DOWN:
								OnKeyEvent(systemIni.GameKeyDown,true,false);
								break;
							default:
								break;
							}
							findNew = true;
							break;
						}
						else if(ptr->start&&fabs(ptr->duration-gameini.delay)<gameini.eps)
						{
							switch(ptr->type)
							{
							case UNIT_TRIANGLE:
								OnKeyEvent(systemIni.GameKeyTriangle,false,false);
								break;
							case UNIT_CROSS:
								OnKeyEvent(systemIni.GameKeyCross,false,false);
								break;
							case UNIT_RECT:
								OnKeyEvent(systemIni.GameKeyRectangle,false,false);
								break;
							case UNIT_CIRCLE:
								OnKeyEvent(systemIni.GameKeyCircle,false,false);
								break;
							case UNIT_LEFT:
								OnKeyEvent(systemIni.GameKeyLeft,false,false);
								break;
							case UNIT_RIGHT:
								OnKeyEvent(systemIni.GameKeyRight,false,false);
								break;
							case UNIT_UP:
								OnKeyEvent(systemIni.GameKeyUp,false,false);
								break;
							case UNIT_DOWN:
								OnKeyEvent(systemIni.GameKeyDown,false,false);
								break;
							default:
								break;
							}
							findNew = true;
							break;
						}
					}
				}
			}
		}
	}
	//totalTime += dwTime;
	//singleTime = _notemap.Cal_Single(_notemap.GetBPM());
	//while(noteTime+singleTime<=totalTime) Run(_notemap.GetNotes()), noteTime+=singleTime,_notemap.NextNotes();
}

void NoteMana::OnKeyEvent(UINT nChar, bool bKeyDown, bool bAltDown)
{
	UNIT type;
	if(nChar==systemIni.GameKeyUp)
		type=UNIT_UP;
	else if(nChar==systemIni.GameKeyDown)
		type=UNIT_DOWN;
	else if(nChar==systemIni.GameKeyLeft)
		type=UNIT_LEFT;
	else if(nChar==systemIni.GameKeyRight)
		type=UNIT_RIGHT;
	else if(nChar==systemIni.GameKeyTriangle)
		type=UNIT_TRIANGLE;
	else if(nChar==systemIni.GameKeyCross)
		type=UNIT_CROSS;
	else if(nChar==systemIni.GameKeyRectangle)
		type=UNIT_RECT;
	else if(nChar==systemIni.GameKeyCircle)
		type=UNIT_CIRCLE;
	else
		return;

	if(bKeyDown)
	{

		soundEngine.PlayHit();
		vector<Unit>::iterator ptr = unit.begin();
		while(ptr!=unit.end()) 
		{ 
			if(ptr->restTime-gameini.delay>gameini.SAFE)
				{
					ptr = unit.end();
					break;
				}
			if(ptr->noteType==NOTETYPE_STRIP)
			{
				if(!ptr->start&&ptr->preshed==false&&ptr->type==type)
					break;
			}
			else
			{
				if(ptr->preshed==false&&ptr->type==type)
					break;
			}
			ptr++;
		}
		if(ptr==unit.end())
		{
			ptr = unit.begin();
			while(ptr!=unit.end())
			{
				if(ptr->preshed==false)
					break;
				ptr++;
			}
		}
		if(ptr!=unit.end())
		{
			double restTime = fabs(ptr->restTime-gameini.delay);
			if(ptr->noteType==NOTETYPE_STRIP)
			{

			}
			if(ptr->type==type)
			{
				if(restTime<=gameini.COOL)
				{
					effectMana.AddEffectResetFlame(ptr->type);
					GAMEDATA.ModifyHP(gameini.HP_ADD), GAMEDATA.cool_cnt++, GAMEDATA.combo++, GAMEDATA.score += gameini.SCORE_COOL + ((GAMEDATA.combo>50?50:GAMEDATA.combo)/10)*50, ptr->Presh(COOL), gameui.SetState(COOL);
					GAMEDATA.maxScoreTempCombo++;
					GAMEDATA.maxScoreCanGet += gameini.SCORE_COOL + ((GAMEDATA.maxScoreTempCombo>50?50:GAMEDATA.maxScoreTempCombo)/10)*50;
					if(GAMEDATA.chanceTime)
					{
						GAMEDATA._chanceTimeCombo++;
						GAMEDATA.score += (GAMEDATA._chanceTimeCombo>50?50:GAMEDATA._chanceTimeCombo)*100;
						GAMEDATA._chanceTimeScore += (GAMEDATA._chanceTimeCombo>50?50:GAMEDATA._chanceTimeCombo)*100;
						numberMana.SetExtraScore((GAMEDATA._chanceTimeCombo>50?50:GAMEDATA._chanceTimeCombo)*100,ptr->note.x,ptr->note.y);
						GAMEDATA.maxScoreTempChanceTimeCombo++;
						GAMEDATA.maxScoreCanGet += (GAMEDATA.maxScoreTempChanceTimeCombo>50?50:GAMEDATA.maxScoreTempChanceTimeCombo)*100;
					}
				}
				else if(restTime<=gameini.FINE)
				{
					effectMana.AddEffectResetFlame(ptr->type);
					GAMEDATA.ModifyHP(gameini.HP_ADD), GAMEDATA.fine_cnt++, GAMEDATA.combo++, GAMEDATA.score += gameini.SCORE_FINE + ((GAMEDATA.combo>50?50:GAMEDATA.combo)/10)*50, ptr->Presh(FINE), gameui.SetState(FINE);
					GAMEDATA.maxScoreTempCombo++;
					GAMEDATA.maxScoreCanGet += gameini.SCORE_COOL + ((GAMEDATA.maxScoreTempCombo>50?50:GAMEDATA.maxScoreTempCombo)/10)*50;
					if(GAMEDATA.chanceTime)
					{
						GAMEDATA._chanceTimeCombo++;
						GAMEDATA.score += (GAMEDATA._chanceTimeCombo>50?50:GAMEDATA._chanceTimeCombo)*100;
						GAMEDATA._chanceTimeScore += (GAMEDATA._chanceTimeCombo>50?50:GAMEDATA._chanceTimeCombo)*100;
						numberMana.SetExtraScore((GAMEDATA._chanceTimeCombo>50?50:GAMEDATA._chanceTimeCombo)*100,ptr->note.x,ptr->note.y);
						GAMEDATA.maxScoreTempChanceTimeCombo++;
						GAMEDATA.maxScoreCanGet += (GAMEDATA.maxScoreTempChanceTimeCombo>50?50:GAMEDATA.maxScoreTempChanceTimeCombo)*100;
					}
				}
				else if(restTime<=gameini.SAFE)
				{
					effectMana.AddEffectResetFlame(ptr->type);
					GAMEDATA.ModifyHP(gameini.HP_ADD), GAMEDATA.score += gameini.SCORE_SAFE, GAMEDATA.safe_cnt++, GAMEDATA.combo=0, ptr->Presh(SAFE), gameui.SetState(SAFE);
					GAMEDATA.maxScoreTempCombo++;
					GAMEDATA.maxScoreCanGet += gameini.SCORE_COOL + ((GAMEDATA.maxScoreTempCombo>50?50:GAMEDATA.maxScoreTempCombo)/10)*50;
					if(GAMEDATA.chanceTime)
					{
						GAMEDATA._chanceTimeCombo=0;
						numberMana.SetExtraScore((GAMEDATA._chanceTimeCombo>50?50:GAMEDATA._chanceTimeCombo)*100,ptr->note.x,ptr->note.y);
						GAMEDATA.maxScoreTempChanceTimeCombo++;
						GAMEDATA.maxScoreCanGet += (GAMEDATA.maxScoreTempChanceTimeCombo>50?50:GAMEDATA.maxScoreTempChanceTimeCombo)*100;
					}
				}
//				else
//					GAMEDATA.ModifyHP(-gameini.HP_DEC), GAMEDATA.combo = 0, numberMana.SetCombo(0,ptr->note.x,ptr->note.y), gameui.SetState(WORST);
				GAMEDATA.maxcombo = max(GAMEDATA.maxcombo,GAMEDATA.combo);
			}
			else
			{
				GAMEDATA.combo = 0, numberMana.SetCombo(0,ptr->note.x,ptr->note.y);
				GAMEDATA.maxScoreTempCombo++;
				GAMEDATA.maxScoreCanGet += gameini.SCORE_COOL + ((GAMEDATA.maxScoreTempCombo>50?50:GAMEDATA.maxScoreTempCombo)/10)*50;
				if(GAMEDATA.chanceTime)
				{
					GAMEDATA._chanceTimeCombo=0;
					numberMana.SetExtraScore((GAMEDATA._chanceTimeCombo>50?50:GAMEDATA._chanceTimeCombo)*100,ptr->note.x,ptr->note.y);
					GAMEDATA.maxScoreTempChanceTimeCombo++;
					GAMEDATA.maxScoreCanGet += (GAMEDATA.maxScoreTempChanceTimeCombo>50?50:GAMEDATA.maxScoreTempChanceTimeCombo)*100;
				}
				gameui.SetState(WORST);
				GAMEDATA.ModifyHP(-gameini.HP_DEC);
				//if(restTime<=gameini.COOL)
				//	GAMEDATA.ModifyHP(gameini.HP_ADD), GAMEDATA.score += gameini.SCORE_COOL>>1, GAMEDATA.cool_cnt++, ptr->Presh(COOL), gameui.SetState(_COOL);
				//else if(restTime<=gameini.FINE)
				//	GAMEDATA.ModifyHP(gameini.HP_ADD), GAMEDATA.score += gameini.SCORE_FINE>>1, GAMEDATA.fine_cnt++, ptr->Presh(FINE), gameui.SetState(_FINE);
				//else if(restTime<=gameini.SAFE)
				//	GAMEDATA.ModifyHP(gameini.HP_ADD), GAMEDATA.score += gameini.SCORE_SAFE>>1, GAMEDATA.safe_cnt++, ptr->Presh(SAFE), gameui.SetState(_SAFE);
//				else
//					GAMEDATA.ModifyHP(gameini.HP_ADD), gameui.SetState(WORST);
			}
		}
	}
	else
	{
		vector<Unit>::iterator ptr = unit.begin();

		while(ptr!=unit.end())
		{
			if(ptr->noteType==NOTETYPE_STRIP&&ptr->type==type&&ptr->start&&!ptr->preshed)
				break;
			ptr++;
		}
		if(ptr!=unit.end())
		{
			double restTime = fabs(ptr->duration-gameini.delay);
			soundEngine.PlayHit();
			if(restTime<=gameini.COOL)
			{
				GAMEDATA.ModifyHP(gameini.HP_ADD), GAMEDATA.cool_cnt++, GAMEDATA.combo++, GAMEDATA.score += gameini.SCORE_COOL + ((GAMEDATA.combo>50?50:GAMEDATA.combo)/10)*50, ptr->Presh(COOL), gameui.SetState(COOL);
				GAMEDATA.maxScoreTempCombo++;
				GAMEDATA.maxScoreCanGet += gameini.SCORE_COOL + ((GAMEDATA.maxScoreTempCombo>50?50:GAMEDATA.maxScoreTempCombo)/10)*50;
				if(GAMEDATA.chanceTime)
				{
					GAMEDATA._chanceTimeCombo++;
					GAMEDATA.score += (GAMEDATA._chanceTimeCombo>50?50:GAMEDATA._chanceTimeCombo)*100;
					GAMEDATA._chanceTimeScore += (GAMEDATA._chanceTimeCombo>50?50:GAMEDATA._chanceTimeCombo)*100;
					numberMana.SetExtraScore((GAMEDATA._chanceTimeCombo>50?50:GAMEDATA._chanceTimeCombo)*100,ptr->note.x,ptr->note.y);
					GAMEDATA.maxScoreTempChanceTimeCombo++;
					GAMEDATA.maxScoreCanGet += (GAMEDATA.maxScoreTempChanceTimeCombo>50?50:GAMEDATA.maxScoreTempChanceTimeCombo)*100;
				}
			}
			else if(restTime<=gameini.FINE)
			{
				GAMEDATA.ModifyHP(gameini.HP_ADD), GAMEDATA.fine_cnt++, GAMEDATA.combo++, GAMEDATA.score += gameini.SCORE_FINE + ((GAMEDATA.combo>50?50:GAMEDATA.combo)/10)*50, ptr->Presh(FINE), gameui.SetState(FINE);
				GAMEDATA.maxScoreTempCombo++;
				GAMEDATA.maxScoreCanGet += gameini.SCORE_COOL + ((GAMEDATA.maxScoreTempCombo>50?50:GAMEDATA.maxScoreTempCombo)/10)*50;
				if(GAMEDATA.chanceTime)
				{
					GAMEDATA._chanceTimeCombo++;
					GAMEDATA.score += (GAMEDATA._chanceTimeCombo>50?50:GAMEDATA._chanceTimeCombo)*100;
					GAMEDATA._chanceTimeScore += (GAMEDATA._chanceTimeCombo>50?50:GAMEDATA._chanceTimeCombo)*100;
					numberMana.SetExtraScore((GAMEDATA._chanceTimeCombo>50?50:GAMEDATA._chanceTimeCombo)*100,ptr->note.x,ptr->note.y);
					GAMEDATA.maxScoreTempChanceTimeCombo++;
					GAMEDATA.maxScoreCanGet += (GAMEDATA.maxScoreTempChanceTimeCombo>50?50:GAMEDATA.maxScoreTempChanceTimeCombo)*100;
				}
			}
			else if(restTime<=gameini.SAFE)
			{
				GAMEDATA.ModifyHP(gameini.HP_ADD), GAMEDATA.score += gameini.SCORE_SAFE, GAMEDATA.safe_cnt++, GAMEDATA.combo=0, ptr->Presh(SAFE), gameui.SetState(SAFE);
				GAMEDATA.maxScoreTempCombo++;
				GAMEDATA.maxScoreCanGet += gameini.SCORE_COOL + ((GAMEDATA.maxScoreTempCombo>50?50:GAMEDATA.maxScoreTempCombo)/10)*50;
				if(GAMEDATA.chanceTime)
				{
					GAMEDATA._chanceTimeCombo=0;
					numberMana.SetExtraScore((GAMEDATA._chanceTimeCombo>50?50:GAMEDATA._chanceTimeCombo)*100,ptr->note.x,ptr->note.y);
					GAMEDATA.maxScoreTempChanceTimeCombo++;
					GAMEDATA.maxScoreCanGet += (GAMEDATA.maxScoreTempChanceTimeCombo>50?50:GAMEDATA.maxScoreTempChanceTimeCombo)*100;
				}
			}
			else
			{
				GAMEDATA.ModifyHP(-gameini.HP_DEC), GAMEDATA.combo = 0, GAMEDATA.worst_cnt++, numberMana.SetCombo(0,ptr->note.x,ptr->note.y),ptr->Presh(WORST),gameui.SetState(WORST);
				GAMEDATA.maxScoreTempCombo++;
				GAMEDATA.maxScoreCanGet += gameini.SCORE_COOL + ((GAMEDATA.maxScoreTempCombo>50?50:GAMEDATA.maxScoreTempCombo)/10)*50;
				if(GAMEDATA.chanceTime)
				{
					GAMEDATA._chanceTimeCombo = 0;
					numberMana.SetExtraScore((GAMEDATA._chanceTimeCombo>50?50:GAMEDATA._chanceTimeCombo)*100,ptr->note.x,ptr->note.y);
					GAMEDATA.maxScoreTempChanceTimeCombo++;
					GAMEDATA.maxScoreCanGet += (GAMEDATA.maxScoreTempChanceTimeCombo>50?50:GAMEDATA.maxScoreTempChanceTimeCombo)*100;
				}
			}
			GAMEDATA.maxcombo = max(GAMEDATA.maxcombo,GAMEDATA.combo);
		}
	}
}
//------------------------------------------------------------------------------------

void GameUI::Init()
{
	state = NOSTATE;
	uistate = NORMAL;
	ui = graphEngine.AddTexture("pic\\ui.png");
	top_bar = MakeRect(0,0,480,22);
	bottom_bar = MakeRect(0,23,480,54);
	top_bar_red = MakeRect(0,193,480,22);
	bottom_bar_red = MakeRect(0,216,480,54);
}

void GameUI::SetNew()
{
	state = NOSTATE;
	uistate = NORMAL;
	delayTime = uidelay = 0;
	deltaTop = (top_bar.bottom-top_bar.top);
	deltaBottom = (bottom_bar.bottom-bottom_bar.top);
	MoveIn(30);
}

void GameUI::Update(double dwTime)
{
	black_top->Update();
	black_bottom->Update();
	if(GAMEDATA.chanceTime)
		chanceTimeLabel->Update();
	if(fabs(delayTime)>EPS)
	{
		delayTime -= dwTime;
		if(delayTime<0)
			delayTime = 0;
	}
	else
		state = NOSTATE;
	if(fabs(uidelay)>EPS)
	{
		uidelay -= dwTime;
		if(uidelay<0)
			uidelay = 0;
	}
	else
		uistate = NORMAL;
	spark++;

	nowTime++;
	if(moveState==1)
	{
		deltaTop = (top_bar.bottom-top_bar.top)*double(moveTime-nowTime)/moveTime;
		deltaBottom = (bottom_bar.bottom-bottom_bar.top)*double(moveTime-nowTime)/moveTime;
	}
	else if(moveState==2)
	{
		deltaTop = (top_bar.bottom-top_bar.top)*double(nowTime)/moveTime;
		deltaBottom = (bottom_bar.bottom-bottom_bar.top)*double(nowTime)/moveTime;
	}
	if(nowTime==moveTime)
		moveState = 0;
}

void GameUI::Draw()
{
	//hp
	int hpnum = ((GAMEDATA.hp+gameini.SINGLE_HP-1)/gameini.SINGLE_HP)-1;

	//bar
	if(hpnum<=3&&((spark>>5)&1))
	{
		graphEngine.Draw(ui,top_bar_red,0,-deltaTop);
		graphEngine.Draw(ui,bottom_bar_red,0,base::Height-bottom_bar.bottom+bottom_bar.top+deltaBottom);
	}
	else
	{
		graphEngine.Draw(ui,top_bar,0,-deltaTop);
		graphEngine.Draw(ui,bottom_bar,0,base::Height-bottom_bar.bottom+bottom_bar.top+deltaBottom);
	}

	RECT hp_rect,sign_rect;
	if(hpnum>=0&&hpnum<3&&((spark>>5)&1))
	{
		hp_rect = MakeRect(152+hpnum*38,79+38,38,38);
		sign_rect = MakeRect(116,271,36,36);
	}
	else if(hpnum>=0&&hpnum<13)
	{
	    hp_rect = MakeRect(hpnum*38,79,38,38);
		sign_rect = MakeRect(40,271,36,36);
	}
	else if(hpnum>=13)
	{
		hp_rect = MakeRect((hpnum-13)*38,79+38,38,38);
		sign_rect = MakeRect(40,271,36,36);
	}
	if(uistate==ADD)
		sign_rect = MakeRect(spark/5%5*38,155,38,38);
	else if(uistate==DEC)
		sign_rect = MakeRect(76,271,36,36);
	else if(hpnum<14)
		sign_rect = MakeRect(0,0,0,0);

	if(hpnum>=0)
	{
		graphEngine.Draw(ui,hp_rect,9,base::Height-bottom_bar.bottom+bottom_bar.top+11+deltaBottom);
		switch(uistate)
		{
		case NORMAL:
			graphEngine.Draw(ui,sign_rect,11,base::Height-bottom_bar.bottom+bottom_bar.top+10+deltaBottom);
			break;
		case ADD:
			graphEngine.Draw(ui,sign_rect,9,base::Height-bottom_bar.bottom+bottom_bar.top+10+deltaBottom);
			break;
		case DEC:
			graphEngine.Draw(ui,sign_rect,8,base::Height-bottom_bar.bottom+bottom_bar.top+10+deltaBottom);
			break;
		}
	}

	//state
	if(state!=NOSTATE)
		graphEngine.Draw(ui,GAMEDATA.state_rect[state],base::Width-90,base::Height-20+deltaBottom);

	
	numberMana.DrawScore(GAMEDATA.score);

	black_top->Draw();
	black_bottom->Draw();

	if(GAMEDATA.chanceTime)
		chanceTimeLabel->Draw();

	graphEngine.Draw(ui,MakeRect(491,140,11,11),MakeRect(5,3,11,11));
	graphEngine.SetFont("ËÎÌו",12);
	graphEngine.DrawText(_notemap.GetMapName()+" - "+HardType[_notemap.GetLevel()-1],22,3);
}

void GameUI::SetState(PLAYSTATE _state)
{
	state = _state;
	if(state!=NOSTATE)
		delayTime = gameini.playstate_delay;
}

void GameUI::SetUIState(UISTATE _state)
{
	uistate = _state;
	if(uistate!=NORMAL)
		uidelay = gameini.uistate_delay;
}

void GameUI::MoveAway(int time)
{
	moveState = 2;
	moveTime = time, nowTime = 0;
}

void GameUI::MoveIn(int time)
{
	moveState = 1;
	moveTime = time, nowTime = 0;
}

//------------------------------------------------------------------------------------
void GameCore::Init()
{
	graphEngine.InitEngine(base::Device,base::Sprite);
	videoEngine.Init();
	soundEngine.Init();
	GAMEDATA.Init();
	gameui.Init();
	gameini.Init();
	_notemana.Init();
	effectMana.Init();

	white = new GUIStaticImage("pic\\block.png",MakeRect(0,0,8,8),MakeRect(0,0,base::Width,base::Height));
	black_top = new GUIStaticImage("pic\\black.png",MakeRect(0,0,8,8),MakeRect(0,-(gameui.top_bar.bottom-gameui.top_bar.top),gameui.top_bar.right-gameui.top_bar.left,gameui.top_bar.bottom-gameui.top_bar.top));
	(*black_top->efvct.a)=128;
	black_bottom = new GUIStaticImage("pic\\black.png",MakeRect(0,0,8,8),MakeRect(0,base::Height,gameui.top_bar.right-gameui.top_bar.left,gameui.top_bar.bottom-gameui.top_bar.top));
	(*black_bottom->efvct.a)=128;
	chanceTimeLabel = new GUIStaticImage("pic\\num.png",MakeRect(201,3,73,14),MakeRect(3,253,73,14));
	chanceTimeLabel->efvct.AddEffect(new Spark(chanceTimeLabel->efvct.a,3,true,-1,255,255,128,255));
}

GameCore::~GameCore()
{
	SAFE_DELETE(menu);
	SAFE_DELETE(white);
	SAFE_DELETE(black_top);
	SAFE_DELETE(black_bottom);
	graphEngine.ReleaseTex();
}

void GameCore::SetNew(string filename,string baseDirec,int gamemode)
{
	GAMEDATA.SetNew();
	gameui.SetNew();
	GAMEDATA.gamemode = gamemode;
	videoEngine.SetNew();
	soundEngine.ClearChannel();
	soundEngine.ClearPool();
	_notemap.SetNew(filename,baseDirec);
	singleTime = _notemap.Cal_Single(_notemap.GetBPM());
	nowDistance = gameini.DISTANCE*gameini.BPM/_notemap.GetBPM();
	noteTime = 0, totalTime = 0;
	_notemana.SetNew();
	_notemap.Resource_read();
	effectMana.SetNew();
	finish = paused = false, visible = true;
	(*black_top->efvct.drawRect)=MakeRect(0,-(gameui.top_bar.bottom-gameui.top_bar.top),gameui.top_bar.right-gameui.top_bar.left,gameui.top_bar.bottom-gameui.top_bar.top);
	(*black_bottom->efvct.drawRect)=MakeRect(0,base::Height,gameui.top_bar.right-gameui.top_bar.left,gameui.top_bar.bottom-gameui.top_bar.top);
}

void GameCore::Run(LpFrame _nowFrame)
{
	if(_nowFrame==NULL)
		return;
	if(_nowFrame->_IsSetBPM)
	{
		singleTime = _notemap.Cal_Single(_nowFrame->_BPM);
		nowDistance = gameini.DISTANCE*gameini.BPM/_nowFrame->_BPM;
	}
	string filename;
	if(_nowFrame->_resource!=NOSOURCE)
	{
		filename = _notemap.GetResource(_nowFrame->_resource);
		if(filename!="")
		{
			if(CheckResource(filename)==GRAPH)
				GAMEDATA.background = graphEngine.AddTexture(filename,GameCoreRes);
			else
				videoEngine.Play(filename,0,0);
		}
	}
	for(int i = 0; i < BGSNum; i++)
		if(_nowFrame->_BGM[i]!=NOWAV)
		{
			filename = _notemap.GetWav(_nowFrame->_BGM[i]);
			if(filename!="")
				soundEngine.PlayMusic(filename);
		}
	if(!GAMEDATA.chanceTime&&_nowFrame->notePos==_notemap._chanceTimeStart)
	{
		GAMEDATA.chanceTime = true;
		(*black_top->efvct.drawRect) = MakeRect(0,0,480,0);
		(*black_bottom->efvct.drawRect) = MakeRect(0,272,480,0);
		black_top->efvct.AddEffect(new GUIEF_Stretch(black_top->efvct.drawRect,20,MakeRect(0,0,gameui.top_bar.right-gameui.top_bar.left,gameui.top_bar.bottom-gameui.top_bar.top),0));
		black_bottom->efvct.AddEffect(new GUIEF_Stretch(black_bottom->efvct.drawRect,20,MakeRect(0,base::Height-gameui.top_bar.bottom+gameui.top_bar.top,gameui.top_bar.right-gameui.top_bar.left,gameui.top_bar.bottom-gameui.top_bar.top),0));
		effectMana.AddEffectChanceTimeStart();
		gameui.MoveAway(30);
	}
	if(GAMEDATA.chanceTime&&_nowFrame->notePos==_notemap._chanceTimeEnd+1)
	{
		GAMEDATA.chanceTime = false;
		black_top->efvct.ReleaseEffects();
		black_bottom->efvct.ReleaseEffects();
		black_top->efvct.AddEffect(new GUIEF_Stretch(black_top->efvct.drawRect,20,MakeRect(0,-(gameui.top_bar.bottom-gameui.top_bar.top),gameui.top_bar.right-gameui.top_bar.left,gameui.top_bar.bottom-gameui.top_bar.top),0));
		black_bottom->efvct.AddEffect(new GUIEF_Stretch(black_bottom->efvct.drawRect,20,MakeRect(0,base::Height,gameui.top_bar.right-gameui.top_bar.left,gameui.top_bar.bottom-gameui.top_bar.top),0));
		effectMana.AddEffectChanceTimeEnd(GAMEDATA._chanceTimeScore);
		gameui.MoveIn(30);
	}
}

void GameCore::Update(double dwTime)
{
	soundEngine.Update();
	//GAMEDATA.gamemode = 2;
	if(!paused)
	{
		videoEngine.Update();
		//if(GAMEDATA.gamemode!=GAME_MODE_PV)
			_notemana.Update(dwTime);
		if(GAMEDATA.gamemode!=GAME_MODE_PV)
			gameui.Update(dwTime);
		EffectMana::Instance().Update();
		pSystem.Update();

		totalTime += dwTime;

		while(noteTime+singleTime<=totalTime) Run(_notemap.GetFrame()),noteTime += singleTime,_notemap.NextFrame();

		nowFrame = _notemap.GetFrame();

		if(GAMEDATA.hp<=0||nowFrame==NULL)
			Finish(), ifclear = (GAMEDATA.hp>0);

		_notemana.Add(_notemap.GetFramePos()+gameini.note_standing);
	}
	else
		menu->Update();
	if(finish)
	{
		white->Update();
		if((*white->efvct.a)==255) 
		{
			soundEngine.ClearChannel();
			soundEngine.ClearPool();
			pSystem.Clear();
			BackToUIScreen(GAMEDATA.cool_cnt,GAMEDATA.fine_cnt,GAMEDATA.safe_cnt,0,GAMEDATA.worst_cnt,GAMEDATA.maxcombo,GAMEDATA._chanceTimeScore,GAMEDATA.score,ifclear,GAMEDATA.maxScoreCanGet);
		}
	}
}

void GameCore::Draw(IDirect3DDevice9 *Device)
{
	if(!visible)
		return;
	if(GAMEDATA.background)
		graphEngine.Draw(GAMEDATA.background,0,0);
	videoEngine.Draw();
	if(GAMEDATA.gamemode!=GAME_MODE_PV)
	{
		pSystem.Draw();
		_notemana.Draw();
		EffectMana::Instance().Draw();
	}
	if(GAMEDATA.gamemode!=GAME_MODE_PV)
		gameui.Draw();
	if(paused)
		menu->Draw();
	if(finish)
		white->Draw();
}

void GameCore::OnKeyEvent(UINT nChar, bool bKeyDown, bool bAltDown)
{
	if(GAMEDATA.gamemode==GAME_MODE_NORMAL)
		if(!paused)
			_notemana.OnKeyEvent(nChar,bKeyDown,bAltDown);
	if(paused)
		menu->OnKeyEvent(nChar,bKeyDown,bAltDown);
	if(bKeyDown)
	{
		if(!paused&&(nChar==KeyEnter||nChar==KeyEsc))
		{
		    PauseGame(true);
			menu->ShowMenu();
		}
	}
}

void GameCore::PauseGame(bool _paused)
{
	paused = _paused;
	soundEngine.PauseAllMusic(paused);
	videoEngine.Paused(paused);
}

void GameCore::Clear()
{
	_notemap.Clear();
	paused = true, visible = false;
	soundEngine.StopAllMusic();
	videoEngine.Clear();
	soundEngine.ClearChannel();
	soundEngine.ClearPool();
	pSystem.Clear();
}

void GameCore::Finish()
{
	finish = paused = true;
	soundEngine.StopAllMusic();
	videoEngine.Clear();
	white->SetColor(0,255,255,255);
	white->efvct.AddEffect(new GUIEF_Alpha(white->efvct.a,25,255));
}