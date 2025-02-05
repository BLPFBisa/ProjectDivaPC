#include "notemap.h"
#include "VideoEngine.h"
#include "SoundEngine.h"
#include "GraphEngine.h"
#include <iostream>
using namespace std;

int CheckResource(string filename)
{
	if(filename.find(".avi")!=string::npos||filename.find(".mpg")!=string::npos||filename.find(".rmvb")!=string::npos||filename.find(".wmv")!=string::npos)
		return VIDEO;
	else
		return GRAPH;
}

//NoteMap
void NoteMap::SetNew(string filename,string baseDirec)
{
	int pos;
	
	Clear();

	FILE *fin = fopen(filename.c_str(),"r");
	//ifstream fin(filename.c_str());
	char buffer[300];
	if(!fin)
		return;

	//Map General Info
	fgets(buffer,300,fin);
	checkTail0(buffer,300);
	EditorVer = buffer;
	//getline(fin,EditorVer);
	fgets(buffer,300,fin);
	checkTail0(buffer,300);
	_mapName = buffer;
	//getline(fin,_mapName);
	fgets(buffer,300,fin);
	checkTail0(buffer,300);
	_noterName = buffer;
	//getline(fin,_noterName);
	fgets(buffer,300,fin);
	checkTail0(buffer,300);
	_authorName = buffer;
	//getline(fin,_authorName);
	fgets(buffer,300,fin);
	checkTail0(buffer,300);
	_musicStyle = buffer;
	//getline(fin,_musicStyle);
	fgets(buffer,300,fin);
	checkTail0(buffer,300);
	//OverViewPic
	fscanf(fin,"%d\n%d\n%lf\n%d\n",&_level,&_hard,&_timeTable._BPM,&_timeTable._frameNum);
	//fin>>_level>>_hard;

    //TimeTable General Info
	//fin>>_timeTable._BPM;//Total BPM
	//fin>>_timeTable._frameNum;//Period Num
	_timeTable._frameNum *= notePerPeriod;
	for(int i = 0; i < _timeTable._frameNum; i++)
		_timeTable._frame[i].notePos = i;

	//Period Info
			 
	//BPM
	while(true)
	{
		fscanf(fin,"%d",&pos);
		//fin >> pos;
		if(pos==-1)
			break;
		fscanf(fin,"%lf\n",&_timeTable._frame[pos]._BPM);
		//fin >> _timeTable._frame[pos]._BPM;
	    _timeTable._frame[pos]._IsSetBPM = true;
	}

	//Resource
	while(true)
	{
		fscanf(fin,"%d",&pos);
		//fin >> pos;
		if(pos==-1)
			break;
		fscanf(fin,"%d\n",&_timeTable._frame[pos]._resource);
		//fin >> _timeTable._frame[pos]._resource;
	}

	//BGS
	while(true)
	{
		fscanf(fin,"%d",&pos);
		//fin >> pos;
		if(pos==-1)
			break;
		int bgsNum;
		fscanf(fin," %d",&bgsNum);
		//fin >> bgsNum;
		fscanf(fin," %d\n",&_timeTable._frame[pos]._BGM[bgsNum]);
		//fin >> _timeTable._frame[pos]._BGM[bgsNum];
	}
	
	//note
	while(true)
	{
		fscanf(fin,"%d",&pos);
		//fin >> pos;
		if(pos==-1)
			break;
	    int noteNum =  _timeTable._frame[pos]._noteNum;
		Note &tnote = _timeTable._frame[pos]._note[noteNum];
		fscanf(fin," %d %d %d %d %d %d\n",&tnote._type,&tnote._x,&tnote._y,&tnote._tailx,&tnote._taily,&tnote._key);
		//fin >> _timeTable._frame[pos]._note[noteNum]._type >> _timeTable._frame[pos]._note[noteNum]._x >> 
			//_timeTable._frame[pos]._note[noteNum]._y >> _timeTable._frame[pos]._note[noteNum]._tailx >> 
			//_timeTable._frame[pos]._note[noteNum]._taily >> _timeTable._frame[pos]._note[noteNum]._key;
		_timeTable._frame[pos]._noteNum++;
	}
	
	wav.clear();
	resource.clear();

	//wav
	while(true)
	{
		fscanf(fin,"%d",&pos);
		//fin >> pos;
		if(pos==-1)
			break;
		string filename;
		fgets(buffer,300,fin);
		checkTail0(buffer,300);
		filename=buffer;
		//getline(fin,filename);
		trimLeft(filename);
		trimRight(filename);
		filename = baseDirec + filename;
		wav[pos] = filename;
	}

	//resource
	while(true)
	{
		fscanf(fin,"%d",&pos);
		//fin >> pos;
		if(pos==-1)
		    break;
		string filename;
		fgets(buffer,300,fin);
		checkTail0(buffer,300);
		filename=buffer;
		//getline(fin,filename);
		trimLeft(filename);
		trimRight(filename);
		filename = baseDirec + filename;
		resource[pos] = filename;
	}
	
	if(EditorVer>="1.0.1.0")
		fscanf(fin,"%d %d",&_chanceTimeStart,&_chanceTimeEnd);
	fclose(fin);
}

void NoteMap::Resource_read()
{
	string filename;
	for(map<int,string>::iterator ptr = resource.begin(); ptr != resource.end(); ptr++)
	{
		filename = ptr->second;
		if(CheckResource(filename)==VIDEO)
			videoEngine.Read(filename);
	}
	for(map<int,string>::iterator ptr = resource.begin(); ptr != resource.end(); ptr++)
	{
		filename = ptr->second;
		if(CheckResource(filename)==GRAPH)
			graphEngine.AddTexture(filename,GameCoreRes);
	}
	for(map<int,string>::iterator ptr = wav.begin(); ptr != wav.end(); ptr++)
	{
		filename = ptr->second;
		soundEngine.ReadMusic(filename);
	}
}

//TimeTable
bool TimeTable::NextFrame()
{
	if(framePos==_frameNum)
		return true;
	return ((++framePos)==_frameNum);
}

bool TimeTable::NextNotes()
{
	if(notesPos==_frameNum)
		return true;
	return ((++notesPos)==_frameNum);
}
/*//Period
bool Period::NextFrame()
{
	if(framePos==notePerPeriod)
		return true;
	return ((++framePos)==notePerPeriod);
}*/