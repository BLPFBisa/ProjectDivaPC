#ifndef __NoteMapH__
#define __NoteMapH__

#include "Base.h"
using namespace std;

typedef int RESOURCE;
typedef int WAV;
const RESOURCE NOSOURCE = -1;
const int GRAPH = 0;
const int VIDEO = 1;
const WAV NOWAV = -1;
const int BGSNum = 10;
const int NOTENum = 8;
const int notePerPeriod = 192;
const int PeriodNum = 5000;
const int timePerPeriod = 4;
extern bool bHasAudio;
enum UNIT{UNIT_CIRCLE=0,UNIT_RECT=1,UNIT_CROSS=2,UNIT_TRIANGLE=3,UNIT_RIGHT=4,UNIT_LEFT=5,UNIT_DOWN=6,UNIT_UP=7,UNIT_KEYMAX=8};
enum NOTETYPE{NOTETYPE_NORMAL,NOTETYPE_STRIP};
extern int UNIT_COLOR[][3];

int CheckResource(string filename);

class Note
{
public:
	int notePos;
	int _type,_x,_y,_tailx,_taily,_key;
	double duration;
	Note():notePos(0),_type(0),_x(0),_y(0),_tailx(0),_taily(0),_key(0) {}
	void Clear() {_type = _x = _y = _tailx = _taily = _key = 0;}
};

class Frame
{
public:
	bool _IsSetBPM;
	int _noteNum;
	double _BPM;
	int notePos;
	double timePosition;
	RESOURCE _resource;
	WAV _BGM[BGSNum];
	Note _note[NOTENum];
	Frame():_IsSetBPM(false),timePosition(0),_noteNum(0),_BPM(0),_resource(NOSOURCE),notePos(0) {memset(_BGM,NOWAV,sizeof(_BGM));}
	void Clear() {_IsSetBPM = false, _noteNum = 0, _BPM = 0, _resource = NOSOURCE, notePos = 0, timePosition = 0, memset(_BGM,NOWAV,sizeof(_BGM));}
};


typedef Frame *LpFrame;

/*class Period
{
	int framePos;
	Frame _frame[notePerPeriod];
	friend class Period;
	friend class NoteMap;
public:
	Period():framePos(0) {}
	void Clear() {framePos = 0;}
	bool NextFrame();
	LpFrame GetFrame() {if(framePos==notePerPeriod)return NULL;return &_frame[framePos];}
};*/

class TimeTable
{
	double _BPM;
	int _frameNum;
	int framePos;
	int notesPos;
	//Frame _frame[PeriodNum*notePerPeriod];
	Frame *_frame;
	friend class NoteMap;
public:
	TimeTable():_BPM(0),_frameNum(0),framePos(0),notesPos(0) {_frame=NULL;}
	void Clear() {_BPM = 0,_frameNum = framePos = notesPos = 0;SAFE_RELEASE_ARRAY(_frame);}
	void Prework();
	bool NextFrame();
	bool NextNotes();
	LpFrame GetNotes() {if(notesPos==_frameNum)return NULL; return &_frame[notesPos];}
	LpFrame GetFrame() {if(framePos==_frameNum)return NULL; return &_frame[framePos];}
};

//#define _notemap NoteMap::Instance()

class NoteMap
{
	string EditorVer;
	string _mapName,_noterName,_authorName,_musicStyle;
	int _level,_hard;
	TimeTable _timeTable;
	map<int,string> wav;
	map<int,string> resource;

public:
	NoteMap() {}
	NoteMap(string filename,string baseDirec) {SetNew(filename,baseDirec);}

	int _chanceTimeStart,_chanceTimeEnd;

	//static NoteMap& Instance(void) {static NoteMap instance; return instance;}

	void Init() {}
	void SetNew(string filename,string baseDirec);
	void Resource_read();

	bool NextFrame() {return _timeTable.NextFrame();}
	bool NextNotes() {return _timeTable.NextNotes();}
	LpFrame GetFrame() {return _timeTable.GetFrame();}
	LpFrame GetNotes() {return _timeTable.GetNotes();}

	string GetWav(WAV id) {if(wav.find(id)==wav.end())return ""; return wav[id];}
	string GetResource(RESOURCE id) {if(resource.find(id)==resource.end())return ""; return resource[id];}

	void Clear() {_chanceTimeStart=_chanceTimeEnd=-1,_timeTable.Clear();}
	int GetFramePos() {return _timeTable.framePos;}
	int GetNotesPos() {return _timeTable.notesPos;}
	int GetTotalPos() {return _timeTable._frameNum;}
	int GetLevel() {return _level;}
	string GetMapName() {return _mapName;}
	double GetBPM() {return _timeTable._BPM;}
	double Cal_Single(const double &_bpm) {return 60.0*base::SECOND/(_bpm*(notePerPeriod/timePerPeriod));/*60.0*base::FPS/(_bpm/timePerPeriod*notePerPeriod);*/}
};

extern NoteMap _notemap;

#endif