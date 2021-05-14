#include "Audio.h"
#include "Circularbuffer.h"
#include "Camera.h"
#pragma comment(lib, "lib/fmod_vc.lib")

//For this task we will obtain the position of the camera in order to use it as a trigger for the event. 
//Initialises Camera
CCamera* cam;

//Created two channels and used length of 4096 samples for the circular buffer. 
//We also created an old buffer that will be used for Convolution. 
Circularbuffer<float> LeftChannel(4098);
Circularbuffer<float> RightChannel(4098);
Circularbuffer<float> Oldbuffer(4098);

//Function to add a value to a right channel.
void AddToRChan(float value) {
	RightChannel.AddToBuffer(value);
}

//Function to add a value to a left channel.
void AddToLChan(float value) {
	LeftChannel.AddToBuffer(value);
}

//Function to add a value to a Old channel.
void AddToOChan(float value) {
	Oldbuffer.AddToBuffer(value);
}

// Check for the Fmod error
void FmodErrorCheck(FMOD_RESULT result)
{
	if (result != FMOD_OK) {
		const char* errorString = FMOD_ErrorString(result);
		// MessageBox(NULL, errorString, "FMOD Error", MB_OK);
		// Warning: error message commented out -- if headphones not plugged into computer in lab, error occurs
	}
}

//Convolution method used for the low-pass FIR filter. It is used as a manipulation of the input by another. Code is inspired by Weyde (2021).
static void LPFConvolution(int n, float* input, float* output, float* coeff, int coef_size, float* length, const int samples, Circularbuffer<float>* oldbuffer, float coefScale = 1) {
	//Sets output to be equal to the input.
	*output = *input;

	//Creates a variable that will be represent length arrya with an index.
	auto changesamp = length[(samples - 1) - n];
	//Ensures that output equals to coefficient * our variable. 
	*output = coeff[0] * changesamp;

	for (int i = 1; i <= coef_size - 1; i++) {
		//Adds cooefficients onto the output.
		*output += oldbuffer->DisplayPreviousPosition(-i) * (coeff[i] * coefScale);
	}

	//Adds input (x) to the old buffer at the circular buffer. 
	AddToOChan(*input);
}

//Convolution method used for the high-pass FIR filter. It is used as a manipulation of the input by another. Code is inspired by Weyde (2021).
static void HPFConvolution(int n, float* input, float* output, float* coeff, int coef_size, float* length, const int samples, Circularbuffer<float>* oldbuffer, float coefScale = 1) {
	//Sets output to be equal to the input.
	*output = *input;

	//Creates a variable that will be represent length arrya with an index.
	auto changesamp = length[(samples - 1) - n];
	//Ensures that output equals to coefficient * our variable. 
	*output = coeff[0] * changesamp;

	for (int i = 1; i <= coef_size - 1; i++) {
		//Adds cooefficients onto the output.
		*output += oldbuffer->DisplayPreviousPosition(-i) * (coeff[i] * coefScale);
	}

	//Adds input (x) to the old buffer at the circular buffer. 
	AddToOChan(*input);
}

//Coefficients for Low-pass filter.
float LPFCoefficients[255] = {
	0.000111619,	5.22E-05,	6.39E-05,	7.70E-05,
	9.15E-05,	0.00010734,	0.000124654,	0.000143372,
	0.000163458,	0.000184838,	0.000207434,	0.000231128,
	0.000255806,	0.000281306,	0.000307465,	0.000334052,
	0.000360832,	0.000387512,	0.000413798,	0.000439355,
	0.000463853,	0.000486897,	0.000508091,	0.00052697,
	0.000543094,	0.000555987,	0.000565208,	0.000570242,
	0.000570574,	0.000565643,	0.00055501,	0.000538177,
	0.00051465,	0.0004838,	0.000445379,	0.00039889,
	0.000343811,	0.000279998,	0.000206939,	0.0001245,
	3.24E-05, -6.95E-05, -0.000181262, -0.000302883,
	-0.000434318, -0.000575386, -0.000725865, -0.000885395,
	-0.001053564, -0.001229818, -0.001413554, -0.00160402,
	-0.001800393, -0.002001715, -0.002206961, -0.00241498,
	-0.002624552, -0.002834318, -0.003042859, -0.003248652,
	-0.003450133, -0.003645615, -0.003833369, -0.004011597,
	-0.004178494, -0.004332162, -0.004470698, -0.004592193,
	-0.00469476, -0.004776417, -0.004835356, -0.004869678,
	-0.004877596, -0.004857378, -0.004807347, -0.004725945,
	-0.004611693, -0.00446325, -0.004279394, -0.004059049,
	-0.003801297, -0.003505377, -0.003170725, -0.002796943,
	-0.002383842, -0.001931427, -0.001439917, -0.000909751,
	-0.000341584,	0.000263718,	0.000905055,	0.001581103,
	0.00229032,	0.003030955,	0.003801033,	0.004598372,
	0.005420594,	0.006265139,	0.007129255,	0.008010021,
	0.008904374,	0.009809112,	0.010720884,	0.011636263,
	0.012551722,	0.01346364,	0.014368388,	0.015262261,
	0.016141576,	0.017002638,	0.017841784,	0.018655415,
	0.019439987,	0.020192065,	0.020908306,	0.02158552,
	0.022220657,	0.022810838,	0.02335338,	0.023845785,
	0.024285792,	0.02467137,	0.025000721,	0.025272314,
	0.025484885,	0.025637438,	0.025729255,	0.025759908,
	0.025729255,	0.025637438,	0.025484885,	0.025272314,
	0.025000721,	0.02467137,	0.024285792,	0.023845785,
	0.02335338,	0.022810838,	0.022220657,	0.02158552,
	0.020908306,	0.020192065,	0.019439987,	0.018655415,
	0.017841784,	0.017002638,	0.016141576,	0.015262261,
	0.014368388,	0.01346364,	0.012551722,	0.011636263,
	0.010720884,	0.009809112,	0.008904374,	0.008010021,
	0.007129255,	0.006265139,	0.005420594,	0.004598372,
	0.003801033,	0.003030955,	0.00229032,	0.001581103,
	0.000905055,	0.000263718, -0.000341584, -0.000909751,
	-0.001439917, -0.001931427, -0.002383842, -0.002796943,
	-0.003170725, -0.003505377, -0.003801297, -0.004059049,
	-0.004279394, -0.00446325, -0.004611693, -0.004725945,
	-0.004807347, -0.004857378, -0.004877596, -0.004869678,
	-0.004835356, -0.004776417, -0.00469476, -0.004592193,
	-0.004470698, -0.004332162, -0.004178494, -0.004011597,
	-0.003833369, -0.003645615, -0.003450133, -0.003248652,
	-0.003042859, -0.002834318, -0.002624552, -0.00241498,
	-0.002206961, -0.002001715, -0.001800393, -0.00160402,
	-0.001413554, -0.001229818, -0.001053564, -0.000885395,
	-0.000725865, -0.000575386, -0.000434318, -0.000302883,
	-0.000181262, -6.95E-05,	3.24E-05,	0.0001245,
	0.000206939,	0.000279998,	0.000343811,	0.00039889,
	0.000445379,	0.0004838,	0.00051465,	0.000538177,
	0.00055501,	0.000565643,	0.000570574,	0.000570242,
	0.000565208,	0.000555987,	0.000543094,	0.00052697,
	0.000508091,	0.000486897,	0.000463853,	0.000439355,
	0.000413798,	0.000387512,	0.000360832,	0.000334052,
	0.000307465,	0.000281306,	0.000255806,	0.000231128,
	0.000207434,	0.000184838,	0.000163458,	0.000143372,
	0.000124654,	0.00010734,	9.15E-05,	7.70E-05,
	6.39E-05,	5.22E-05,	0.000111619

};

//Coefficients for high-pass filter.
float HPFCoefficients[277] = {
	//Add Coefficients here
	0.012499406, -0.002314519, -0.002115093, -0.001935187, 
	-0.001773281, -0.001626091, -0.001492117, -0.001368459,
	-0.001253625, -0.001145126, -0.001041906, -0.000941701,
	-0.000843559, -0.00074557, -0.000647772, -0.000548394,
	-0.00044737, -0.000343426, -0.000237513, -0.000128937,
	-1.76E-05,	9.59E-05,	0.000210336,	0.000326813,
	0.000442205,	0.000557306,	0.000670155,	0.00077978,
	0.000884428,	0.000983581,	0.00107498,	0.001157824,
	0.001229656,	0.001290584,	0.001337818,	0.001371524,
	0.001388749,	0.001390607,	0.001373761,	0.00134053,
	0.00128586,	0.001219867,	0.001116607,	0.001022269,
	0.000891889,	0.000738131,	0.000578052,	0.000404344,
	0.000216131,	1.10E-05, -0.000205389, -0.00043034,
	-0.000658594, -0.000889364, -0.001119514, -0.001348451,
	-0.001572123, -0.001789016, -0.001994774, -0.002186802,
	-0.002360114, -0.002512459, -0.002641053, -0.002743314,
	-0.00281562, -0.002858455, -0.002867372, -0.002842211,
	-0.00278119, -0.002682757, -0.002546194, -0.002371499,
	-0.002158364, -0.001907734, -0.00162022, -0.001298415,
	-0.000943499, -0.000558795, -0.000146546,	0.000288413,
	0.000743427,	0.001212584,	0.001693733,	0.002177203,
	0.002666,	0.003143288,	0.003611923,	0.004065813,
	0.004491885,	0.004888833,	0.005251126,	0.005572429,
	0.0058441,	0.006060868,	0.006217446,	0.006310038,
	0.006332845,	0.006281249,	0.006149901,	0.005936375,
	0.005637886,	0.005252817,	0.004778692,	0.004215184,
	0.003562398,	0.002820404,	0.001988756,	0.001072761,
	7.32E-05, -0.001005796, -0.002158634, -0.003382034,
	-0.004669136, -0.006014192, -0.007410014, -0.008849332,
	-0.010323829, -0.011824533, -0.013342637, -0.014868761,
	-0.016393489, -0.017906768, -0.019399583, -0.020861599,
	-0.022284317, -0.023655721, -0.024970025, -0.02621432,
	-0.027381967, -0.028466948, -0.029457263, -0.030348841,
	-0.031135369, -0.03181073, -0.032368895, -0.032807269,
	-0.03312294, -0.03331411,	0.966622126, -0.03331411,
	-0.03312294, -0.032807269, -0.032368895, -0.03181073,
	-0.031135369, -0.030348841, -0.029457263, -0.028466948,
	-0.027381967, -0.02621432, -0.024970025, -0.023655721,
	-0.022284317, -0.020861599, -0.019399583, -0.017906768,
	-0.016393489, -0.014868761, -0.013342637, -0.011824533,
	-0.010323829, -0.008849332, -0.007410014, -0.006014192,
	-0.004669136, -0.003382034, -0.002158634,  -0.001005796,
	7.32E-05,	0.001072761,	0.001988756,	0.002820404,
	0.003562398,	0.004215184,	0.004778692,	0.005252817,
	0.005637886,	0.005936375,	0.006149901,	0.006281249,
	0.006332845,	0.006310038,	0.006217446,	0.006060868,
	0.0058441,	0.005572429,	0.005251126,	0.004888833,
	0.004491885,	0.004065813,	0.003611923,	0.003143288,
	0.002666,	0.002177203,	0.001693733,	0.001212584,
	0.000743427,	0.000288413, -0.000146546, -0.000558795,
	-0.000943499, -0.001298415, -0.00162022, -0.001907734,
	-0.002158364, -0.002371499, -0.002546194, -0.002682757,
	-0.00278119, -0.002842211, -0.002867372, -0.002858455,
	-0.00281562, -0.002743314, -0.002641053, -0.002512459,
	-0.002360114, -0.002186802, -0.001994774, -0.001789016,
	-0.001572123, -0.001348451, -0.001119514, -0.000889364,
	-0.000658594, -0.00043034, -0.000205389,	1.10E-05,
	0.000216131,	0.000404344,	0.000578052,	0.000738131,
	0.000891889,	0.001022269,	0.001116607,	0.001219867,
	0.00128586,	0.00134053,	0.001373761,	0.001390607,
	0.001388749,	0.001371524,	0.001337818,	0.001290584,
	0.001229656,	0.001157824,	0.00107498,	0.000983581,
	0.000884428,	0.00077978,	0.000670155,	0.000557306,
	0.000442205,	0.000326813,	0.000210336,	9.59E-05,
	-1.76E-05, - 0.000128937, -0.000237513, -0.000343426,
	-0.00044737, -0.000548394, -0.000647772, -0.00074557,
	-0.000843559, -0.000941701, -0.001041906, -0.001145126,
	-0.001253625, -0.001368459, -0.001492117, -0.001626091,
	-0.001773281, -0.001935187, -0.002115093, -0.002314519,
	0.012499406

};

//Function used in DSPCallback for channel loop. 
void ChannelLoop(float* inbuffer, int inchannels, int sample, int currentChannel) {
//Creates a float which indicate inbuffer calculation through each channel.
//This float will be used to add correct inbuffer onto the each channel. 
	float* inbuff = &inbuffer[(inchannels * sample) + currentChannel];

	if (currentChannel == 0) {
		AddToRChan(*inbuff);
	}

	if (currentChannel == 1) {
		AddToLChan(*inbuff);
	}
}

//Halves the volume when the player is below 0 on Y-axis. 
void LowerVolumeEvent(float* outbuffer, float* inbuffer, int sample, int currentChannel, int inchan, int* outchan) {
	if (cam->GetPosition().y < 0 && cam->GetPosition().y > -100) {
		outbuffer[(sample * *outchan) + currentChannel] = inbuffer[(sample * inchan) + currentChannel] * 0.2f;
	}
}

// DSP callback method that will be used for Digital Sound Processing. Code inspired by Mathews(2020) & Weyde(2021). 
FMOD_RESULT F_CALLBACK DSPCallback(FMOD_DSP_STATE* dsp_state, float* inbuffer, float* outbuffer, unsigned int length, int inchannels, int* outchannels)
{
	FMOD::DSP* thisdsp = (FMOD::DSP*)dsp_state->instance;

	//Loops through all the samples in the size. 
	for (unsigned int samp = 0; samp < length; samp++)
	{
		//Loops through all the channels in specified outchannels when called in DSPCallback Method. 
		for (int chan = 0; chan < *outchannels; chan++)
		{
			ChannelLoop(inbuffer, inchannels, samp, chan);
			
			//If the camera is to be found below 0 on Y-axis, the volume will be halved. 
			LowerVolumeEvent(outbuffer, inbuffer, samp, chan, inchannels, outchannels);
		}

		//Manipulates vectors of the channels and introduces outbuffer and inbuffer calculations to be used for the convolution.  
		float* leftSide = LeftChannel.ToArray();
		float* rightSide = RightChannel.ToArray();
		float* outbuff = &outbuffer[(*outchannels * samp) + 0];
		float* inbuffe = &inbuffer[(inchannels * samp) + 0];

		//Low-pass FIR filter
		if (cam->NPFilter() == true) {
			LPFConvolution(samp, inbuffe, outbuff, LPFCoefficients, 255, rightSide, length, &Oldbuffer, cam->GetPosition().x + 1);
			LPFConvolution(samp, inbuffe + 1, outbuff + 1, LPFCoefficients, 255, leftSide, length, &Oldbuffer, cam->GetPosition().x + 1);
		}

		//High-pass FIR Filter
		if (cam->HPFilter() == true) {
			HPFConvolution(samp, inbuffe, outbuff, HPFCoefficients, 277, rightSide, length, &Oldbuffer, cam->GetPosition().x + 1);
			HPFConvolution(samp, inbuffe + 1, outbuff + 1, HPFCoefficients, 277, leftSide, length, &Oldbuffer, cam->GetPosition().x + 1);
		}
	}

	return FMOD_OK;
}

CAudio::CAudio()
{}

CAudio::~CAudio()
{}

bool CAudio::Initialise(CCamera* camera)
{
	cam = camera;

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



	// Create the DSP effect
	{
		FMOD_DSP_DESCRIPTION dspdesc;
		memset(&dspdesc, 0, sizeof(dspdesc));

		strncpy_s(dspdesc.name, "My first DSP unit", sizeof(dspdesc.name));
		dspdesc.numinputbuffers = 1;
		dspdesc.numoutputbuffers = 1;
		dspdesc.read = DSPCallback;

		result = m_FmodSystem->createDSP(&dspdesc, &m_dsp);
		FmodErrorCheck(result);

		if (result != FMOD_OK)
			return false;
	}

	return true;

}

// Load an event sound
bool CAudio::LoadEventSound(char* filename)
{
	result = m_FmodSystem->createSound(filename, FMOD_LOOP_OFF, 0, &m_eventSound);
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
bool CAudio::LoadMusicStream(char* filename)
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
	FmodErrorCheck(result);

	if (result != FMOD_OK)
		return false;

	m_musicChannel->addDSP(0, m_dsp);

	return true;
}

void CAudio::Update(glm::vec3 pos, glm::vec3 up, glm::vec3 front, glm::vec3 cam)
{
	Car_Update(pos, up, front, cam);

	m_FmodSystem->update();
}

//Car engine Sounds:
//Loads 3D sound onto the game. 
bool CAudio::Load_car_sound(const char* filename) {
	result = m_FmodSystem->createSound(filename, FMOD_3D | FMOD_LOOP_NORMAL, 0, &m_eventSound2);
	FmodErrorCheck(result);
	if (result != FMOD_OK)
		return false;

	musicChannel2->addDSP(1, m_dsp);
	result = m_FmodSystem->set3DSettings(1.0f, 1.0f, 0.2f);

	return true;
}

//Plays a 3D sound. 
bool CAudio::Play_car_sound() {
	result = m_FmodSystem->playSound(m_eventSound, NULL, false, &musicChannel2);
	result = musicChannel2->setVolume(0.6);

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
	musicChannel2->set3DSpread(360);
	musicChannel2->set3DMinMaxDistance(1.0, 500.0);
	musicChannel2->set3DAttributes(&Fmodposition, &Fmodvelocity);

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

//Coefficients made in Python sourced from: Stuart Matthews (2020)
//1.32727108e-09, 7.07530209e-09, 2.35903552e-08, 6.13798167e-08,
//1.35025413e-07, 2.61011214e-07, 4.52468561e-07, 7.10288692e-07,
//1.01105268e-06, 1.29375661e-06, 1.44915107e-06, 1.31721749e-06,
//6.99215233e-07, -6.09896576e-07, -2.76567778e-06, -5.79738729e-06
//- 9.51992478e-06 - 1.34528799e-05 - 1.67727103e-05 - 1.83283097e-05
//- 1.67482831e-05 - 1.06578868e-05, 9.96105124e-07, 1.85421958e-05,
//4.11644680e-05, 6.65585613e-05, 9.07756300e-05, 1.08367774e-04,
//1.12923355e-04, 9.80256825e-05, 5.85860953e-05, -7.59809191e-06,
//-9.83121612e-05, -2.05751240e-04, -3.16073463e-04, -4.10013742e-04
//- 4.64779102e-04 - 4.57251857e-04 - 3.68268824e-04 - 1.87459443e-04,
//8.21355058e-05, 4.20619816e-04, 7.89962302e-04, 1.13572036e-03,
//1.39225130e-03, 1.49135940e-03, 1.37366812e-03, 1.00132456e-03,
//3.70063355e-04, -4.81689159e-04, -1.46898551e-03, -2.46368958e-03,
//-3.30599838e-03, -3.82395788e-03, -3.85934242e-03, -3.29683533e-03,
//-2.09227443e-03 - 2.95061462e-04, 1.94011546e-03, 4.35622133e-03,
//6.61376299e-03, 8.32757109e-03, 9.11771253e-03, 8.66909448e-03,
//6.79211876e-03, 3.47543813e-03, -1.07821923e-03, -6.44088973e-03,
//-1.19779649e-02, -1.68978896e-02, -2.03299780e-02, -2.14229946e-02
//- 1.94534396e-02, -1.39300362e-02, -4.68021905e-03, 8.09429657e-03,
//2.38025752e-02, 4.14950657e-02, 5.99388933e-02, 7.77310134e-02,
//9.34370982e-02, 1.05740158e-01, 1.13581088e-01, 1.16273874e-01,
//1.13581088e-01, 1.05740158e-01, 9.34370982e-02, 7.77310134e-02,
//5.99388933e-02, 4.14950657e-02, 2.38025752e-02, 8.09429657e-03
//- 4.68021905e-03, -1.39300362e-02, -1.94534396e-02, -2.14229946e-02,
//-2.03299780e-02, -1.68978896e-02, -1.19779649e-02, -6.44088973e-03,
//-1.07821923e-03, 3.47543813e-03, 6.79211876e-03, 8.66909448e-03,
//9.11771253e-03, 8.32757109e-03, 6.61376299e-03, 4.35622133e-03,
//1.94011546e-03, -2.95061462e-04, -2.09227443e-03, -3.29683533e-03,
//-3.85934242e-03, -3.82395788e-03, -3.30599838e-03, -2.46368958e-03,
//-1.46898551e-03, -4.81689159e-04, 3.70063355e-04, 1.00132456e-03,
//1.37366812e-03, 1.49135940e-03, 1.39225130e-03, 1.13572036e-03,
//7.89962302e-04, 4.20619816e-04, 8.21355058e-05, -1.87459443e-04,
//-3.68268824e-04, -4.57251857e-04, -4.64779102e-04, -4.10013742e-04,
//-3.16073463e-04, -2.05751240e-04 - 9.83121612e-05, -7.59809191e-06,
//5.85860953e-05, 9.80256825e-05, 1.12923355e-04, 1.08367774e-04,
//9.07756300e-05, 6.65585613e-05, 4.11644680e-05, 1.85421958e-05,
//9.96105124e-07, -1.06578868e-05, -1.67482831e-05, -1.83283097e-05,
//-1.67727103e-05, -1.34528799e-05, -9.51992478e-06, -5.79738729e-06,
//-2.76567778e-06, -6.09896576e-07, 6.99215233e-07, 1.31721749e-06,
//1.44915107e-06, 1.29375661e-06, 1.01105268e-06, 7.10288692e-07,
//4.52468561e-07, 2.61011214e-07, 1.35025413e-07, 6.13798167e-08,
//2.35903552e-08, 7.07530209e-09, 1.32727108e-09