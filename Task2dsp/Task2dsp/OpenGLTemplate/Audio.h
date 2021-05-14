#pragma once
#include <windows.h>									// Header File For The Windows Library
#include "./include/fmod_studio/fmod.hpp"
#include "./include/fmod_studio/fmod_errors.h"
#include "Game.h"
#include "Camera.h"

class CAudio
{
public:
	CAudio();
	~CAudio();
	bool Initialise();
	bool LoadEventSound(const char *filename);
	bool PlayEventSound();
	bool LoadMusicStream(const char *filename);
	bool PlayMusicStream();
	void Update(glm::vec3 pos, glm::vec3 pos1, glm::vec3 up, glm::vec3 front, glm::vec3 cam);
	bool Load_fighter_sound(const char* filename);
	bool Play_fighter_sound();
	bool Pause_fighter_sound();
	bool Unpause_fighter_sound();
	void Fighter_Update(glm::vec3 pos, glm::vec3 up, glm::vec3 front, glm::vec3 cam);
	bool Load_car_sound(const char* filename);
	bool Play_car_sound();
	void Car_Update(glm::vec3 pos, glm::vec3 up, glm::vec3 front, glm::vec3 cam);

private:
	void FmodErrorCheck(FMOD_RESULT result);


	FMOD_RESULT result;
	FMOD::System *m_FmodSystem;	// the global variable for talking to FMOD

	//Music Sounds
	FMOD::Sound *m_music;

	//Music Channels
	FMOD::Channel* m_musicChannel;

	//3D sounds for the environment
	FMOD::Channel* musicChannel2;
	FMOD::Channel* musicChannel3;
	FMOD::Sound* m_eventSound;
	FMOD::Sound* m_eventSound2;

};
