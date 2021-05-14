#pragma once
#include <windows.h>									// Header File For The Windows Library
#include "./include/fmod_studio/fmod.hpp"
#include "./include/fmod_studio/fmod_errors.h"
#include "Game.h"


class CAudio
{
public:
	CAudio();
	~CAudio();
	//For this task we will obtain the position of the camera in order to use it as a trigger for the event. 
	bool Initialise(CCamera* camera);
	bool LoadEventSound(char* filename);
	bool PlayEventSound();
	bool LoadMusicStream(char* filename);
	bool PlayMusicStream();
	void Update();

private:

	FMOD_RESULT result;
	FMOD::System* m_FmodSystem;	// the global variable for talking to FMOD
	FMOD::Sound* m_eventSound;

	FMOD::Sound* m_music;
	FMOD::Channel* m_musicChannel;

	FMOD::DSP* m_dsp;
};

void ChannelLoop(float* inbuffer, int inchannels, int sample, int currentChannel);
void LowerVolumeEvent(float* outbuffer, float* inbuffer, int sample, int currentChannel, int inchan, int* outchan);
void AddToRightChan(float value);
void AddToLeftChan(float value);
void AddToOldChan(float value);