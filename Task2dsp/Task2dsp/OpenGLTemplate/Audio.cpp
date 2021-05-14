#include "Audio.h"

#pragma comment(lib, "lib/fmod_vc.lib")

CAudio::CAudio()
{}

CAudio::~CAudio()
{}

bool CAudio::Initialise()
{
	// Create an FMOD system
	result = FMOD::System_Create(&m_FmodSystem);
	FmodErrorCheck(result);
	if (result != FMOD_OK) 
		return false;

	// Initialise the system
	result = m_FmodSystem->init(32, FMOD_INIT_NORMAL, 0);
	FmodErrorCheck(result);
	if (result != FMOD_OK) 
		return false;

	return true;
	
}

// Load an event sound
bool CAudio::LoadEventSound(const char *filename)
{
	result = m_FmodSystem->createSound(filename, NULL, 0, &m_eventSound);
	FmodErrorCheck(result);
	if (result != FMOD_OK) 
		return false;

	return true;
}

// Play an event sound
bool CAudio::PlayEventSound()
{
	result = m_FmodSystem->playSound(m_eventSound, NULL, false, NULL);
	FmodErrorCheck(result);
	if (result != FMOD_OK)
		return false;
	return true;
}

// Load a music stream
bool CAudio::LoadMusicStream(const char *filename)
{
	result = m_FmodSystem->createStream(filename, NULL | FMOD_LOOP_NORMAL, 0, &m_music);
	FmodErrorCheck(result);

	if (result != FMOD_OK) 
		return false;

	return true;
}

// Play a music stream
bool CAudio::PlayMusicStream()
{
	result = m_FmodSystem->playSound(m_music, NULL, false, &m_musicChannel);
	//Sets music channel to lower volume. 
	result = m_musicChannel->setVolume(0.1);
	FmodErrorCheck(result);

	if (result != FMOD_OK)
		return false;
	return true;
}

// Check for error
void CAudio::FmodErrorCheck(FMOD_RESULT result)
{						
	if (result != FMOD_OK) {
		const char *errorString = FMOD_ErrorString(result);
		// MessageBox(NULL, errorString, "FMOD Error", MB_OK);
		// Warning: error message commented out -- if headphones not plugged into computer in lab, error occurs
	}
}

void CAudio::Update(glm::vec3 pos, glm::vec3 pos1, glm::vec3 up, glm::vec3 front, glm::vec3 cam)
{
	Car_Update(pos1, up, front, cam);

	Fighter_Update(pos, up, front, cam);

	m_FmodSystem->update();
}

//Fighter Sounds:
//Loads 3D sound onto the game. 
bool CAudio::Load_fighter_sound(const char* filename) {
	result = m_FmodSystem->createSound(filename, FMOD_3D | FMOD_LOOP_NORMAL, 0, &m_eventSound);
	FmodErrorCheck(result);
	if (result != FMOD_OK)
		return false;

	result = m_FmodSystem->set3DSettings(1.0f, 1.0f, 0.2f);

	return true;
}

//Plays a 3D sound. 
bool CAudio::Play_fighter_sound() {
	result = m_FmodSystem->playSound(m_eventSound, NULL, false, &musicChannel2);
	result = musicChannel2->setVolume(0.6);

	FmodErrorCheck(result);
	if (result != FMOD_OK)
		return false;
	return true;
}

//Pauses 3D sound on music channel2.
bool CAudio::Pause_fighter_sound() {
	result = musicChannel2->setPaused(true);
	FmodErrorCheck(result);
	if (result != FMOD_OK)
		return false;
	return true;
}

//Unpauses 3D sound on music Channel2. 
bool CAudio::Unpause_fighter_sound() {
	result = musicChannel2->setPaused(false);
	FmodErrorCheck(result);
	if (result != FMOD_OK)
		return false;
	return true;
}

//Fighter Update:
void CAudio::Fighter_Update(glm::vec3 pos, glm::vec3 up, glm::vec3 front, glm::vec3 cam) {
	FMOD_VECTOR Fmodposition, Fmodvelocity;

	Fmodposition.x = pos.x;
	Fmodposition.y = pos.y;
	Fmodposition.z = pos.z;
	Fmodvelocity.x = 10.0f;
	Fmodvelocity.y = 10.0f;
	Fmodvelocity.z = 10.0f;

	//Sound settings
	musicChannel2->set3DSpread(360);
	musicChannel2->set3DMinMaxDistance(1.0, 500.0);
	musicChannel2->set3DAttributes(&Fmodposition, &Fmodvelocity);

	//Listener
	FMOD_VECTOR listener_position, listener_velocity, up_vector, view_vector;

	glm::vec3 cam_pos = cam;
	glm::vec3 dir = front;
	dir = glm::normalize(dir);
	glm::vec3 upv = up;

	listener_position.x = cam_pos.x;
	listener_position.y = cam_pos.y;
	listener_position.z = cam_pos.z;

	listener_velocity.x = 10;
	listener_velocity.y = 10;
	listener_velocity.z = 10;

	up_vector.x = upv.x;
	up_vector.y = upv.y;
	up_vector.z = upv.z;

	view_vector.x = dir.x;
	view_vector.y = dir.y;
	view_vector.z = dir.z;

	m_FmodSystem->set3DListenerAttributes(0, &listener_position, &listener_velocity, &view_vector, &up_vector);
}

//Car engine Sounds:
//Loads 3D sound onto the game. 
bool CAudio::Load_car_sound(const char* filename) {
	result = m_FmodSystem->createSound(filename, FMOD_3D | FMOD_LOOP_NORMAL, 0, &m_eventSound2);
	FmodErrorCheck(result);
	if (result != FMOD_OK)
		return false;

	result = m_FmodSystem->set3DSettings(1.0f, 1.0f, 0.2f);

	return true;
}

//Plays a 3D sound. 
bool CAudio::Play_car_sound() {
	result = m_FmodSystem->playSound(m_eventSound2, NULL, false, &musicChannel3);
	result = musicChannel3->setVolume(0.2);

	FmodErrorCheck(result);
	if (result != FMOD_OK)
		return false;
	return true;
}

//Car Update:
void CAudio::Car_Update(glm::vec3 pos, glm::vec3 up, glm::vec3 front, glm::vec3 cam) {
	FMOD_VECTOR Fmodposition, Fmodvelocity;

	Fmodposition.x = pos.x;
	Fmodposition.y = pos.y;
	Fmodposition.z = pos.z;
	Fmodvelocity.x = 10.0f;
	Fmodvelocity.y = 10.0f;
	Fmodvelocity.z = 10.0f;

	//Sound settings
	musicChannel3->set3DSpread(360);
	musicChannel3->set3DMinMaxDistance(1.0, 500.0);
	musicChannel3->set3DAttributes(&Fmodposition, &Fmodvelocity);

	//Sets the listener
	FMOD_VECTOR list_pos, list_vel, up_vector, view_vector;

	glm::vec3 cam_pos = cam;
	glm::vec3 dir = front;
	dir = glm::normalize(dir);
	glm::vec3 upv = up;

	list_pos.x = cam_pos.x;
	list_pos.y = cam_pos.y;
	list_pos.z = cam_pos.z;

	list_vel.x = 10;
	list_vel.y = 10;
	list_vel.z = 10;

	up_vector.x = upv.x;
	up_vector.y = upv.y;
	up_vector.z = upv.z;

	view_vector.x = dir.x;
	view_vector.y = dir.y;
	view_vector.z = dir.z;

	m_FmodSystem->set3DListenerAttributes(0, &list_pos, &list_vel, &view_vector, &up_vector);
}