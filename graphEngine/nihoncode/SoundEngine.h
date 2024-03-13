#ifndef __SoundEngineH__
#define __SoundEngineH__

#include "defines.h"
#pragma comment( lib, "fmodex_vc.lib" )
#include "fmod.hpp"
#include "fmod_errors.h"

#define NEW_SOUND_ENGINE

typedef int CHANNEL;

#define soundEngine SoundEngine::Instance()

#ifdef NEW_SOUND_ENGINE

class SoundEngine
{
	const static float soundSpd;
	const static int maxChannel = 10000;
	int nowChannel;
	float volumeDes[10000];
    FMOD::System    *system;
	FMOD_RESULT      result;
	map<string,FMOD::Sound*> pool;
	FMOD::Channel *channel[maxChannel];
	SoundEngine(int MaxChannel = 100);
	~SoundEngine();
public:
	static SoundEngine& Instance(void) {static SoundEngine instance; return instance;}
	void ERRCHECK(FMOD_RESULT result);

	void ClearChannel();
	void ClearPool();

	void Init() {}
	void Update();
	bool Check(CHANNEL Ichannel) {return (Ichannel>=0&&Ichannel<=nowChannel);}
	
	CHANNEL GetNewChannel() {if(nowChannel<maxChannel)return nowChannel++; else return -1;}
	void PlaySound(string filename, float volume = 1.000);
	FMOD::Sound* ReadMusic(string filename);
	void PlayMusic(string filename, CHANNEL Ichannel = -1, bool loop = false, float volume = 1.000);

	//details control
	void StopMusic(CHANNEL Ichannel, bool alphaout = false);
	void StopAllMusic(bool alphaout = false);
	void PauseMusic(CHANNEL Ichannel, bool Pause);
	void PauseAllMusic(bool Pause);
};

#else

class SoundEngine
{
	const static float soundSpd;
	int maxChannel;
	int nowChannel;
	FMOD::System    *system;
	FMOD_RESULT      result;
	FMOD::Channel *channel[10000];
	FMOD::Sound *sound[10000];
	map<string,int> pool;
	float volumeDes[10000];
	SoundEngine(int MaxChannel = 10000);
    ~SoundEngine();
public:
	static SoundEngine& Instance(void) {static SoundEngine instance; return instance;}
	void ERRCHECK(FMOD_RESULT result);

	void Init() {}
	void Update();
	bool Check(CHANNEL Ichannel) {return (Ichannel>=0&&Ichannel<nowChannel);}
	//play music or sound
	CHANNEL GetNewChannel() {if(nowChannel<maxChannel)return nowChannel++; else return 0;}
	void ClearChannel() {nowChannel = 0;StopAllMusic();}
	void PlaySound(string filename, float volume = 1.000);
	CHANNEL ReadMusic(string filename, CHANNEL Ichannel = -1);
	CHANNEL PlayMusic(string filename, CHANNEL Ichannel = -1, bool loop = false, float volume = 1.000);

	//details control
	void StopMusic(CHANNEL Ichannel, bool alphaout = false);
	void StopAllMusic(bool alphaout = false);
	void PauseMusic(CHANNEL Ichannel, bool Pause);
};

#endif

#endif