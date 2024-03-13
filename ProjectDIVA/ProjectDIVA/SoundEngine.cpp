#include "SoundEngine.h"
#include "UIScreen.h"

const float SoundEngine::soundSpd = 0.03f/(1000.0f/55.0f);
extern int ScreenState;
extern CComPtr<IBasicAudio>	m_pAudio;		// audio

#ifdef NEW_SOUND_ENGINE

SoundEngine soundEngine = SoundEngine();

void SoundEngine::ERRCHECK(FMOD_RESULT result)
{
	if (result != FMOD_OK)
    {
        printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
		string erstr=FMOD_ErrorString(result);
		ErrorExit("FMOD error! "+erstr,-1);
        //exit(-1);
    }
}

SoundEngine::SoundEngine(int MaxChannel)
{
		unsigned int     version;
		FMOD_SPEAKERMODE speakermode;
		FMOD_CAPS        caps;

		nowChannel = 0;

		result = FMOD::System_Create(&system);
		ERRCHECK(result);
    
		result = system->getVersion(&version);
		ERRCHECK(result);

		if (version < FMOD_VERSION)
		{
			printf("Error!  You are using an old version of FMOD %08x.  This program requires %08x\n", version, FMOD_VERSION);
			return;
		}

		result = system->getDriverCaps(0, &caps, 0, &speakermode);
		ERRCHECK(result);

		result = system->setSpeakerMode(speakermode);
		ERRCHECK(result);

		if (caps & FMOD_CAPS_HARDWARE_EMULATED)      
		{                                            
			result = system->setDSPBufferSize(1024, 10);
			ERRCHECK(result);
		}

		result = system->init(MaxChannel, FMOD_INIT_NORMAL, 0); 
		if (result == FMOD_ERR_OUTPUT_CREATEBUFFER)     
		{
			result = system->setSpeakerMode(FMOD_SPEAKERMODE_STEREO);
			ERRCHECK(result);
                
			result = system->init(MaxChannel, FMOD_INIT_NORMAL, 0); 
			ERRCHECK(result);
		}
		ClearChannel();
		ClearPool();

		system->createSound("gamedata\\hit.wav",FMOD_HARDWARE, 0, &hitWav);
}

SoundEngine::~SoundEngine()
{
	ClearChannel();
	ClearPool();
	result = system->close();
    ERRCHECK(result);
    result = system->release();
    ERRCHECK(result);
}

void SoundEngine::PlayHit(float factor)
{
	FMOD::Channel *channel;
	system->playSound(FMOD_CHANNEL_FREE, hitWav, true, &channel);
	channel->setVolume(systemIni.sndVolume*factor);
	channel->setPaused(false);
	//ERRCHECK(result);
	//ERRCHECK(result);
}

void SoundEngine::Update(float dwTimeMilliSecond)
{
	system->update();

	if(ScreenState==SCREEN_GAMEMAIN) if(curr_music){
		FMOD_OPENSTATE state;
		if(curr_music->getOpenState(&state,NULL,NULL,NULL)==FMOD_OK){
			if(!state) if(m_pAudio) m_pAudio->put_Volume(0);
		}
	}

	for(int i = 0; i < nowChannel; i++)
	{
		if(channel[i]!=NULL)
		{
			float volume;
			result = channel[i]->getVolume(&volume);
			if(volume>volumeDes[i])
			{
				volume -= soundSpd*dwTimeMilliSecond;
				if(volume<volumeDes[i]) volume = volumeDes[i];
			}
			if(volume<volumeDes[i])
			{
				volume += soundSpd*dwTimeMilliSecond;
				if(volume>volumeDes[i]) volume = volumeDes[i];
			}
			channel[i]->setVolume(volume);
		}
	}
}

void SoundEngine::ClearChannel()
{
	StopAllMusic();
	nowChannel = 0;
	memset(volumeDes,0,sizeof(volumeDes));
}

void SoundEngine::ClearPool()
{
	StopAllMusic();
	for(map<string,FMOD::Sound*>::iterator ptr = pool.begin(); ptr != pool.end(); ptr++)
		ptr->second->release();
	pool.clear();
}

void SoundEngine::PlaySound(string filename, float volume)
{
	FMOD::Sound *sound;
	FMOD::Channel *channel;
	sound = ReadMusic(filename);
	result = system->playSound(FMOD_CHANNEL_FREE, sound, true, &channel);
	ERRCHECK(result);
	result = channel->setVolume((float)volume);
	ERRCHECK(result);
	result = channel->setPaused(false);
	ERRCHECK(result);
}

FMOD::Sound* SoundEngine::ReadMusic(string filename)
{
	if(pool.find(filename)!=pool.end())
		return pool[filename];
	FMOD::Sound *sound;
	result = system->createStream(filename.data(), FMOD_HARDWARE, 0, &sound);
	pool[filename] = sound;
	return sound;
}

FMOD::Sound* SoundEngine::PlayMusic(string filename, CHANNEL Ichannel, bool loop, float volume)
{
	if(Ichannel<0||Ichannel>=maxChannel)
	{
		Ichannel = GetNewChannel();
		if(Ichannel<0)
		{
			PlaySound(filename,volume);
			return NULL;
		}
	}
	nowChannel = max(nowChannel,Ichannel+1);
	FMOD::Sound *sound = ReadMusic(filename);
	if(loop)
	{
		result = sound->setMode(FMOD_LOOP_NORMAL);
		ERRCHECK(result);
	}
	result = system->playSound(FMOD_CHANNEL_FREE, sound, true, &channel[Ichannel]);
	if(result!=FMOD_OK)
		result = result;
    //ERRCHECK(result);
	volumeDes[Ichannel] = volume;
	result = channel[Ichannel]->setVolume((float)volume);
	if(result!=FMOD_OK)
		result = result;
	//ERRCHECK(result);
	result = channel[Ichannel]->setPaused(false);
	if(result!=FMOD_OK)
		result = result;
	//ERRCHECK(result);
	return sound;
}

void SoundEngine::StopMusic(CHANNEL Ichannel, bool alphaout)
{
	if(!Check(Ichannel))
		return;
	bool iP1;
	if(channel[Ichannel]!=NULL)
	{
		if(!alphaout)
		{
			result = channel[Ichannel]->isPlaying(&iP1);
			if(iP1)
			{
				result = channel[Ichannel]->stop();
			}
			channel[Ichannel] = NULL;
		}
		volumeDes[Ichannel] = 0;
	}
}

void SoundEngine::StopAllMusic(bool alphaout)
{
	bool iP1;
	curr_music = NULL;
	for(int i = 0; i < nowChannel; i++)
		if(channel[i]!=NULL)
		{
			if(!alphaout)
			{
				result = channel[i]->isPlaying(&iP1);
				if(iP1)
				{
					result = channel[i]->stop();
				}
				channel[i] = NULL;
			}
			volumeDes[i] = 0;
		}
}

void SoundEngine::PauseMusic(CHANNEL Ichannel, bool Pause)
{
	if(!Check(Ichannel))
		return;
	channel[Ichannel]->setPaused(Pause);
}

void SoundEngine::PauseAllMusic(bool Pause)
{
	for(int i = 0; i < nowChannel; i++)
		if(channel[i]!=NULL)
			channel[i]->setPaused(Pause);
}

#else

void SoundEngine::ERRCHECK(FMOD_RESULT result)
{
	if (result != FMOD_OK)
    {
        printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
		string erstr=FMOD_ErrorString(result);
		ErrorExit("FMOD error! "+erstr,-1);
        //exit(-1);
    }
}

SoundEngine::SoundEngine(int MaxChannel)
{
		unsigned int     version;
		FMOD_SPEAKERMODE speakermode;
		FMOD_CAPS        caps;

		this->maxChannel = MaxChannel, this->nowChannel = 0;

		result = FMOD::System_Create(&system);
		ERRCHECK(result);
    
		result = system->getVersion(&version);
		ERRCHECK(result);

		if (version < FMOD_VERSION)
		{
			printf("Error!  You are using an old version of FMOD %08x.  This program requires %08x\n", version, FMOD_VERSION);
			return;
		}

		result = system->getDriverCaps(0, &caps, 0, 0, &speakermode);
		ERRCHECK(result);

		result = system->setSpeakerMode(speakermode);
		ERRCHECK(result);

		if (caps & FMOD_CAPS_HARDWARE_EMULATED)      
		{                                            
			result = system->setDSPBufferSize(1024, 10);
			ERRCHECK(result);
		}

		result = system->init(1000, FMOD_INIT_NORMAL, 0); 
		if (result == FMOD_ERR_OUTPUT_CREATEBUFFER)     
		{
			result = system->setSpeakerMode(FMOD_SPEAKERMODE_STEREO);
			ERRCHECK(result);
                
			result = system->init(MaxChannel, FMOD_INIT_NORMAL, 0); 
			ERRCHECK(result);
		}
		memset(channel,0,sizeof(channel));
		memset(sound,0,sizeof(sound));
		memset(volumeDes,0,sizeof(volumeDes));
}

SoundEngine::~SoundEngine()
{
	result = system->close();
    ERRCHECK(result);
    result = system->release();
    ERRCHECK(result);
}

void SoundEngine::PlaySound(string filename, float volume)
{
	FMOD::Sound *sound;
	FMOD::Channel *channel;
	result = system->createSound(filename.data(), FMOD_HARDWARE, 0, &sound);
	ERRCHECK(result);
	result = system->playSound(FMOD_CHANNEL_FREE, sound, true, &channel);
	ERRCHECK(result);
	result = channel->setVolume((float)volume);
	ERRCHECK(result);
	result = channel->setPaused(false);
	ERRCHECK(result);
}

CHANNEL SoundEngine::ReadMusic(string filename, CHANNEL Ichannel)
{
	if(Ichannel==-1&&pool.find(filename)!=pool.end())
	{
		Ichannel = pool[filename];
		StopMusic(Ichannel);
		if(sound[Ichannel])
			return Ichannel;
	}
	if(!Check(Ichannel))
		Ichannel = GetNewChannel();
	StopMusic(Ichannel);
	sound[Ichannel]->release();
	result = system->createStream(filename.data(), FMOD_HARDWARE, 0, &sound[Ichannel]);
	pool[filename] = Ichannel;
	return Ichannel;
}

CHANNEL SoundEngine::PlayMusic(string filename, CHANNEL Ichannel, bool loop, float volume)
{
	Ichannel = ReadMusic(filename,Ichannel);
	if(loop)
	{
		result = sound[Ichannel]->setMode(FMOD_LOOP_NORMAL);
		ERRCHECK(result);
	}
	result = system->playSound(FMOD_CHANNEL_FREE, sound[Ichannel], true, &channel[Ichannel]);
    ERRCHECK(result);
	volumeDes[Ichannel] = volume;
	result = channel[Ichannel]->setVolume((float)volume);
	ERRCHECK(result);
	result = channel[Ichannel]->setPaused(false);
	ERRCHECK(result);
	return Ichannel;
}


void SoundEngine::Update()
{
	float volume;
	for(int i = 0; i < nowChannel; i++)
		if(channel[i]!=NULL)
		{
			result = channel[i]->getVolume(&volume);
			if(volume>volumeDes[i])
			{
				volume -= soundSpd;
				if(volume<volumeDes[i]) volume = volumeDes[i];
			}
			if(volume<volumeDes[i])
			{
				volume += soundSpd;
				if(volume>volumeDes[i]) volume = volumeDes[i];
			}
			channel[i]->setVolume(volume);
		}
}

void SoundEngine::StopMusic(CHANNEL Ichannel, bool alphaout)
{
	if(!Check(Ichannel))
		return;
	bool iP1;
	if(channel[Ichannel]!=NULL)
	{
		if(!alphaout)
		{
			result = channel[Ichannel]->isPlaying(&iP1);
			if(iP1)
			{
				result = channel[Ichannel]->stop();
			}
			channel[Ichannel] = NULL;
		}
		volumeDes[Ichannel] = 0;
	}
}

void SoundEngine::StopAllMusic(bool alphaout)
{
	bool iP1;
	for(int i = 0; i < nowChannel; i++)
		if(channel[i]!=NULL)
		{
			if(!alphaout)
			{
				result = channel[i]->isPlaying(&iP1);
				if(iP1)
				{
					result = channel[i]->stop();
				}
				channel[i] = NULL;
			}
			volumeDes[i] = 0;
		}
}

void SoundEngine::PauseMusic(CHANNEL Ichannel, bool Pause)
{
	if(!Check(Ichannel))
		return;
	channel[Ichannel]->setPaused(Pause);
}

#endif