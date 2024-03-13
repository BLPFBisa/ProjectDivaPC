#include <Math.h>
#include "GameMana.h"
#include "EffectMana.h"
#include "Gui.h"
#include "UIScreen.h"

GameUI gameui;

float SINGLE_LINE_INC;
float DOUBLE_LINE_INC;
float DOUBLE_LINE_TWIST_FACTOR;
float DOUBLE_LINE_SPACE_FACTOR;
float COMET_LINE_INC;
float STRIP_NOTE_SIZE;
float NOTE_SIZE;
float META_SIZE;
float ARROW_HEAD;

int nowDistance;
int *spin_angle_inc;
GUIStaticImage *white;
GUIStaticImage *black_top,*black_bottom,*chanceTimeLabel;
void (Unit::*UnitDrawLine)(const Point &a, const Point &b);
GameData GAMEDATA;

void GameData::Init()
{
	//note_pic = graphEngine.AddTexture("pic\\note.png");
	background = NULL;

	// load notes_new.png
	struct toLoad{
		Resource** ppRes;
		string res_name;
	};
	const int toloads_N=8;
	static toLoad toloads[toloads_N]={
		{(Resource**)&pointer_rc,		"note_pointer"			},
		{(Resource**)&note_rc8,			"note_rect[8]"			},
		{(Resource**)&rhythm_rc8,		"rhythm_rect[8]"		},
		{(Resource**)&note_strip_rc8,	"note_rect_strip[8]"	},
		{(Resource**)&rhythm_strip_rc8,	"rhythm_rect_strip[8]"	},
		{(Resource**)&bar_in_rc8,		"bar_in[8]"				},
		{(Resource**)&bar_out_rc,		"bar_out"				},
		{(Resource**)&presh_rc8,		"presh_rect[8]"			},
	};
	for(int x=0; x<toloads_N; x++)
	{
		*toloads[x].ppRes = g_res.getResource(toloads[x].res_name);
	}

	// ui.png
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
	background = NULL;//graphEngine.AddTexture("pic\\black.png");
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

static int ser=0;
Unit::Unit(Note _note,double standing, int _notePos)
{
	serial = ser++;
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
	angle = 0, preshed = false, start = false, first = true;
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
/*
void Unit::AddMetafile(float x, float y, D3DCOLOR color, int flag)
{
	RECT rect;
	float meta_size = (flag==0)?META_SIZE:META_SIZE*0.9f;
	graphEngine.Draw(GAMEDATA.note_pic,(flag==0)?GAMEDATA.bar_out_rect:GAMEDATA.bar_in_rects[type],
		MakefRect(x-meta_size*0.5f,y-meta_size*0.5f,meta_size,meta_size),color);
}
/*/
void Unit::AddMetafile(float x, float y, D3DCOLOR color, int flag)
{
	RECT rect;
	if(flag==0)
	{
		graphEngine.Draw(GAMEDATA.bar_out_rc,x-8.0f,y-8.0f);
	}
	else if(flag==1)
	{
		graphEngine.Draw(&GAMEDATA.bar_in_rc8[type],x-7.3f,y-6.5f);
	}
}

void Unit::AddParticle(float x, float y, float size)
{
	pSystem.AddParticle(
		new ParticleTail(x,y,128,UNIT_COLOR[type],particle_square,size),
		notePos*4+type
	);
}

void Unit::AddCometParticle(float x, float y, float dx, float dy)
{
	// Random color interpolated with Unit color
	int rgb[3]={rand()&0xff,rand()&0xff,rand()&0xff}, *unit_color = UNIT_COLOR[type];
	//if(serial&1){
	if(0){
		int maxv = max(rgb[0],rgb[1]);
		maxv = max(maxv,rgb[2]);
		float r = (float)256/(maxv+1);
		rgb[0] = (rgb[0]+1)*r;
		rgb[1] = (rgb[1]+1)*r;
		rgb[2] = (rgb[2]+1)*r;
	}
	rgb[0] = (rgb[0]+unit_color[0])>>1;
	rgb[1] = (rgb[1]+unit_color[1])>>1;
	rgb[2] = (rgb[2]+unit_color[2])>>1;

	// Adjust disperse density
	SetLength(dx,dy,sqrt((float)rand()/RAND_MAX));
	//SetLength(dx,dy,(float)rand()/RAND_MAX);

	// Add particle
	//if(serial&1)
	if(GAMEDATA.chanceTime)
		pSystem.AddParticle((ParticleBase*)
			(new ParticleComet(	x,y,256,rgb,particle_star2,dx,dy,
								(float)rand()*(7.0f/RAND_MAX),type,0x600f,GAMEDATA.hp,0,0)),notePos*4+type);
	else
		pSystem.AddParticle(
			new ParticleComet(	x,y,256,rgb,particle_sphere,dx,dy,(float)rand()*(7.5f/RAND_MAX),
								type,0x200f,GAMEDATA.hp,0,0),
			notePos*4+type
		);
}

void Unit::AddStarParticle(float x, float y, float size, D3DCOLOR color)
{
	// Random color interpolated with Unit color
	int rgb[3]={rand()&0xff,rand()&0xff,rand()&0xff}, *unit_color = UNIT_COLOR[type];
	rgb[0] = (rgb[0]+unit_color[0])>>1;
	rgb[1] = (rgb[1]+unit_color[1])>>1;
	rgb[2] = (rgb[2]+unit_color[2])>>1;

	// Add particle
	pSystem.AddParticle(
		new ParticleComet(x,y,256,rgb,particle_star,0,0,size,type,(GAMEDATA.chanceTime)?0x2007:0x2002,GAMEDATA.hp,0,0),
		notePos*4+type
	);
}

void Unit::AddSpinParticle(short angle)
{
	pSystem.AddParticle(
		new ParticleComet(0,0,256,UNIT_COLOR[type],particle_5star,note.x,note.y,10,type,0x0039,GAMEDATA.hp,0,angle),
		0
	);
}

Point Unit::Bezier(const Point &p0, const Point &p1, const Point &p2, const float &t)
{
	float tt = 1-t;
	return p0*tt*tt+p1*2*t*tt+p2*t*t;
}

Point Unit::Bezier(const Point &p0, const Point &p1, const Point &p2, const Point &p3, const float &t)
{
	float tt = 1-t;
	return p0*tt*tt*tt+p1*3*t*tt*tt+p2*3*t*t*tt+p3*t*t*t;
}

void Unit::DrawSingleLine(const Point &a, const Point &b)
{
	float	move_dir_x=b.x-a.x, move_dir_y=b.y-a.y;
	float	D = sqrt(move_dir_x*move_dir_x+move_dir_y*move_dir_y);
	if(D==0) return;
	float	x = a.x, y = a.y;
	float	x_inc = move_dir_x*SINGLE_LINE_INC/D;
	float	y_inc = move_dir_y*SINGLE_LINE_INC/D;

	for(int i=D/SINGLE_LINE_INC; i>=0; i--)
	{
		if(InsideDrawRange(x,y))
			AddParticle(x,y,4);
		x += x_inc;
		y += y_inc;
	}
}

void Unit::DrawDoubleLine(const Point &a, const Point &b)
{
	float	move_dir_x=b.x-a.x, move_dir_y=b.y-a.y;
	float	D = UnitLength(move_dir_x,move_dir_y);
	if(D==0) return;

	float	x = a.x, y = a.y;
	float	x_inc = move_dir_x*DOUBLE_LINE_INC;
	float	y_inc = move_dir_y*DOUBLE_LINE_INC;
	float	vert_dir_x = move_dir_y;
	float	vert_dir_y = -move_dir_x;
	float	_length_posi = length_drawn;

	for(int i=D/DOUBLE_LINE_INC; i>=0; i--)
	{
		float X,Y;
		{// draw 1st point
			X = x + vert_dir_x*DOUBLE_LINE_SPACE_FACTOR*cos(_length_posi*DOUBLE_LINE_TWIST_FACTOR);
			Y = y + vert_dir_y*DOUBLE_LINE_SPACE_FACTOR*sin(_length_posi*DOUBLE_LINE_TWIST_FACTOR);
			if(InsideDrawRange(X,Y))
				AddParticle(X,Y,1.6f);
		}
		{// draw 2nd point
			X = x - vert_dir_x*DOUBLE_LINE_SPACE_FACTOR*cos(_length_posi*DOUBLE_LINE_TWIST_FACTOR);
			Y = y - vert_dir_y*DOUBLE_LINE_SPACE_FACTOR*sin(_length_posi*DOUBLE_LINE_TWIST_FACTOR);
			if(InsideDrawRange(X,Y))
				AddParticle(X,Y,1.6f);
		}
		x += x_inc;
		y += y_inc;
		_length_posi += DOUBLE_LINE_INC;
	}
	length_drawn += D;
}

void Unit::DrawCometLine(const Point &a, const Point &b)
{
	float	move_dir_x=b.x-a.x, move_dir_y=b.y-a.y;
	float	D = UnitLength(move_dir_x,move_dir_y);
	if(D==0) return;

	float	x = a.x, y = a.y;
	float	x_inc = move_dir_x*COMET_LINE_INC;
	float	y_inc = move_dir_y*COMET_LINE_INC;

	for(int i=D/COMET_LINE_INC; i>=0; i--)
	{
		float	x_dir, y_dir;
		RandomDir(x_dir,y_dir);
		if(InsideDrawRange(x,y))
			AddCometParticle(x,y,x_dir-move_dir_x,y_dir-move_dir_y);
		x += x_inc;
		y += y_inc;
	}
}

void Unit::DrawBar(const Point &a, const Point &b, D3DCOLOR color, int flag)
{
	float	move_dir_x=b.x-a.x, move_dir_y=b.y-a.y;
	float	D = sqrt(move_dir_x*move_dir_x+move_dir_y*move_dir_y);
	if(D==0) return;
	float	x = a.x, y = a.y;
	float	x_inc = move_dir_x*SINGLE_LINE_INC/D;
	float	y_inc = move_dir_y*SINGLE_LINE_INC/D;

	for(float  i = 0; i < D; i+=SINGLE_LINE_INC)
	{
		if(InsideDrawRangeEx(x,y))
			AddMetafile(x,y,color,flag);
		x += x_inc;
		y += y_inc;
	}
}

void Unit::Draw_Normal()
{
	double realTime = (restTime-gameini.delay)<0?0:(restTime-gameini.delay);

	//draw stationary note
	D3DXMATRIX mat,mat2,ptransform;
	base::Sprite->GetTransform(&ptransform);
	D3DXMatrixTransformation2D(&mat, NULL, 0.0f, NULL, &base::MakeDrawRect(D3DXVECTOR2(note.x,note.y)), angle, NULL);
	if(realTime<totalTime*0.875)	// normal size
	{
		graphEngine.Draw(	&GAMEDATA.note_rc8[type],
							MakefRect(note.x-NOTE_SIZE*0.5f,note.y-NOTE_SIZE*0.5f,NOTE_SIZE,NOTE_SIZE)
							/*,D3DCOLOR_ARGB(int((restTime/gameini.standing)*255),255,255,255)*/);
		base::Sprite->SetTransform(&mat);
		graphEngine.Draw(	GAMEDATA.pointer_rc,
							MakefRect(note.x-NOTE_SIZE*0.5f,note.y-NOTE_SIZE*(0.5f+ARROW_HEAD),NOTE_SIZE,NOTE_SIZE));
	}
	else							// just appear, enlarge size
	{
		float percent = 1+(gameini.NOTE_BLOWUP-1)*((realTime-totalTime*0.875)/(totalTime*0.125));
		float note_size = NOTE_SIZE*percent;
		graphEngine.Draw(	&GAMEDATA.note_rc8[type],
			MakefRect(note.x-note_size*0.5f,note.y-note_size*0.5f,note_size,note_size)
			/*,D3DCOLOR_ARGB(int((restTime/gameini.standing)*255),255,255,255)*/);
		base::Sprite->SetTransform(&mat);
		graphEngine.Draw(	GAMEDATA.pointer_rc,
			MakefRect(note.x-note_size*0.5f,note.y-note_size*(0.5f+ARROW_HEAD),note_size,note_size)
			/*,D3DCOLOR_ARGB(int((restTime/gameini.standing)*255),255,255,255)*/);
	}
	base::Sprite->SetTransform(&ptransform);

	//draw rhythm, flying note
	double percent = (restTime-gameini.delay)/totalTime;
	Point _rhythm;

	if(restTime<gameini.delay)	// shrink notes
	{
		_rhythm = note+(rhythm-note).unit()*(nowDistance*2/3);
		bezier_p1 = note+(_rhythm-note)*0.25+(_rhythm-note).normal()*0.15;
		bezier_p2 = note+(_rhythm-note)*0.75-(_rhythm-note).normal()*0.15;
		_rhythm = Bezier(note,bezier_p1,bezier_p2,_rhythm,percent);
		percent = restTime/gameini.delay;
		float note_size = NOTE_SIZE*percent;
		graphEngine.Draw( preshed?&GAMEDATA.presh_rc8[type]:&GAMEDATA.rhythm_rc8[type],
			MakefRect(_rhythm.x-note_size*0.5f,_rhythm.y-note_size*0.5f,note_size,note_size));
	}
	else
	{
		_rhythm = note+(rhythm-note).unit()*(nowDistance);
		bezier_p1 = note+(_rhythm-note)*0.25+(_rhythm-note).normal()*0.15;
		bezier_p2 = note+(_rhythm-note)*0.75-(_rhythm-note).normal()*0.15;
		_rhythm = Bezier(note,bezier_p1,bezier_p2,_rhythm,percent);
		graphEngine.Draw( preshed?&GAMEDATA.presh_rc8[type]:&GAMEDATA.rhythm_rc8[type],
			MakefRect(_rhythm.x-NOTE_SIZE*0.5f,_rhythm.y-NOTE_SIZE*0.5f,NOTE_SIZE,NOTE_SIZE));

		// Draw line to connect to last position
		if(first)
		{
			first = false;
			length_drawn = (float)rand()*(6.2831853f/RAND_MAX/DOUBLE_LINE_TWIST_FACTOR);
		}else
			(this->*UnitDrawLine)(lastPoint,_rhythm);

		lastPoint = _rhythm;
	}
}

void Unit::Draw_Strip()
{
	double realTime = (restTime-gameini.delay)<0?0:(restTime-gameini.delay);

	//draw stationary note
	D3DXMATRIX mat,mat2,ptransform;
	base::Sprite->GetTransform(&ptransform);
	D3DXMatrixTransformation2D(&mat, NULL, 0.0f, NULL, &base::MakeDrawRect(D3DXVECTOR2(note.x,note.y)), angle, NULL);
	if(realTime<totalTime*0.875)	// normal size
	{
		graphEngine.Draw( &GAMEDATA.note_strip_rc8[type],
			MakefRect(note.x-STRIP_NOTE_SIZE*0.5f,note.y-STRIP_NOTE_SIZE*0.5f,STRIP_NOTE_SIZE,STRIP_NOTE_SIZE));
			//,D3DCOLOR_ARGB(int((restTime/gameini.standing)*255),255,255,255)
		base::Sprite->SetTransform(&mat);
		graphEngine.Draw( GAMEDATA.pointer_rc,
			MakefRect(note.x-STRIP_NOTE_SIZE*0.5f,note.y-STRIP_NOTE_SIZE*(0.5f+ARROW_HEAD),STRIP_NOTE_SIZE,STRIP_NOTE_SIZE));
			//,D3DCOLOR_ARGB(int((restTime/gameini.standing)*255),255,255,255)
	}
	else							// just appear, enlarge size
	{
		double percent = 1.0f+(gameini.NOTE_BLOWUP-1.0f)*((realTime-totalTime*0.875)/(totalTime*0.125));
		float note_size = STRIP_NOTE_SIZE*percent;
		graphEngine.Draw( &GAMEDATA.note_strip_rc8[type],
			MakefRect(note.x-note_size*0.5f,note.y-note_size*0.5f,note_size,note_size));
			//,D3DCOLOR_ARGB(int((restTime/gameini.standing)*255),255,255,255)
		base::Sprite->SetTransform(&mat);
		graphEngine.Draw( GAMEDATA.pointer_rc,
			MakefRect(note.x-note_size*0.5f,note.y-note_size*(0.5f+ARROW_HEAD),note_size,note_size));
			//,D3DCOLOR_ARGB(int((restTime/gameini.standing)*255),255,255,255)
	}
	base::Sprite->SetTransform(&ptransform);

	//draw rhythm
	double percent = realTime/totalTime, realdura = duration-gameini.delay<0?0:(duration-gameini.delay);
	Point _rhythm,_tail;
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
	if( fabs(restTime)>EPS && !start )
	{
		// Draw head symbol
		graphEngine.Draw(preshed?&GAMEDATA.presh_rc8[type]:&GAMEDATA.rhythm_strip_rc8[type],
			MakefRect( _rhythm.x-STRIP_NOTE_SIZE*0.5f, _rhythm.y-STRIP_NOTE_SIZE*0.5f, STRIP_NOTE_SIZE, STRIP_NOTE_SIZE ) );

		if(systemIni.particle>=3)
		{
			// Add star particle
			if(first)
			{
				first = false;
				length_drawn = 0;
			}
			else
			{
				length_drawn += (_rhythm-lastPoint).mod();
				while(length_drawn>0){
					float rand_size = (float)rand()/RAND_MAX;
					// Chance time got 2 times the number of star particles
					length_drawn -= rand_size*(GAMEDATA.chanceTime)?1:2;
					if(InsideDrawRange(_rhythm.x,_rhythm.y))
						AddStarParticle( _rhythm.x+rand()*(12.0f/RAND_MAX)-6.0f,
							_rhythm.y+rand()*(12.0f/RAND_MAX)-6.0f, rand_size*10, 0 );
				}
			}

			// save current position
			lastPoint = _rhythm;
		}
	}

	// Draw tail symbol
	graphEngine.Draw( preshed?&GAMEDATA.presh_rc8[type]:&GAMEDATA.rhythm_strip_rc8[type],
		MakefRect(_tail.x-STRIP_NOTE_SIZE*0.5f,_tail.y-STRIP_NOTE_SIZE*0.5f,STRIP_NOTE_SIZE,STRIP_NOTE_SIZE) );
}

void Unit::Draw_Bezier(const Point &p0, const Point &p1, const Point &p2, const Point &p3, float l, float r, int flag, D3DCOLOR color)
{
	static int TOTAL = 64;
	static float inc = 1.0f/TOTAL;
	Point p = Bezier(p0,p1,p2,p3,l), q;
	for( float t=l; t < r; t+=inc )
	{
		q = Bezier(p0,p1,p2,p3,t);
		DrawBar(p,q,color,flag);
		p = q;
	}
	q = Bezier(p0,p1,p2,p3,r);
	DrawBar(p,q,color,flag);
}

/*
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
}*/

void Unit::Draw()
{
	if(noteType==NOTETYPE_NORMAL)
		Draw_Normal();
	else
		Draw_Strip();
}

void Unit::Clear()
{
	if(systemIni.particle>=3)
		pSystem.SetState(GAMEDATA.chanceTime?0xc000:0x8000,notePos*4+type);
	else
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
			effectMana.AddEffectNotePress(note.x,note.y,id);
			numberMana.SetCombo(GAMEDATA.combo,note.x,note.y);
		}
		else
		{
			effectMana.AddEffectNotePress(note.x,note.y,id);
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
	{
		if(_note->_note[i]._type<NOTENum)
			unit.push_back(Unit(_note->_note[i],noteTime-nowTime>0?noteTime-nowTime:0,_note->_note[i].notePos));
		else
			unit.push_back(Unit(_note->_note[i],noteTime-nowTime>0?noteTime-nowTime:0,_note->_note[i].notePos,_note->_note[i].duration*singleTime));
	}
}

void NoteMana::SetNew()
{
	singleTime = noteTime = nowTime = 0;
	Clear();
}

void NoteMana::Draw(NOTETYPE type)
{
	for(auto ptr = unit.begin(); ptr != unit.end(); ptr++)
	{
		if(ptr->noteType==type)
			ptr->Draw();
	}
}

void NoteMana::Add(int pos)
{
	if(pos>=_notemap.GetTotalPos())
		pos = _notemap.GetTotalPos();
	while(_notemap.GetNotesPos()<pos)
	{
		Run(_notemap.GetNotes());
		if(_notemap.GetNotes())
			noteTime =_notemap.GetNotes()->timePosition,_notemap.NextNotes();
		else
			break;
	}
}

void NoteMana::Pause(bool state)
{
	if(state==State_Paused) Update(0);
	else lastTime = GetCurrentCount();
}

void NoteMana::Update( LONGLONG dwTime, UINT keyInfo )
{
	// Time management
	{
		LONGLONG currTime = GetCurrentCount();
		if(nowTime) dwTime = currTime-lastTime;
		lastTime = currTime;
	}

	// Called from OnKeyEvent, save time stamp and return immediately
	if(keyInfo)
	{
		key_event new_event = {dwTime,keyInfo};
		key_queue.push_back(new_event);
		return;
	}

	// Process the key_event queue first
	int hit_count=0;
	while(!key_queue.empty())
	{
		key_event &event = key_queue.pop();
		nowTime += event.dwTime;
		for(auto ptr = unit.begin(); ptr != unit.end(); ptr++)
			ptr->Update(event.dwTime);
		OnKeyEvent(event.keyInfo&0x7fffffff,(event.keyInfo&0x80000000)!=0);
	}

	// Do the rest
	nowTime += dwTime;
	for(auto ptr = unit.begin(); ptr != unit.end(); ptr++)
		ptr->Update(dwTime);

	if(GAMEDATA.gamemode==GAME_MODE_PV)
		return;

	bool findNew = true;

	while(findNew)
	{
		findNew = false;
		for(auto ptr = unit.begin(); ptr != unit.end(); ptr++)
		{
			// skip pressed but not yet deleted notes
			if(ptr->preshed)
			{
				if(fabs(ptr->delayTime)<EPS)
				{
					ptr->Clear();
					unit.erase(ptr);
				}
				continue;
			}

			if(ptr->noteType==NOTETYPE_NORMAL)		// normal note
			{
				if(GAMEDATA.gamemode==GAME_MODE_AUTO && (ptr->restTime-gameini.delay)<=gameini.eps)
				{		// auto press in auto mode
					UINT type = ptr->type;
					if( type>=0 && type<UNIT_KEYMAX )
						hit_count += OnKeyEvent(systemIni.GameKeyMap[type],true);
					findNew = true;
				}
				else if(fabs(ptr->restTime)<EPS)			// timeout, note is missed
				{
					GAMEDATA.ModifyHP(-gameini.HP_DEC), GAMEDATA.combo = 0, GAMEDATA.worst_cnt++, numberMana.SetCombo(0,ptr->note.x,ptr->note.y), gameui.SetState(WORST);
					GAMEDATA.maxScoreTempCombo++;
					GAMEDATA.maxScoreCanGet += gameini.SCORE_COOL + ((GAMEDATA.maxScoreTempCombo>50?50:GAMEDATA.maxScoreTempCombo)/10)*50;
					if(GAMEDATA.chanceTime)			// reset chance time combo
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
				break;
			}
			else		// strip note
			{
				if(GAMEDATA.gamemode==GAME_MODE_AUTO)
				{
					if(!ptr->start && (ptr->restTime-gameini.delay)<gameini.eps)
					{		// simulate press
						UINT type = ptr->type;
						if( type>=0 && type<UNIT_KEYMAX )
							hit_count += OnKeyEvent(systemIni.GameKeyMap[type],true);
						findNew = true;
						break;
					}
					else if(ptr->start && (ptr->duration-gameini.delay)<gameini.eps)
					{		// simulate release
						UINT type = ptr->type;
						if( type>=0 && type<UNIT_KEYMAX )
							hit_count += OnKeyEvent(systemIni.GameKeyMap[type],false);
						findNew = true;
						break;
					}
				}
				if(ptr->finish())		// missed
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
			}
		}
	}
	if(hit_count)
		soundEngine.PlayHit(hit_count);

	//totalTime += dwTime;
	//singleTime = _notemap.Cal_Single(_notemap.GetBPM());
	//while(noteTime+singleTime<=totalTime) Run(_notemap.GetNotes()), noteTime+=singleTime,_notemap.NextNotes();
}

int NoteMana::OnKeyEvent(UINT nChar, bool bKeyDown, bool bInternal)
{
	int type, hit_count=0;
	UINT *keymap = systemIni.GameKeyMap;
	for( type=0; type<UNIT_KEYMAX && nChar!=keymap[type]; type++ );
	if( type == UNIT_KEYMAX ) return 0;

	if(!bInternal)
	{	// enqueue the key event and return immediately
		Update(0,bKeyDown?(nChar|0x80000000):nChar);
		if(bKeyDown) soundEngine.PlayHit(1);
		return 0;
	}

	if(bKeyDown)
	{
		//soundEngine.PlayHit();
		hit_count++;
		auto ptr = unit.begin();
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
				if(restTime<=gameini.COOL || GAMEDATA.gamemode==GAME_MODE_AUTO)
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
					if(GAMEDATA.chanceTime && systemIni.particle>=3)
						for(short angle=spin_angle_inc[0]; angle<360; angle+=spin_angle_inc[0])//
							ptr->AddSpinParticle(angle);
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
					if(GAMEDATA.chanceTime && systemIni.particle>=3)
						for(short angle=spin_angle_inc[1]; angle<360; angle+=spin_angle_inc[1])
							ptr->AddSpinParticle(angle);
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
		auto ptr = unit.begin();

		while(ptr!=unit.end())
		{
			if(ptr->noteType==NOTETYPE_STRIP&&ptr->type==type&&ptr->start&&!ptr->preshed)
				break;
			ptr++;
		}
		if(ptr!=unit.end())
		{
			double restTime = fabs(ptr->duration-gameini.delay);
			soundEngine.PlayHit(1.0f);
			hit_count++;
			if(restTime<=gameini.COOL || GAMEDATA.gamemode==GAME_MODE_AUTO)
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
				if(GAMEDATA.chanceTime && systemIni.particle>=3)
					for(short angle=spin_angle_inc[0]; angle<=360; angle+=spin_angle_inc[0])
						ptr->AddSpinParticle(angle);
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
				if(GAMEDATA.chanceTime && systemIni.particle>=3)
					for(short angle=spin_angle_inc[1]; angle<360; angle+=spin_angle_inc[1])
						ptr->AddSpinParticle(angle);
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
	return hit_count;
}
//------------------------------------------------------------------------------------

void GameUI::Init()
{
	state = NOSTATE;
	uistate = NORMAL;
	ui = graphEngine.AddTexture("pic\\ui.png");
	top_bar_rc = (ImageResource*)g_res.getResource("top_bar");
	bottom_bar_rc = (ImageResource*)g_res.getResource("bottom_bar");
	top_bar_red_rc = (ImageResource*)g_res.getResource("top_bar_red");
	bottom_bar_red_rc = (ImageResource*)g_res.getResource("bottom_bar_red");
	rhythm_logo_rc = (ImageResource*)g_res.getResource("rhythm_logo");
	hp_gauge_rc = (ImageResource*)g_res.getResource("hp_gauge[20]");

	spin_angle_inc = ((IntResource*)g_res.getResource("spin_angle_inc[2]"))->value;
	SINGLE_LINE_INC = *((FloatResource*)g_res.getResource("single_line_inc"))->value;
	DOUBLE_LINE_INC = *((FloatResource*)g_res.getResource("double_line_inc"))->value;
	DOUBLE_LINE_TWIST_FACTOR = *((FloatResource*)g_res.getResource("double_line_twist_factor"))->value;
	DOUBLE_LINE_SPACE_FACTOR = *((FloatResource*)g_res.getResource("double_line_space_factor"))->value;
	COMET_LINE_INC = *((FloatResource*)g_res.getResource("comet_line_inc"))->value;
	STRIP_NOTE_SIZE = *((FloatResource*)g_res.getResource("strip_note_size"))->value;
	NOTE_SIZE = *((FloatResource*)g_res.getResource("note_size"))->value;
	META_SIZE = *((FloatResource*)g_res.getResource("meta_size"))->value;
	ARROW_HEAD = *((FloatResource*)g_res.getResource("arrow_head"))->value;
}

void GameUI::SetNew()
{
	state = NOSTATE;
	uistate = NORMAL;
	delayTime = uidelay = 0;
	deltaTop = top_bar_rc->Height;
	deltaBottom = bottom_bar_rc->Height;
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
		deltaTop = top_bar_rc->Height*double(moveTime-nowTime)/moveTime;
		deltaBottom = bottom_bar_rc->Height*double(moveTime-nowTime)/moveTime;
	}
	else if(moveState==2)
	{
		deltaTop = top_bar_rc->Height*double(nowTime)/moveTime;
		deltaBottom = bottom_bar_rc->Height*double(nowTime)/moveTime;
	}
	if(nowTime==moveTime)
		moveState = 0;
}

void GameUI::Draw()
{
	//hp
	int hpnum = ((GAMEDATA.hp+gameini.SINGLE_HP-1)/gameini.SINGLE_HP)-1;
	int hp_index = hpnum;

	//bar
	if(hpnum<=3&&((spark>>5)&1))
	{
		graphEngine.Draw(top_bar_red_rc,0,-deltaTop);
		graphEngine.Draw(bottom_bar_red_rc,0,HEIGHT-bottom_bar_rc->Height+deltaBottom);
	}
	else
	{
		graphEngine.Draw(top_bar_rc,0,-deltaTop);
		graphEngine.Draw(bottom_bar_rc,0,HEIGHT-bottom_bar_rc->Height+deltaBottom);
	}

	RECT hp_rect,sign_rect;
	if(hpnum>=0&&hpnum<3&&((spark>>5)&1))
	{
		//hp_rect = MakeRect(152+hpnum*38,79+38,38,38);
		hp_index = hpnum+17;
		sign_rect = MakeRect(116,271,36,36);
	}
	else if(hpnum>=0&&hpnum<13)
	{
		//hp_rect = MakeRect(hpnum*38,79,38,38);
		sign_rect = MakeRect(40,271,36,36);
	}
	else if(hpnum>=13)
	{
		//hp_rect = MakeRect((hpnum-13)*38,79+38,38,38);
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
		//graphEngine.Draw(ui,hp_rect,9,HEIGHT-bottom_bar_rc->Height+11+deltaBottom);
		graphEngine.Draw(&hp_gauge_rc[hp_index],9,HEIGHT-bottom_bar_rc->Height+11+deltaBottom);
		switch(uistate)
		{
		case NORMAL:
			graphEngine.Draw(ui,sign_rect,11,HEIGHT-bottom_bar_rc->Height+10+deltaBottom);
			break;
		case ADD:
			graphEngine.Draw(ui,sign_rect,9,HEIGHT-bottom_bar_rc->Height+10+deltaBottom);
			break;
		case DEC:
			graphEngine.Draw(ui,sign_rect,8,HEIGHT-bottom_bar_rc->Height+10+deltaBottom);
			break;
		}
	}

	//state
	if(state!=NOSTATE)
		graphEngine.Draw(ui,GAMEDATA.state_rect[state],WIDTH-90,HEIGHT-20+deltaBottom);


	numberMana.DrawScore(GAMEDATA.score);

	black_top->Draw();
	black_bottom->Draw();

	if(GAMEDATA.chanceTime)
		chanceTimeLabel->Draw();

	graphEngine.Draw(rhythm_logo_rc);
	graphEngine.SetFont(StringTable(56).c_str(),12,false,FW_BOLD);
	graphEngine.DrawText(_notemap.GetMapName()+" - "+StringTable(HardType[_notemap.GetLevel()-1]),22,3);
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
	numberMana.Init();
	pSystem.Init();

	white = new GUIStaticImage("white");
	black_top = new GUIStaticImage("black_top");
	(*black_top->efvct.a)=128;
	black_bottom = new GUIStaticImage("black_bottom");
	(*black_bottom->efvct.a)=128;
	chanceTimeLabel = new GUIStaticImage("chanceTimeLabel");
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
	_notemap.Resource_read();
	effectMana.SetNew();
	_notemana.SetNew();
	finish = paused = false, visible = true;
	(*black_top->efvct.drawRect)=MakeRect(0,-gameui.top_bar_rc->Height,gameui.top_bar_rc->Width,gameui.top_bar_rc->Height);
	(*black_bottom->efvct.drawRect)=MakeRect(0,HEIGHT,gameui.top_bar_rc->Width,gameui.top_bar_rc->Height);
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
				videoEngine.Play(filename, true);
		}
	}
	for(int i = 0; i < BGSNum; i++)
		if(_nowFrame->_BGM[i]!=NOWAV)
		{
			filename = _notemap.GetWav(_nowFrame->_BGM[i]);
			soundEngine.curr_music = (filename!="")?soundEngine.PlayMusic(filename):NULL;
		}
	if(!GAMEDATA.chanceTime&&_nowFrame->notePos==_notemap._chanceTimeStart)
	{
		GAMEDATA.chanceTime = true;
		(*black_top->efvct.drawRect) = MakeRect(0,0,480,0);
		(*black_bottom->efvct.drawRect) = MakeRect(0,272,480,0);
		black_top->efvct.AddEffect(new GUIEF_Stretch(black_top->efvct.drawRect,20,MakeRect(0,0,gameui.top_bar_rc->Width,gameui.top_bar_rc->Height),0));
		black_bottom->efvct.AddEffect(new GUIEF_Stretch(black_bottom->efvct.drawRect,20,MakeRect(0,HEIGHT-gameui.top_bar_rc->Height,gameui.top_bar_rc->Width,gameui.top_bar_rc->Height),0));
		effectMana.AddEffectChanceTimeStart();
		gameui.MoveAway(30);
	}
	if(GAMEDATA.chanceTime&&_nowFrame->notePos==_notemap._chanceTimeEnd+1)
	{
		GAMEDATA.chanceTime = false;
		black_top->efvct.ReleaseEffects();
		black_bottom->efvct.ReleaseEffects();
		black_top->efvct.AddEffect(new GUIEF_Stretch(black_top->efvct.drawRect,20,MakeRect(0,-(gameui.top_bar_rc->Height),gameui.top_bar_rc->Width,gameui.top_bar_rc->Height),0));
		black_bottom->efvct.AddEffect(new GUIEF_Stretch(black_bottom->efvct.drawRect,20,MakeRect(0,HEIGHT,gameui.top_bar_rc->Width,gameui.top_bar_rc->Height),0));
		effectMana.AddEffectChanceTimeEnd(GAMEDATA._chanceTimeScore);
		gameui.MoveIn(30);
	}
}

void GameCore::Update(LONGLONG dwTime)
{
	float dwTimeMilliSecond = (float)dwTime*1000/base::SECOND;
	//soundEngine.Update(dwTimeMilliSecond);	//already updated in GameMainUpdate???
	//GAMEDATA.gamemode = 2;
	if(!paused)
	{
		videoEngine.Update();
		//if(GAMEDATA.gamemode!=GAME_MODE_PV)
		_notemana.Update(dwTime);
		if(GAMEDATA.gamemode!=GAME_MODE_PV)
			gameui.Update(dwTime);
		effectMana.Update(dwTimeMilliSecond*0.055f);
		pSystem.Update(dwTimeMilliSecond);

		totalTime += dwTime;

		while(noteTime<=totalTime) 
		{
			LpFrame pFrame = _notemap.GetFrame();
			Run(pFrame);
			if(pFrame)
				noteTime = pFrame->timePosition,_notemap.NextFrame();
			else
				break;
		}

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
		_notemana.Draw(NOTETYPE_STRIP);
		pSystem.Draw();
		_notemana.Draw(NOTETYPE_NORMAL);
		effectMana.Draw();
	}
	if(GAMEDATA.gamemode!=GAME_MODE_PV)
		gameui.Draw();
	if(paused)
		menu->Draw();
	if(finish)
		white->Draw();
	//Sleep(166);
}

void GameCore::OnKeyEvent(UINT nChar, bool bKeyDown)
{
	if(GAMEDATA.gamemode==GAME_MODE_NORMAL)
		if(!paused)
			_notemana.OnKeyEvent(nChar,bKeyDown,false);
	if(paused)
		menu->OnKeyEvent(nChar,bKeyDown);
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
	_notemana.Pause(paused);
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