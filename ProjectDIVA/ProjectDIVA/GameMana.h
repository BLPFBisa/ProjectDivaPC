#ifndef __GameManaH__
#define __GameManaH__

#include <queue>
#include "defines.h"
#include "notemap.h"
#include "GameIni.h"
#include "GraphEngine.h"
#include "VideoEngine.h"
#include "Gui.h"

//#define GAMEDATA GameData::Instance()

enum PLAYSTATE{SAFE,COOL,FINE,_SAFE,_COOL,_FINE,WORST,NOSTATE};
enum UISTATE{ADD,DEC,NORMAL};

class GameData
{
	static const int TYPENUM = 8;
	ImageResource *pointer_rc;
	ImageResource *note_rc8;
	ImageResource *rhythm_rc8;
	ImageResource *note_strip_rc8;
	ImageResource *rhythm_strip_rc8;
	ImageResource *bar_in_rc8;
	ImageResource *bar_out_rc;
	ImageResource *presh_rc8;

	RECT state_rect[7];
	Texture *background;
	friend class Unit;
	friend class NoteMana;
	friend class GameCore;
	friend class GameUI;
	int hp, score, combo, maxScoreCanGet, maxScoreTempCombo, maxScoreTempChanceTimeCombo;
	int cool_cnt,fine_cnt,safe_cnt,worst_cnt,maxcombo;
	int _chanceTimeScore,_chanceTimeCombo; 
	bool chanceTime;

public:
	int gamemode;

	//static GameData& Instance(void) {static GameData instance; return instance;}
	GameData() {}
	~GameData() {}
	void Init();
	void SetNew();
	void Release();
	void ModifyHP(int delta);
	int GetCombo(){return combo;}
};
// Frequently accessed class use global definition
extern GameData GAMEDATA;

class Unit
{
	int serial;
	//static const int STRIP_DELAY = 1;
	int notePos;
	bool preshed,first;
	UNIT type;
	double angle;
	Point note;
	Point rhythm;
	Point bezier_p1,bezier_p2,lastPoint;
	int key;
	double restTime;
	double delayTime;
	double totalTime;
	//double _a,_b,_c;
	friend class NoteMana;
	double endtime;
	//STRIP
	bool start;
	float length_drawn;
	double totalduration,duration;
	NOTETYPE noteType;
public:
	Unit():type(UNIT_CIRCLE),noteType(NOTETYPE_NORMAL),restTime(0),angle(0),preshed(false) {}
	Unit(Note _note, double standing, int _notePos);
	Unit(Note _note, double standing, int _notePos, double duration);
	void Update(double dwTime);
	void Draw_Normal();
	void Draw_Strip();
	bool finish();
	void Draw_Bezier(const Point &p0, const Point &p1, const Point &p2, const Point &p3, float l = 0, float r = 1, int flag = 0, D3DCOLOR color = D3DXCOLOR(255,255,255,255));
	//void Draw_Frame(const Point &p0, const Point &p1, const Point &p2, D3DCOLOR color = D3DXCOLOR(255,255,255,255));
	void Draw();
	void Clear();
	void Presh(PLAYSTATE id);
	Point Bezier(const Point &p0, const Point &p1, const Point &p2, const float &t);
	Point Bezier(const Point &p0, const Point &p1, const Point &p2, const Point &p3, const float &t);
	void DrawNullLine(const Point &a, const Point &b){return;}
	void DrawSingleLine(const Point &a, const Point &b);
	void DrawDoubleLine(const Point &a, const Point &b);
	void DrawCometLine(const Point &a, const Point &b);
	void DrawBar(const Point &a, const Point &b, D3DCOLOR color, int flag = 0);
	void AddMetafile(float x, float y, D3DCOLOR color, int flag = 0);
	void AddParticle(float x, float y, float size);
	void AddCometParticle(float x, float y, float dx, float dy);
	void AddStarParticle(float x, float y, float size, D3DCOLOR color);
	void AddSpinParticle(short angle);
};

extern void (Unit::*UnitDrawLine)(const Point &a, const Point &b);

// Thread-safe unordered vector
template <class T>
class TSvector {
private:
	T*	m_data;
	int	m_size;
	int	m_max_size;
	int m_lock;
	DWORD	threadID;
	HANDLE	hEvent;

	void AcquireLock()
	{
		DWORD currentID = GetCurrentThreadId();
		if(currentID!=threadID)
		{
			WaitForSingleObject(hEvent,INFINITE);
			threadID = currentID;
		}
		m_lock++;
	}
	void ReleaseLock()
	{
		if(--m_lock == 0)
		{
			threadID = 0;
			SetEvent(hEvent);
		}
	}

	struct iterator{	// must be private
		int posi;
		TSvector <T> *q;

		iterator& operator ++ ()
		{
			++ posi;
			return *this;
		}
		iterator& operator -- ()
		{
			-- posi;
			return *this;
		}
		iterator& operator + (int rhs)
		{
			posi += rhs;
			return *this;
		}
		iterator& operator += (int rhs)
		{
			posi += rhs;
			return *this;
		}
		bool operator != (int rhs){ return posi!=rhs; }
		bool operator == (int rhs){	return posi==rhs; }
		void operator = (int rhs) { posi = rhs; }
		T& operator *()	{ return q->m_data[posi]; }
		T* operator ->(){ return &q->m_data[posi]; }
		iterator(int _posi, TSvector <T> *_q):posi(_posi),q(_q)
		{	// acquire lock
			q->AcquireLock();
		}
		~iterator()
		{	// release lock
			q->ReleaseLock();
		}
	};

public:
	TSvector(int _size = 16)
	{
		if(_size<=0) _size=4;
		m_lock = m_size = 0;
		m_max_size = _size;
		m_data = new T [m_max_size];
		hEvent = CreateEvent(NULL,FALSE,TRUE,NULL);
	}
	~TSvector()
	{
		AcquireLock();
		CloseHandle(hEvent);
		delete [] m_data;
	}

	bool empty() const { return !m_size; }
	int size() const { return m_size; }
	void push_back(T& x)
	{
		AcquireLock();
		if(m_size >= m_max_size)	//expand if full
		{
			m_max_size <<= 1;
			T* new_data = new T [m_max_size];
			for(int x=0; x<m_size; x++)
				new_data[x] = m_data[x];
			delete [] m_data;
			m_data = new_data;
		}
		m_data[m_size++] = x;
		ReleaseLock();
	}
	void push_back(T x)
	{
		AcquireLock();
		if(m_size >= m_max_size)	//expand if full
		{
			m_max_size <<= 1;
			T* new_data = new T [m_max_size];
			for(int x=0; x<m_size; x++)
				new_data[x] = m_data[x];
			delete [] m_data;
			m_data = new_data;
		}
		m_data[m_size++] = x;
		ReleaseLock();
	}
	T pop_back()
	{
		AcquireLock();
		if(m_size>0)
		{
			--m_size;
			ReleaseLock();
			return m_data[m_size];
		}
		ReleaseLock();
	}
	void erase(iterator &it)
	{
		if(m_size>0)
		{
			m_data[it.posi] = m_data[--m_size];
			m_data[m_size] = T();
		}
	}
	void clear()
	{
		AcquireLock();
		delete [] m_data;
		m_data = new T [m_max_size];
		m_size = 0;
		ReleaseLock();
	}

	iterator begin(){ return iterator(0,this); }
	int end(){ return m_size; }
};

template <class T>
class CircularQueue {
private:
	T*	m_data;
	int p_head;
	int p_tail;
	int m_size;
	int m_max_size;
	int m_mask;

public:
	CircularQueue( int _size = 255 )
	{
		for(m_max_size=1; m_max_size<=_size; m_max_size<<=1);
		m_mask = m_max_size-1;
		m_data = new T [m_max_size];
		p_head = p_tail = m_size = 0;
	}
	~CircularQueue()
	{
		delete [] m_data;
	}

	void push_back(T x)
	{
		if( m_size >= m_max_size-1 )
		{	//expand the queue
			T* new_data = new T [m_max_size<<1];
			memcpy(new_data,m_data,sizeof(T)*m_max_size);
			for(int x=p_head; x!=p_tail; inc(x))
				if(x>p_tail) new_data[x+m_max_size] = new_data[x];
			delete [] m_data;
			m_data = new_data;
			if(p_head>p_tail) p_head += m_max_size;
			m_max_size<<=1;
			m_mask = m_max_size-1;
		}
		m_data[p_tail] = x;
		inc(p_tail);
		++m_size;
	}
	T& pop()
	{
		int old_head = p_head;
		if(m_size>0)
		{
			//m_data[p_head].~T();
			inc(p_head);
			--m_size;
		}
		return m_data[old_head];
	}

	struct iterator{
		int posi;
		CircularQueue <T> *q;
		iterator& operator ++ ()
		{
			//posi = (posi+1)&q->m_mask;
			q->inc(posi);
			return *this;
		}
		bool operator != (const iterator rhs)
		{
			return posi!=rhs.posi;
		}
		bool operator == (const iterator rhs)
		{
			return posi==rhs.posi;
		}
		T& operator *(){return q->m_data[posi];}
		T* operator ->(){ return &q->m_data[posi];}
	};
	iterator begin()
	{
		iterator i={p_head,this};
		return i;
	}
	iterator end()
	{
		iterator i={p_tail,this};
		return i;
	}
	bool empty() const { return !m_size; }
	int size() const { return m_size; }
	void clear(){ while(m_size>0)pop(); }
	inline void inc(int &var){ var=(var+1)&m_mask; }
	void erase(iterator &it)
	{
		if(!m_size) return;
		int x;
		for(x=p_head; x!=p_tail; inc(x))
			if(x==it.posi) break;
		if(x==p_tail) return;
		for(int oldx=(x-1+m_max_size)&m_mask; x!=p_head; x=oldx, oldx=(x-1+m_max_size)&m_mask)
			m_data[x] = m_data[oldx];
		inc(p_head);
		--m_size;
	}
};

class NoteMana
{
	double singleTime;
	double noteTime;
	CircularQueue <Unit> unit;
	struct key_event
	{
		LONGLONG dwTime;
		UINT keyInfo;
	};
	CircularQueue <key_event> key_queue;
	
public:
	LONGLONG nowTime,startTime,lastTime;
	NoteMana():key_queue(CircularQueue<key_event>(15)),unit(CircularQueue<Unit>(63)) {}//
	void SetNew();
	void Init() {}
	void Clear() {unit.clear();}
	//---------------------------------------------
	void Add(int pos);
	void Run(LpFrame _note);
	void Draw(NOTETYPE type);
	void Update(LONGLONG,UINT=0);
	void Pause(bool state);
	//---------------------------------------------
	int OnKeyEvent(UINT nChar, bool bKeyDown, bool bInternal=true);
	//---------------------------------------------
	//void Pop() {if(!unit.empty()) unit.pop();}
	int GetSize() {return unit.size();}
	int GetScore() {return GAMEDATA.score;}
};

//#define gameui GameUI::Instance()
class GameUI
{
	PLAYSTATE state;
	UISTATE uistate;
	double delayTime, uidelay;
	int spark;
	int moveTime, nowTime;
	int moveState; //0 normal 1 in 2 out
	int deltaTop, deltaBottom;
	Texture *ui;
	ImageResource *top_bar_rc, *bottom_bar_rc;
	ImageResource *top_bar_red_rc, *bottom_bar_red_rc;
	ImageResource *rhythm_logo_rc, *hp_gauge_rc;
	friend class GameCore;
public:
	//static GameUI& Instance(void) {static GameUI instance; return instance;}
	GameUI() {}
	~GameUI() {}
	void Init();
	void Draw();
	void Update(double dwTime);
	void SetNew();
	void SetState(PLAYSTATE _state);
	void SetUIState(UISTATE _state);
	void MoveAway(int time);
	void MoveIn(int time);
};

extern GameUI gameui;

class GameCore
{
	double singleTime;
	double noteTime;
	LONGLONG totalTime;
	LpFrame nowFrame;
	bool finish;
	bool visible;
	bool ifclear;

public:
	GUIMessageBox *menu;
	bool paused;
	RESOURCE resource;
	NoteMana _notemana;
	GameCore() {}
	~GameCore();
	void Init();
	void SetNew(string filename,string baseDirec,int gamemode = GAME_MODE_NORMAL);
	void SetPausedMenu(GUIMessageBox *_menu) {menu = _menu;}
	//--------------------------------
	void Run(LpFrame _nowFrame);
	void Update(LONGLONG dwTime);
	void Draw(IDirect3DDevice9 *Device);
	void Finish();
	void Clear();
	//--------------------------------
	void OnKeyEvent(UINT nChar, bool bKeyDown);
	//--------------------------------
	void PauseGame(bool _paused);
	bool IsFinish() {return (nowFrame==NULL);}
	int GetScore() {return _notemana.GetScore();}
	int GetNoteSize() {return _notemana.GetSize();}
	int GetCombo() {return GAMEDATA.combo;}
};
extern GameCore core;

#endif

