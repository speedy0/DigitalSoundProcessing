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
	void Update(glm::vec3 pos, glm::vec3 up, glm::vec3 front, glm::vec3 cam);
	bool Load_car_sound(const char* filename);
	bool Play_car_sound();
	void Car_Update(glm::vec3 pos, glm::vec3 up, glm::vec3 front, glm::vec3 cam);

private:

	FMOD_RESULT result;
	FMOD::System* m_FmodSystem;	// the global variable for talking to FMOD
	FMOD::Sound* m_eventSound;

	FMOD::Sound* m_music;
	FMOD::Channel* m_musicChannel;

	FMOD::DSP* m_dsp;

	//For task2 dynamic filter
	FMOD::Channel* musicChannel2;
	FMOD::Sound* m_eventSound2;
};

void ChannelLoop(float* inbuffer, int inchannels, int sample, int currentChannel);
void LowerVolumeEvent(float* outbuffer, float* inbuffer, int sample, int currentChannel, int inchan, int* outchan);
void AddToRightChan(float value);
void AddToLeftChan(float value);
void AddToOldChan(float value);