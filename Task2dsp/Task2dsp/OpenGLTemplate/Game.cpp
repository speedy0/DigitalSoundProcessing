/* 
OpenGL Template for INM376 / IN3005
City University London, School of Mathematics, Computer Science and Engineering
Source code drawn from a number of sources and examples, including contributions from
 - Ben Humphrey (gametutorials.com), Michal Bubner (mbsoftworks.sk), Christophe Riccio (glm.g-truc.net)
 - Christy Quinn, Sam Kellett and others

 For educational use by Department of Computer Science, City University London UK.

 This template contains a skybox, simple terrain, camera, lighting, shaders, texturing

 Potential ways to modify the code:  Add new geometry types, shaders, change the terrain, load new meshes, change the lighting, 
 different camera controls, different shaders, etc.
 
 Template version 5.0a 29/01/2017
 Dr Greg Slabaugh (gregory.slabaugh.1@city.ac.uk) 

 version 6.0a 29/01/2019
 Dr Eddie Edwards (Philip.Edwards@city.ac.uk)

 Used as a template for INM376 'Computer Graphics' Coursework. 
*/


#include "game.h"


// Setup includes
#include "HighResolutionTimer.h"
#include "GameWindow.h"

// Game includes
#include "Camera.h"
#include "Skybox.h"
#include "Plane.h"
#include "Shaders.h"
#include "FreeTypeFont.h"
#include "Sphere.h"
#include "MatrixStack.h"
#include "OpenAssetImportMesh.h"
#include "Audio.h"
#include "CatmullRom.h"
#include "cube.h"
#include "Rectangle.h"
#include "Road.h"
#include "Pentahedron.h"

// Constructor
Game::Game()
{
	m_pSkybox = NULL;
	m_pCamera = NULL;
	m_pShaderPrograms = NULL;
	m_pPlanarTerrain = NULL;
	m_pFtFont = NULL;
	m_pBarrelMesh = NULL;
	m_pHorseMesh = NULL;
	m_Tree = NULL;
	m_Fighter = NULL;
	m_Car = NULL;
	m_Jeep = NULL;
	m_f360 = NULL;
	m_pSphere = NULL;
	m_pHighResolutionTimer = NULL;
	m_pAudio = NULL;
	m_CatmullRom = NULL;
	m_Cube = NULL;
	m_GP = NULL;
	m_Road = NULL;
	m_Pp = NULL;

	m_dt = 0.0;
	m_t = 0.f;
	m_framesPerSecond = 0;
	m_frameCount = 0;
	m_elapsedTime = 0.0f;
	m_currentDist = 0.f;
	m_camRot = 0.f;
	m_TreeSpacing = 10.f;
	m_HouseSpacing = 13.f;
	m_RoofSpacing = 104.f;
	m_GrassSpacing = 12.f;
	m_rotY = 0.f;
	m_Speed = 1.f;
	m_CurrSpeed = 0.f;
	fighter_position = glm::vec3(0.f);
	car_position = glm::vec3(0.f);
	Turn = glm::vec3(0.f);
	fighter_orientation = glm::mat4(1);
	car_orientation = glm::mat4(1);

	//Sets the audio to play.
	isMusic = true;
	//Sets the lights
	isNight = false;
	isDynamic = true;
	//turns on/off fog
	fog = false;
	//Turns on and off discard.
	isAppearing = false;

	//Initialises Freeview as main camera.
	isFPview = false;
	isTPview = false;
	isTopDownview = false;
	isFreeview = true;

	//Initialises position of the player on the N-axis for TNB.
	isRight = false;
	isLeft = false;
	isMiddle = true;
	
	//Sets won and lost condition to false;
	isFinished = false;
	isDead = false;
}

// Destructor
Game::~Game() 
{ 
	//game objects
	delete m_pCamera;
	delete m_pSkybox;
	delete m_pPlanarTerrain;
	delete m_pFtFont;
	delete m_pBarrelMesh;
	delete m_pHorseMesh;
	delete m_Tree;
	delete m_Fighter;
	delete m_Car;
	delete m_Jeep;
	delete m_f360;
	delete m_pSphere;
	delete m_pAudio;
	delete m_CatmullRom;
	delete m_Cube;
	delete m_GP;
	delete m_Road;
	delete m_Pp;

	if (m_pShaderPrograms != NULL) {
		for (unsigned int i = 0; i < m_pShaderPrograms->size(); i++)
			delete (*m_pShaderPrograms)[i];
	}
	delete m_pShaderPrograms;

	//setup objects
	delete m_pHighResolutionTimer;
}

// Initialisation:  This method only runs once at startup
void Game::Initialise() 
{
	// Set the clear colour and depth
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClearDepth(1.0f);

	/// Create objects
	m_pCamera = new CCamera;
	m_pSkybox = new CSkybox;
	m_pShaderPrograms = new vector <CShaderProgram *>;
	m_pPlanarTerrain = new CPlane;
	m_pFtFont = new CFreeTypeFont;
	m_pBarrelMesh = new COpenAssetImportMesh;
	m_pHorseMesh = new COpenAssetImportMesh;
	m_Tree = new COpenAssetImportMesh;
	m_Fighter = new COpenAssetImportMesh;
	m_Car = new COpenAssetImportMesh;
	m_Jeep = new COpenAssetImportMesh;
	m_f360 = new COpenAssetImportMesh;
	m_pSphere = new CSphere;
	m_pAudio = new CAudio;
	m_CatmullRom = new CCatmullRom;
	m_Cube = new CCube;
	m_GP = new GGrassPatch;
	m_Road = new CRoad;
	m_Pp = new CPentahedron;

	RECT dimensions = m_gameWindow.GetDimensions();

	int width = dimensions.right - dimensions.left;
	int height = dimensions.bottom - dimensions.top;

	// Set the orthographic and perspective projection matrices based on the image size
	m_pCamera->SetOrthographicProjectionMatrix(width, height); 
	m_pCamera->SetPerspectiveProjectionMatrix(45.0f, (float) width / (float) height, 0.5f, 5000.0f);

	// Load shaders
	vector<CShader> shShaders;
	vector<string> sShaderFileNames;
	sShaderFileNames.push_back("mainShader.vert");
	sShaderFileNames.push_back("mainShader.frag");
	sShaderFileNames.push_back("textShader.vert");
	sShaderFileNames.push_back("textShader.frag");
	sShaderFileNames.push_back("HouseSP.vert");
	sShaderFileNames.push_back("HouseSP.frag");
	sShaderFileNames.push_back("Dynamic.vert");
	sShaderFileNames.push_back("Dynamic.frag");
	sShaderFileNames.push_back("Road.vert");
	sShaderFileNames.push_back("Road.frag");

	for (int i = 0; i < (int) sShaderFileNames.size(); i++) {
		string sExt = sShaderFileNames[i].substr((int) sShaderFileNames[i].size()-4, 4);
		int iShaderType;
		if (sExt == "vert") iShaderType = GL_VERTEX_SHADER;
		else if (sExt == "frag") iShaderType = GL_FRAGMENT_SHADER;
		else if (sExt == "geom") iShaderType = GL_GEOMETRY_SHADER;
		else if (sExt == "tcnl") iShaderType = GL_TESS_CONTROL_SHADER;
		else iShaderType = GL_TESS_EVALUATION_SHADER;
		CShader shader;
		shader.LoadShader("resources\\shaders\\"+sShaderFileNames[i], iShaderType);
		shShaders.push_back(shader);
	}

	// Create the main shader program
	CShaderProgram *pMainProgram = new CShaderProgram;
	pMainProgram->CreateProgram();
	pMainProgram->AddShaderToProgram(&shShaders[0]);
	pMainProgram->AddShaderToProgram(&shShaders[1]);
	pMainProgram->LinkProgram();
	m_pShaderPrograms->push_back(pMainProgram);

	// Create a shader program for fonts
	CShaderProgram *pFontProgram = new CShaderProgram;
	pFontProgram->CreateProgram();
	pFontProgram->AddShaderToProgram(&shShaders[2]);
	pFontProgram->AddShaderToProgram(&shShaders[3]);
	pFontProgram->LinkProgram();
	m_pShaderPrograms->push_back(pFontProgram);

	//Create a shader program for spotlights
	CShaderProgram* pHouseProgram = new CShaderProgram;
	pHouseProgram->CreateProgram();
	pHouseProgram->AddShaderToProgram(&shShaders[4]);
	pHouseProgram->AddShaderToProgram(&shShaders[5]);
	pHouseProgram->LinkProgram();
	m_pShaderPrograms->push_back(pHouseProgram);

	//Create a shader program for dynamic
	CShaderProgram* pDynamicProgram = new CShaderProgram;
	pDynamicProgram->CreateProgram();
	pDynamicProgram->AddShaderToProgram(&shShaders[6]);
	pDynamicProgram->AddShaderToProgram(&shShaders[7]);
	pDynamicProgram->LinkProgram();
	m_pShaderPrograms->push_back(pDynamicProgram);

	//Create a shader to light up main road.
	CShaderProgram* pRoadProgram = new CShaderProgram;
	pRoadProgram->CreateProgram();
	pRoadProgram->AddShaderToProgram(&shShaders[8]);
	pRoadProgram->AddShaderToProgram(&shShaders[9]);
	pRoadProgram->LinkProgram();
	m_pShaderPrograms->push_back(pRoadProgram);

	// You can follow this pattern to load additional shaders

	// Creates the skybox - Skybox sourced from: https://opengameart.org/content/space-skyboxes-0
	m_pSkybox->Create(2500.0f);
	
	// Creates the terrain with stone-coal Texture.
	m_pPlanarTerrain->Create("resources\\textures\\", "stone-coal.jpg", 2000.0f, 2000.0f, 50.0f); // Texture sourced from: https://previews.123rf.com/images/plrang/plrang1611/plrang161100006/68967407-stone-meteorite-or-coal.jpg

	m_pFtFont->LoadSystemFont("arial.ttf", 32);
	m_pFtFont->SetShaderProgram(pFontProgram);

	// Load some meshes in OBJ format
	m_pBarrelMesh->Load("resources\\models\\Barrel\\Barrel02.obj");  // Downloaded from http://www.psionicgames.com/?page_id=24 on 24 Jan 2013
	m_pHorseMesh->Load("resources\\models\\Horse\\Horse2.obj");  // Downloaded from http://opengameart.org/content/horse-lowpoly on 24 Jan 2013
	m_Tree->Load("resources\\models\\Terrain\\tree.obj"); //Tree obtained from Template of Advanced Games Technology module for Postgraduates at City, University of London.
	m_Fighter->Load("resources\\models\\Fighter\\fighter1.3ds"); //Fighter model obtained from: https://www.dropbox.com/sh/xd8i6p8eltuqrlk/AAA1DLBW-YaQlkYng7IKv2xSa?dl=0
	m_Car->Load("resources\\models\\audi\\audi.3ds"); //Car obtained from: http://www.psionicdesign.com
	m_Jeep->Load("resources\\models\\jeep\\jeep1.3ds"); //Jeep obtained from: http://www.psionicdesign.com
	m_f360->Load("resources\\models\\f360\\f360.3ds"); //3rd Car obtained from: http://www.psionicdesign.com

	// Create a sphere
	m_pSphere->Create("resources\\textures\\", "earth.jpg", 25, 25);  // Texture downloaded from https://b3d.interplanety.org/wp-content/upload_content/2016/08/01-3.jpg
	glEnable(GL_CULL_FACE);

	//Create a cube that will be used as a base of a house in this Coursework.
	m_Cube->Create("resources\\textures\\EmptyHouse.jpg", "resources\\textures\\EmptyHouse1.jpg", 2); //Texture obtained from: https://image.freepik.com/free-photo/background-brick-wall-black-white_39730-1079.jpg

	//Creates a rectangle that will be used as grass patches in this Coursework.
	m_GP->Create("resources\\textures\\wood.jpg", "resources\\textures\\grassfloor01.jpg", 10, 4, 4); //Texture obtained from: https://images.pexels.com/photos/326311/pexels-photo-326311.jpeg?auto=compress&cs=tinysrgb&dpr=1&w=500 & from lab at City, University of London. 

	//Creates pentahedron that will be used as roofs.
	m_Pp->Create("resources\\textures\\roof.jpg", 20.f); //Texture created by me.

	// Initialise audio and play background music
	m_pAudio->Initialise();
	m_pAudio->LoadEventSound("resources\\Audio\\Boing.wav");				// Royalty free sound from freesound.org
	m_pAudio->Load_fighter_sound("resources\\Audio\\spaceship.mp3");		// Sound sourced from: https://freesound.org/people/Bird_man/sounds/275149/
	m_pAudio->Load_car_sound("resources\\Audio\\car-engine.mp3");			// Sound sourced from: https://freesound.org/people/modman34/sounds/512558/
	m_pAudio->LoadMusicStream("resources\\Audio\\Track1-SilentDream.mp3");	//Song designed by The Mirages (Aleksander Napieralski).
	m_pAudio->Play_fighter_sound();
	m_pAudio->Play_car_sound();

	//CatmullRom spline rendering
	m_CatmullRom->CreateCentreline();
	m_CatmullRom->CreateOffsetCurves();
	m_CatmullRom->CreateBarriers();
	m_CatmullRom->CreateTrack();
	m_CatmullRom->CreateBarrierTrack();
	m_CatmullRom->TRender("resources\\textures\\Road_wet.jpg", "resources\\textures\\wood.jpg"); //sourced from: https://cdn.cgbookcase.cloud/file/cgbookcase/textures/downloads/Three_lane_road_wet_01/Three_lane_road_wet_01_4K_Mask.png

	//Side Road Rendering
	m_Road->CreateCL();
	m_Road->CreateOffsetCurves();
	m_Road->CreateTrack();
	m_Road->TRender("resources\\textures\\", "Road_wet.jpg");

	//Ambient Colors:
	ambients.push_back(glm::vec3(1.f, 0.f, 0.f));
	//ambients.push_back(glm::vec3(0.f, 1.f, 0.f));
	ambients.push_back(glm::vec3(0.f, 0.f, 1.f));
}

// Render method runs repeatedly in a loop
void Game::Render() 
{
	
	// Clear the buffers and enable depth testing (z-buffering)
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	// Set up a matrix stack
	glutil::MatrixStack modelViewMatrixStack;
	modelViewMatrixStack.SetIdentity();

	// Use the main shader program 
	CShaderProgram *pMainProgram = (*m_pShaderPrograms)[0];
	pMainProgram->UseProgram();
	pMainProgram->SetUniform("bUseTexture", true);
	pMainProgram->SetUniform("sampler0", 0);
	// Note: cubemap and non-cubemap textures should not be mixed in the same texture unit.  Setting unit 10 to be a cubemap texture.
	int cubeMapTextureUnit = 10; 
	pMainProgram->SetUniform("CubeMapTex", cubeMapTextureUnit);
	

	// Set the projection matrix
	pMainProgram->SetUniform("matrices.projMatrix", m_pCamera->GetPerspectiveProjectionMatrix());

	// Call LookAt to create the view matrix and put this on the modelViewMatrix stack. 
	// Store the view matrix and the normal matrix associated with the view matrix for later (they're useful for lighting -- since lighting is done in eye coordinates)
	modelViewMatrixStack.LookAt(m_pCamera->GetPosition(), m_pCamera->GetView(), m_pCamera->GetUpVector());
	glm::mat4 viewMatrix = modelViewMatrixStack.Top();
	glm::mat3 viewNormalMatrix = m_pCamera->ComputeNormalMatrix(viewMatrix);

	
	// Set light and materials in main shader program
	glm::vec4 lightPosition1 = glm::vec4(-100, 1000, -100, 1); // Position of light source *in world coordinates*
	pMainProgram->SetUniform("light1.position", viewMatrix*lightPosition1); // Position of light source *in eye coordinates*
	if (isNight)
		pMainProgram->SetUniform("light1.La", glm::vec3(0.f, 0.f, 0.1f));		// Ambient colour of light
	else
		pMainProgram->SetUniform("light1.La", glm::vec3(1.f));		// Ambient colour of light
	pMainProgram->SetUniform("light1.Ld", glm::vec3(1.0f));		// Diffuse colour of light
	pMainProgram->SetUniform("light1.Ls", glm::vec3(1.0f));		// Specular colour of light
	if (isNight)
		pMainProgram->SetUniform("material1.Ma", glm::vec3(0.3f));	// Ambient material reflectance
	else
		pMainProgram->SetUniform("material1.Ma", glm::vec3(1.0f));	// Ambient material reflectance
	pMainProgram->SetUniform("material1.Md", glm::vec3(0.0f));	// Diffuse material reflectance
	pMainProgram->SetUniform("material1.Ms", glm::vec3(0.0f));	// Specular material reflectance
	pMainProgram->SetUniform("material1.shininess", 15.0f);		// Shininess material property
	pMainProgram->SetUniform("fog", fog);
		
	// Render the skybox and terrain with full ambient reflectance 
	modelViewMatrixStack.Push();
	pMainProgram->SetUniform("renderSkybox", true);
	// Translate the modelview matrix to the camera eye point so skybox stays centred around camera
	glm::vec3 vEye = m_pCamera->GetPosition();
	modelViewMatrixStack.Translate(vEye);
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	m_pSkybox->Render(cubeMapTextureUnit);
	pMainProgram->SetUniform("renderSkybox", false);
	modelViewMatrixStack.Pop();

	if (!isNight) {

		// Render the planar terrain
		modelViewMatrixStack.Push();
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pPlanarTerrain->Render();
		modelViewMatrixStack.Pop();
	}
	
	// Turn on diffuse + specular materials
	pMainProgram->SetUniform("material1.Ma", glm::vec3(0.5f));	// Ambient material reflectance
	pMainProgram->SetUniform("material1.Md", glm::vec3(0.5f));	// Diffuse material reflectance
	pMainProgram->SetUniform("material1.Ms", glm::vec3(1.0f));	// Specular material reflectance

	//Renders the barrel 
	modelViewMatrixStack.Push();
	modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(glm::vec3(-381.0f, 1.0f, -173.0f));
		modelViewMatrixStack.Scale(5.0f);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pBarrelMesh->Render();
	modelViewMatrixStack.Pop();
	modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(glm::vec3(-605.0f, 1.0f, 43.0f));
		modelViewMatrixStack.Scale(5.0f);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pBarrelMesh->Render();
	modelViewMatrixStack.Pop();
	modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(glm::vec3(-607.0f, 1.0f, -272.0f));
		modelViewMatrixStack.Scale(5.0f);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pBarrelMesh->Render();
	modelViewMatrixStack.Pop();
	modelViewMatrixStack.Pop();

	//Renders a fighter.
	if (!isFPview) {
		modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(fighter_position); 
		modelViewMatrixStack *= fighter_orientation;
		modelViewMatrixStack.Rotate(glm::vec3(0.f, 0.f, 1.f), glm::radians(-90.f));
		modelViewMatrixStack.Rotate(glm::vec3(0.f, 1.f, 0.f), glm::radians(-90.f));
		modelViewMatrixStack.Rotate(glm::vec3(0.f, 0.f, 1.f), glm::radians(-90.f));
		modelViewMatrixStack.Scale(glm::vec3(0.35f));
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_Fighter->Render();
		modelViewMatrixStack.Pop();
	}

	//Renders audi (Car)
	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(car_position);
	modelViewMatrixStack *= car_orientation;
	modelViewMatrixStack.Rotate(glm::vec3(0, 0, 1), glm::radians(90.f));
	modelViewMatrixStack.Rotate(glm::vec3(0, 1, 0), glm::radians(90.f));
	modelViewMatrixStack.Rotate(glm::vec3(0, 0, 1), glm::radians(-180.f));
	modelViewMatrixStack.Scale(glm::vec3(5.f));
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	m_Car->Render();
	modelViewMatrixStack.Pop();

	//Render jeeps (2nd Car)
	modelViewMatrixStack.Push();
	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(glm::vec3(-368.f, 1.f, 134.f));
	modelViewMatrixStack.Rotate(glm::vec3(0, 0, 1), glm::radians(90.f));
	modelViewMatrixStack.Rotate(glm::vec3(0, 1, 0), glm::radians(90.f));
	modelViewMatrixStack.Scale(glm::vec3(1.5f));
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	m_Jeep->Render();
	modelViewMatrixStack.Pop();
	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(glm::vec3(-368.f, 1.f, -383.f));
	modelViewMatrixStack.Rotate(glm::vec3(0, 0, 1), glm::radians(90.f));
	modelViewMatrixStack.Rotate(glm::vec3(0, 1, 0), glm::radians(90.f));
	modelViewMatrixStack.Scale(glm::vec3(1.5f));
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	m_Jeep->Render();
	modelViewMatrixStack.Pop();
	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(glm::vec3(-610.f, 1.f, -75.f));
	modelViewMatrixStack.Rotate(glm::vec3(0, 0, 1), glm::radians(90.f));
	modelViewMatrixStack.Rotate(glm::vec3(0, 1, 0), glm::radians(90.f));
	modelViewMatrixStack.Rotate(glm::vec3(0, 0, 1), glm::radians(180.f));
	modelViewMatrixStack.Scale(glm::vec3(1.5f));
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	m_Jeep->Render();
	modelViewMatrixStack.Pop();
	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(glm::vec3(-610.f, 1.f, 232.f));
	modelViewMatrixStack.Rotate(glm::vec3(0, 0, 1), glm::radians(90.f));
	modelViewMatrixStack.Rotate(glm::vec3(0, 1, 0), glm::radians(90.f));
	modelViewMatrixStack.Rotate(glm::vec3(0, 0, 1), glm::radians(180.f));
	modelViewMatrixStack.Scale(glm::vec3(1.5f));
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	m_Jeep->Render();
	modelViewMatrixStack.Pop();
	modelViewMatrixStack.Pop();

	if (!isNight) {
		//F360 Car spawned near the houses:
		modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(glm::vec3(-610.f, 1.f, 129.f));
		modelViewMatrixStack.Rotate(glm::vec3(0, 0, 1), glm::radians(90.f));
		modelViewMatrixStack.Rotate(glm::vec3(0, 1, 0), glm::radians(90.f));
		modelViewMatrixStack.Rotate(glm::vec3(0, 0, 1), glm::radians(180.f));
		modelViewMatrixStack.Scale(glm::vec3(5.f));
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_f360->Render();
		modelViewMatrixStack.Pop();
	}
	
	//Starts Matrix for Houses rendering without roofs 
	modelViewMatrixStack.Push();
	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(glm::vec3(-609.f, 19.f, -454.f));
	modelViewMatrixStack.Scale(8.f);
	modelViewMatrixStack.Push();
	modelViewMatrixStack.Rotate(glm::vec3(0, 1, 0), glm::radians(90.f));
	for (unsigned i = 0; i < 9; i++) {
		modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(glm::vec3(1.f * ((i - 1) * -m_HouseSpacing), 0.f, 0.f));
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_Cube->Render();
		modelViewMatrixStack.Pop();
	}
	modelViewMatrixStack.Pop();
	modelViewMatrixStack.Pop();

	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(glm::vec3(-369.f, 19.f, 275.f));
	modelViewMatrixStack.Scale(8.f);
	modelViewMatrixStack.Push();
	modelViewMatrixStack.Rotate(glm::vec3(0, 1, 0), glm::radians(-90.f));
	for (unsigned i = 0; i < 9; i++) {
		modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(glm::vec3(1.f * ((i - 1) * -m_HouseSpacing), 0.f, 0.f));
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_Cube->Render();
		modelViewMatrixStack.Pop();
	}
	modelViewMatrixStack.Pop();
	modelViewMatrixStack.Pop();
	modelViewMatrixStack.Pop();

	//Render roofs for the houses
	//Starts Matrix for Houses rendering without roofs 
	modelViewMatrixStack.Push();
	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(glm::vec3(-609.f, 55.f, -454.f));
	modelViewMatrixStack.Push();
	for (unsigned i = 0; i < 9; i++) {
		modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(glm::vec3(0.f, 0.f, 1.f * ((i - 1) * m_RoofSpacing)));
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_Pp->Render();
		modelViewMatrixStack.Pop();
	}
	modelViewMatrixStack.Pop();
	modelViewMatrixStack.Pop();

	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(glm::vec3(-369.f, 55.f, 275.f));
	modelViewMatrixStack.Push();
	for (unsigned i = 0; i < 9; i++) {
		modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(glm::vec3(0.f, 0.f, 1.f * ((i - 1) * -m_RoofSpacing)));
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_Pp->Render();
		modelViewMatrixStack.Pop();
	}
	modelViewMatrixStack.Pop();
	modelViewMatrixStack.Pop();
	modelViewMatrixStack.Pop();

	//Creates Grass Patches near the trees and the road.
	modelViewMatrixStack.Push();
	modelViewMatrixStack.Push();
	for (unsigned i = 0; i < 3; i++) {
		modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(-495.f, 4.f, -49.f + (i * m_GrassSpacing));
		modelViewMatrixStack.Rotate(glm::vec3(1, 0, 0), glm::radians(180.f));
		modelViewMatrixStack.Rotate(glm::vec3(0, 1, 0), glm::radians(90.f));
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_GP->Render();
		modelViewMatrixStack.Pop();
	}
	for (unsigned i = 0; i < 3; i++) {
		modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(-495.f, 4.f, -49.f - (i * m_GrassSpacing));
		modelViewMatrixStack.Rotate(glm::vec3(1, 0, 0), glm::radians(180.f));
		modelViewMatrixStack.Rotate(glm::vec3(0, 1, 0), glm::radians(90.f));
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_GP->Render();
		modelViewMatrixStack.Pop();
	}
	modelViewMatrixStack.Pop();
	modelViewMatrixStack.Push();
	for (unsigned i = 0; i < 3; i++) {
		modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(-495.f, 4.f, 44.f + (i * m_GrassSpacing));
		modelViewMatrixStack.Rotate(glm::vec3(1, 0, 0), glm::radians(180.f));
		modelViewMatrixStack.Rotate(glm::vec3(0, 1, 0), glm::radians(90.f));
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_GP->Render();
		modelViewMatrixStack.Pop();
	}
	for (unsigned i = 0; i < 3; i++) {
		modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(-495.f, 4.f, 44.f - (i * m_GrassSpacing));
		modelViewMatrixStack.Rotate(glm::vec3(1, 0, 0), glm::radians(180.f));
		modelViewMatrixStack.Rotate(glm::vec3(0, 1, 0), glm::radians(90.f));
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_GP->Render();
		modelViewMatrixStack.Pop();
	}
	modelViewMatrixStack.Pop();
	modelViewMatrixStack.Push();
	for (unsigned i = 0; i < 3; i++) {
		modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(-495.f, 4.f, 150.f + (i * m_GrassSpacing));
		modelViewMatrixStack.Rotate(glm::vec3(1, 0, 0), glm::radians(180.f));
		modelViewMatrixStack.Rotate(glm::vec3(0, 1, 0), glm::radians(90.f));
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_GP->Render();
		modelViewMatrixStack.Pop();
	}
	for (unsigned i = 0; i < 3; i++) {
		modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(-495.f, 4.f, 150.f - (i * m_GrassSpacing));
		modelViewMatrixStack.Rotate(glm::vec3(1, 0, 0), glm::radians(180.f));
		modelViewMatrixStack.Rotate(glm::vec3(0, 1, 0), glm::radians(90.f));
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_GP->Render();
		modelViewMatrixStack.Pop();
	}
	modelViewMatrixStack.Pop();
	modelViewMatrixStack.Push();
	for (unsigned i = 0; i < 3; i++) {
		modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(-495.f, 4.f, 253.f + (i * m_GrassSpacing));
		modelViewMatrixStack.Rotate(glm::vec3(1, 0, 0), glm::radians(180.f));
		modelViewMatrixStack.Rotate(glm::vec3(0, 1, 0), glm::radians(90.f));
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_GP->Render();
		modelViewMatrixStack.Pop();
	}
	for (unsigned i = 0; i < 3; i++) {
		modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(-495.f, 4.f, 253.f - (i * m_GrassSpacing));
		modelViewMatrixStack.Rotate(glm::vec3(1, 0, 0), glm::radians(180.f));
		modelViewMatrixStack.Rotate(glm::vec3(0, 1, 0), glm::radians(90.f));
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_GP->Render();
		modelViewMatrixStack.Pop();
	}
	modelViewMatrixStack.Pop();
	modelViewMatrixStack.Push();
	for (unsigned i = 0; i < 3; i++) {
		modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(-495.f, 4.f, -150.f + (i * m_GrassSpacing));
		modelViewMatrixStack.Rotate(glm::vec3(1, 0, 0), glm::radians(180.f));
		modelViewMatrixStack.Rotate(glm::vec3(0, 1, 0), glm::radians(90.f));
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_GP->Render();
		modelViewMatrixStack.Pop();
	}
	for (unsigned i = 0; i < 3; i++) {
		modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(-495.f, 4.f, -150.f - (i * m_GrassSpacing));
		modelViewMatrixStack.Rotate(glm::vec3(1, 0, 0), glm::radians(180.f));
		modelViewMatrixStack.Rotate(glm::vec3(0, 1, 0), glm::radians(90.f));
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_GP->Render();
		modelViewMatrixStack.Pop();
	}
	modelViewMatrixStack.Pop();
	modelViewMatrixStack.Push();
	for (unsigned i = 0; i < 3; i++) {
		modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(-495.f, 4.f, -248.f + (i * m_GrassSpacing));
		modelViewMatrixStack.Rotate(glm::vec3(1, 0, 0), glm::radians(180.f));
		modelViewMatrixStack.Rotate(glm::vec3(0, 1, 0), glm::radians(90.f));
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_GP->Render();
		modelViewMatrixStack.Pop();
	}
	for (unsigned i = 0; i < 3; i++) {
		modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(-495.f, 4.f, -248.f - (i * m_GrassSpacing));
		modelViewMatrixStack.Rotate(glm::vec3(1, 0, 0), glm::radians(180.f));
		modelViewMatrixStack.Rotate(glm::vec3(0, 1, 0), glm::radians(90.f));
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_GP->Render();
		modelViewMatrixStack.Pop();
	}
	modelViewMatrixStack.Pop();
	modelViewMatrixStack.Push();
	for (unsigned i = 0; i < 3; i++) {
		modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(-495.f, 4.f, -348.f + (i * m_GrassSpacing));
		modelViewMatrixStack.Rotate(glm::vec3(1, 0, 0), glm::radians(180.f));
		modelViewMatrixStack.Rotate(glm::vec3(0, 1, 0), glm::radians(90.f));
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_GP->Render();
		modelViewMatrixStack.Pop();
	}
	for (unsigned i = 0; i < 3; i++) {
		modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(-495.f, 4.f, -348.f - (i * m_GrassSpacing));
		modelViewMatrixStack.Rotate(glm::vec3(1, 0, 0), glm::radians(180.f));
		modelViewMatrixStack.Rotate(glm::vec3(0, 1, 0), glm::radians(90.f));
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_GP->Render();
		modelViewMatrixStack.Pop();
	}
	modelViewMatrixStack.Pop();
	modelViewMatrixStack.Push();
	for (unsigned i = 0; i < 3; i++) {
		modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(-495.f, 4.f, -446.f + (i * m_GrassSpacing));
		modelViewMatrixStack.Rotate(glm::vec3(1, 0, 0), glm::radians(180.f));
		modelViewMatrixStack.Rotate(glm::vec3(0, 1, 0), glm::radians(90.f));
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_GP->Render();
		modelViewMatrixStack.Pop();
	}
	for (unsigned i = 0; i < 3; i++) {
		modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(-495.f, 4.f, -446.f - (i * m_GrassSpacing));
		modelViewMatrixStack.Rotate(glm::vec3(1, 0, 0), glm::radians(180.f));
		modelViewMatrixStack.Rotate(glm::vec3(0, 1, 0), glm::radians(90.f));
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_GP->Render();
		modelViewMatrixStack.Pop();
	}
	modelViewMatrixStack.Pop();
	modelViewMatrixStack.Pop();
	
	//Renders the environment: 

	//Renders trees in Matrix Rendering.
	modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(glm::vec3(5.f, 0.f, 0.f));
		modelViewMatrixStack.Rotate(glm::vec3(1.f, 0.f, 0.f), glm::radians(90.f));
		modelViewMatrixStack.Scale(glm::vec3(10.f));
		//Tree Rows:
		modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(glm::vec3(0.f, 0.f, 0.f));
		modelViewMatrixStack.Translate(glm::vec3(-100, 0, 0));
		for (unsigned i = 0; i < 20; i++) {
			modelViewMatrixStack.Push();
			modelViewMatrixStack.Translate(glm::vec3(1.f*((i-1)*m_TreeSpacing), 0.f, 0.f));
			pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
			pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
			m_Tree->Render();
			modelViewMatrixStack.Pop();
		}
		for (unsigned i = 0; i < 10; i++) {
			modelViewMatrixStack.Push();
				modelViewMatrixStack.Translate(glm::vec3(0.f, 10.f * i, 0));
				for (unsigned i = 0; i < 20; i++) {
					modelViewMatrixStack.Push();
					modelViewMatrixStack.Translate(glm::vec3(1.f * ((i - 1) * m_TreeSpacing), 0.f, 0.f));
					pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
					pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
					m_Tree->Render();
					modelViewMatrixStack.Pop();
				}
			modelViewMatrixStack.Pop();
		}
		for (unsigned i = 0; i < 10; i++) {
			modelViewMatrixStack.Push();
			modelViewMatrixStack.Translate(glm::vec3(0.f, -10.f * i, 0));
			for (unsigned i = 0; i < 20; i++) {
				modelViewMatrixStack.Push();
				modelViewMatrixStack.Translate(glm::vec3(1.f * ((i - 1) * m_TreeSpacing), 0.f, 0.f));
				pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
				pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
				m_Tree->Render();
				modelViewMatrixStack.Pop();
			}
			modelViewMatrixStack.Pop();
		}
		modelViewMatrixStack.Pop();
	//Closes Total Matrix.
	modelViewMatrixStack.Pop();

	// Render the sphere
	modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(glm::vec3(247.0f, 108.f, 80.0f));
		modelViewMatrixStack.Rotate(glm::vec3(1, 0, 0), glm::radians(90.f));
		modelViewMatrixStack.Rotate(glm::vec3(0, 0, -1), m_rotY);
		modelViewMatrixStack.Scale(100.0f);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		// To turn off texture mapping and use the sphere colour only (currently white material), uncomment the next line
		pMainProgram->SetUniform("bUseTexture", true);
		m_pSphere->Render();
	modelViewMatrixStack.Pop();

	modelViewMatrixStack.Push();
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		pMainProgram->SetUniform("bUseTexture", true);
	modelViewMatrixStack.Pop(); 
	
	glDisable(GL_CULL_FACE);
	if (!isNight) {
		//CatmullRom Spline
		m_CatmullRom->RenderCentreline();
		m_CatmullRom->RenderOffsetCurves();
		m_CatmullRom->RenderBarriers();
		m_CatmullRom->RenderTrack();
		m_CatmullRom->RenderBarrierTrack();

		//Road
		m_Road->RenderCL();
		m_Road->RenderOffsetCurves();
		m_Road->RenderTrack();
	}

	//Switches to HouseSpotlight program
	CShaderProgram *pHouseProgram = (*m_pShaderPrograms)[2];
	pHouseProgram->UseProgram();
	pHouseProgram->SetUniform("bUseTexture", true);
	pHouseProgram->SetUniform("sampler0", 0);

	glm::vec4 spotlight1(-610, 7500, 129, 1);
	glm::vec4 spotlight2(-496, 7500, 173, 1);
	glm::vec4 spotlight3(-496, 7500, -88, 1);
	glm::vec4 spotlight4(-496, 7500, -405, 1);
	glm::vec4 spotlight5(247.0f, 20000.f, 80.0f, 1);
	//Spotlight 1
	pHouseProgram->SetUniform("spotlight1.Lposition", viewMatrix* spotlight1);
	pHouseProgram->SetUniform("spotlight1.Lambient", glm::vec3(0.f));
	pHouseProgram->SetUniform("spotlight1.Ldiffuse", glm::vec3(1.f, 1.f, 0.f));
	pHouseProgram->SetUniform("spotlight1.Lspecular", glm::vec3(1.f, 1.f, 0.f));
	pHouseProgram->SetUniform("spotlight1.Ldirection", glm::normalize(viewNormalMatrix * glm::vec3(0, -1, 0)));
	pHouseProgram->SetUniform("spotlight1.Exp", 80.f);
	pHouseProgram->SetUniform("spotlight1.Cutoff", 100.f);
	//Spotlight 2
	pHouseProgram->SetUniform("spotlight2.Lposition", viewMatrix * spotlight2);
	pHouseProgram->SetUniform("spotlight2.Lambient", glm::vec3(0.f));
	pHouseProgram->SetUniform("spotlight2.Ldiffuse", glm::vec3(1.f, 0.f, 1.f));
	pHouseProgram->SetUniform("spotlight2.Lspecular", glm::vec3(1.f, 0.f, 1.f));
	pHouseProgram->SetUniform("spotlight2.Ldirection", glm::normalize(viewNormalMatrix * glm::vec3(0, -1, 0)));
	pHouseProgram->SetUniform("spotlight2.Exp", 80.f);
	pHouseProgram->SetUniform("spotlight2.Cutoff", 100.f);
	//Spotlight 3
	pHouseProgram->SetUniform("spotlight3.Lposition", viewMatrix * spotlight3);
	pHouseProgram->SetUniform("spotlight3.Lambient", glm::vec3(0.f));
	pHouseProgram->SetUniform("spotlight3.Ldiffuse", glm::vec3(0.5f, 0.8f, 0.8f));
	pHouseProgram->SetUniform("spotlight3.Lspecular", glm::vec3(0.5f, 0.8f, 0.8f));
	pHouseProgram->SetUniform("spotlight3.Ldirection", glm::normalize(viewNormalMatrix * glm::vec3(0, -1, 0)));
	pHouseProgram->SetUniform("spotlight3.Exp", 80.f);
	pHouseProgram->SetUniform("spotlight3.Cutoff", 100.f);
	//Spotlight 4
	pHouseProgram->SetUniform("spotlight4.Lposition", viewMatrix* spotlight4);
	pHouseProgram->SetUniform("spotlight4.Lambient", glm::vec3(0.f));
	pHouseProgram->SetUniform("spotlight4.Ldiffuse", glm::vec3(0, 1.f, 0));
	pHouseProgram->SetUniform("spotlight4.Lspecular", glm::vec3(0, 1.f, 0));
	pHouseProgram->SetUniform("spotlight4.Ldirection", glm::normalize(viewNormalMatrix* glm::vec3(0, -1, 0)));
	pHouseProgram->SetUniform("spotlight4.Exp", 80.f);
	pHouseProgram->SetUniform("spotlight4.Cutoff", 100.f);
	//Spotlight 5
	pHouseProgram->SetUniform("spotlight5.Lposition", viewMatrix* spotlight5);
	pHouseProgram->SetUniform("spotlight5.Lambient", glm::vec3(0.f));
	pHouseProgram->SetUniform("spotlight5.Ldiffuse", glm::vec3(0.f, 1.f, 1.f));
	pHouseProgram->SetUniform("spotlight5.Lspecular", glm::vec3(0.f, 1.f, 1.0f));
	pHouseProgram->SetUniform("spotlight5.Ldirection", glm::normalize(viewNormalMatrix* glm::vec3(0, -1, 0)));
	pHouseProgram->SetUniform("spotlight5.Exp", 80.f);
	pHouseProgram->SetUniform("spotlight5.Cutoff", 100.f);
	pHouseProgram->SetUniform("material2.Mshininess", 1.0f);
	pHouseProgram->SetUniform("material2.Mambient", glm::vec3(0.4f));
	pHouseProgram->SetUniform("material2.Mdiffuse", glm::vec3(1.0f, 1.0f, 0.f));
	pHouseProgram->SetUniform("material2.Mspecular", glm::vec3(1.0f, 1.0f, 0.f));
	pHouseProgram->SetUniform("fog", fog);

	if (isNight) {
		//F360 Car spawned near the houses:
		modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(glm::vec3(-610.f, 1.f, 129.f));
		modelViewMatrixStack.Rotate(glm::vec3(0, 0, 1), glm::radians(90.f));
		modelViewMatrixStack.Rotate(glm::vec3(0, 1, 0), glm::radians(90.f));
		modelViewMatrixStack.Rotate(glm::vec3(0, 0, 1), glm::radians(180.f));
		modelViewMatrixStack.Scale(glm::vec3(5.f));
		pHouseProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pHouseProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		pHouseProgram->SetUniform("matrices.projMatrix", m_pCamera->GetPerspectiveProjectionMatrix());
		m_f360->Render();
		modelViewMatrixStack.Pop();

			// Render the planar terrain
			modelViewMatrixStack.Push();
			pHouseProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
			pHouseProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
			pHouseProgram->SetUniform("matrices.projMatrix", m_pCamera->GetPerspectiveProjectionMatrix());
			m_pPlanarTerrain->Render();
			modelViewMatrixStack.Pop();

			//Road
			m_Road->RenderCL();
			m_Road->RenderOffsetCurves();
			m_Road->RenderTrack();
	}

	//Switches to HouseSpotlight program
	CShaderProgram* pRoadProgram = (*m_pShaderPrograms)[2];
	pRoadProgram->UseProgram();
	pRoadProgram->SetUniform("bUseTexture", true);
	pRoadProgram->SetUniform("sampler0", 0);

	glm::vec4 spotlight7(0.0f, 50000.f, 0.0f, 1);
	//Spotlight 7
	pRoadProgram->SetUniform("spotlight1.Lposition", viewMatrix* spotlight7);
	pRoadProgram->SetUniform("spotlight1.Lambient", glm::vec3(0.f));
	pRoadProgram->SetUniform("spotlight1.Ldiffuse", glm::vec3(1.f, 0.f, 0.f));
	pRoadProgram->SetUniform("spotlight1.Lspecular", glm::vec3(1.f, 0.f, 0.f));
	pRoadProgram->SetUniform("spotlight1.Ldirection", glm::normalize(viewNormalMatrix* glm::vec3(0, -1, 0)));
	pRoadProgram->SetUniform("spotlight1.Exp", 80.f);
	pRoadProgram->SetUniform("spotlight1.Cutoff", 100.f);
	pRoadProgram->SetUniform("material2.Mshininess", 1.0f);
	pRoadProgram->SetUniform("material2.Mambient", glm::vec3(0.4f));
	pRoadProgram->SetUniform("material2.Mdiffuse", glm::vec3(1.0f, 1.0f, 0.f));
	pRoadProgram->SetUniform("material2.Mspecular", glm::vec3(1.0f, 1.0f, 0.f));
	pRoadProgram->SetUniform("fog", fog);

	if (isNight) {
		//CatmullRom Spline
		m_CatmullRom->RenderCentreline();
		m_CatmullRom->RenderOffsetCurves();
		m_CatmullRom->RenderBarriers();
		m_CatmullRom->RenderTrack();
		m_CatmullRom->RenderBarrierTrack();
	}

	//Switches to Dynamic Light program
	CShaderProgram* pDynamicProgram = (*m_pShaderPrograms)[3];
	pDynamicProgram->UseProgram();
	pDynamicProgram->SetUniform("bUseTexture", true);
	pDynamicProgram->SetUniform("sampler0", 0);

	//Spotlight6
	glm::vec4 spotlight6(687, 2000, 778, 1);
	pDynamicProgram->SetUniform("spotlight6.Lposition", viewMatrix* spotlight6);
	pDynamicProgram->SetUniform("spotlight6.Lambient", glm::vec3(0.5f));
	pDynamicProgram->SetUniform("spotlight6.Ldiffuse", glm::vec3(0.5f));
	pDynamicProgram->SetUniform("spotlight6.Lspecular", glm::vec3(0.5f));
	pDynamicProgram->SetUniform("spotlight6.Ldirection", glm::normalize(viewNormalMatrix* glm::vec3(0, -1, 0)));
	pDynamicProgram->SetUniform("spotlight6.Exp", 80.f);
	pDynamicProgram->SetUniform("spotlight6.Cutoff", 100.f);
	pDynamicProgram->SetUniform("material3.Mshininess", 1.0f);
	pDynamicProgram->SetUniform("material3.Mambient", AmbientLightsColors); //glm::vec3(0.4f)
	pDynamicProgram->SetUniform("material3.Mdiffuse", glm::vec3(1.0f, 1.0f, 0.f)); //glm::vec3(1.0f, 1.0f, 0.f)
	pDynamicProgram->SetUniform("material3.Mspecular", glm::vec3(1.0f, 1.0f, 0.f)); //glm::vec3(1.0f, 1.0f, 0.f)
	pDynamicProgram->SetUniform("t", 0.1f * m_t);
	pDynamicProgram->SetUniform("Dynamic", isDynamic);
	pDynamicProgram->SetUniform("fog", fog);
	pDynamicProgram->SetUniform("discarding", isAppearing);

	// Render the horse 
	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(glm::vec3(-99.0f, 1.0f, 446.0f)); //694, 1, 782
	modelViewMatrixStack.Rotate(glm::vec3(0.0f, 1.0f, 0.0f), 180.0f);
	modelViewMatrixStack.Scale(20.f);
	//first horse (mother)
	modelViewMatrixStack.Push();
	pDynamicProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pDynamicProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	pDynamicProgram->SetUniform("matrices.projMatrix", m_pCamera->GetPerspectiveProjectionMatrix());
	m_pHorseMesh->Render();
	modelViewMatrixStack.Pop();
	//second horse (kids)
	modelViewMatrixStack.Push();
	modelViewMatrixStack.Scale(0.5f);
	//left kid
	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(glm::vec3(4.5f, 0.f, -4.f));
	pDynamicProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pDynamicProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	pDynamicProgram->SetUniform("matrices.projMatrix", m_pCamera->GetPerspectiveProjectionMatrix());
	m_pHorseMesh->Render();
	modelViewMatrixStack.Pop();
	//right kid
	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(glm::vec3(4.5f, 0.f, 4.f));
	pDynamicProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pDynamicProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	pDynamicProgram->SetUniform("matrices.projMatrix", m_pCamera->GetPerspectiveProjectionMatrix());
	m_pHorseMesh->Render();
	modelViewMatrixStack.Pop();
	modelViewMatrixStack.Pop();
	modelViewMatrixStack.Pop();

	// Draw the 2D graphics after the 3D graphics
	DisplayFrameRate();

	// Swap buffers to show the rendered image
	SwapBuffers(m_gameWindow.Hdc());	
}

// Update method runs repeatedly with the Render method
void Game::Update() 
{
	if (!isDynamic)
		AmbientLightsColors = glm::vec3(1.f);
	else
		AmbientLightsColors = ambients[rand() % ambients.size()];

	m_t += (float)(0.1f * m_dt);
	//Calculates required information for CatmullRom Splines.
	glm::vec3 p;
	glm::vec3 pNext;
	glm::vec3 up;
	glm::vec3 upNext;
	m_currentDist += m_dt * 0.1f * m_Speed; //can use m_cameraSpeed if introduced.
	m_CatmullRom->Sample(m_currentDist, p, up);
	m_CatmullRom->Sample((m_currentDist + 0.1f), pNext, upNext);

	glm::vec3 T = glm::normalize(pNext - p);
	glm::vec3 N = glm::normalize(glm::cross(T, up));
	glm::vec3 B = glm::normalize(glm::cross(N, T));

	//Calculates required information for CatmullRom splines for the side road.
	glm::vec3 ap;
	glm::vec3 aN;
	m_Road->Sample(m_currentDist, ap);
	m_Road->Sample(m_currentDist + 0.1f, aN);
	glm::vec3 y = glm::vec3(0, 1, 0);
	glm::vec3 t = glm::normalize(aN - ap);
	glm::vec3 n = glm::normalize(glm::cross(t, y));
	glm::vec3 b = glm::normalize(glm::cross(n, t));

	// Update the camera using the amount of time that has elapsed to avoid framerate dependent motion
	if (isFreeview) {
		m_pCamera->Update(m_dt);
	}
	else if (isFPview) {
		m_pCamera->Set(p + glm::vec3(0.f, 10.f, 0.f), p + 50.f * T, B);
	}
	else if (isTPview) {
		m_pCamera->Set(p + glm::vec3(100.f,100.f, 100.f), fighter_position, B);
	}
	else if (isTopDownview) {
		m_pCamera->Set(glm::vec3(fighter_position.x, 1000, fighter_position.z), fighter_position, glm::vec3(1.f, 0.f, 0.f));
	}

	//Fighter ship calculations. 
	fighter_position = p + glm::vec3(0.f, 8.f, 0.f) + Turn;
	fighter_orientation = glm::mat4(glm::mat3(T, B, N));
	if (Turn.x > 5.f) 
		Turn.x = 5.f;
	else if (Turn.x < -5.f)
		Turn.x = -5.f;
	if (m_Speed > 4.f)
		m_Speed = 4.f;
	else if (m_Speed < 1.f) {
		m_Speed = 1.f;
	}

	//Car calculations
	car_position = ap + glm::vec3(0.f, 1.f, 0.f);
	car_orientation = glm::mat4(glm::mat3(t, b, n));

	//Earth calculations:
	m_rotY += 0.001f * m_dt;

	m_CurrSpeed = m_Speed * 100.f;

	m_pAudio->Update(fighter_position, car_position, m_pCamera->GetUpVector(), m_pCamera->GetStrafeVector(), m_pCamera->GetPosition());

	//Plays the music:
	if (isMusic)
		m_pAudio->PlayMusicStream();
}

void Game::DisplayFrameRate()
{
	CShaderProgram *fontProgram = (*m_pShaderPrograms)[1];

	RECT dimensions = m_gameWindow.GetDimensions();
	int height = dimensions.bottom - dimensions.top;

	// Increase the elapsed time and frame counter
	m_elapsedTime += m_dt;
	m_frameCount++;

	// Now we want to subtract the current time by the last time that was stored
	// to see if the time elapsed has been over a second, which means we found our FPS.
	if (m_elapsedTime > 1000)
    {
		m_elapsedTime = 0;
		m_framesPerSecond = m_frameCount;

		// Reset the frames per second
		m_frameCount = 0;
    }

	if (m_framesPerSecond > 0) {
		// Use the font shader program and render the text
		fontProgram->UseProgram();
		glDisable(GL_DEPTH_TEST);
		fontProgram->SetUniform("matrices.modelViewMatrix", glm::mat4(1));
		fontProgram->SetUniform("matrices.projMatrix", m_pCamera->GetOrthographicProjectionMatrix());
		fontProgram->SetUniform("vColour", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
		m_pFtFont->Render(20, height - 20, 20, "FPS: %d", m_framesPerSecond); 
		fontProgram->SetUniform("vColour", glm::vec4(1.f, 0.f, 0.f, 1.f));
		m_pFtFont->Render(20, height - 40, 20, "X: %f", m_pCamera->GetPosition().x);
		fontProgram->SetUniform("vColour", glm::vec4(0.f, 1.f, 0.f, 1.f));
		m_pFtFont->Render(20, height - 60, 20, "Y: %f", m_pCamera->GetPosition().y);
		fontProgram->SetUniform("vColour", glm::vec4(0.f, 0.f, 1.f, 1.f));
		m_pFtFont->Render(20, height - 80, 20, "Z: %f", m_pCamera->GetPosition().z);
		fontProgram->SetUniform("vColour", glm::vec4(0.f, 1.f, 1.f, 1.f));
		m_pFtFont->Render(40, height - 540, 20, "F1 = Free View, F2/F3 = First and Third Person view, F4 = Top Down View");
		fontProgram->SetUniform("vColour", glm::vec4(1.f, 0.f, 1.f, 1.f));
		m_pFtFont->Render(600, height - 20, 20, "Speed: %f", m_CurrSpeed);
		if (isFinished) {
			fontProgram->SetUniform("vColour", glm::vec4(1.f, 1.f, 1.f, 1.f));
			m_pFtFont->Render(300, height - 250, 40, "You've won!!!");
			m_pFtFont->Render(300, height - 290, 40, "Congratulations!");
			//m_pFtFont->Render(300, height - 330, 20, "Your score is : ");
		}
		else if (isDead) {
			fontProgram->SetUniform("vColour", glm::vec4(1.f, 0.f, 0.f, 1.f));
			m_pFtFont->Render(300, height - 250, 40, "You've lost!!!");
			//m_pFtFont->Render(300, height - 330, 20, "Your score is : ");
		}
	}
}

// The game loop runs repeatedly until game over
void Game::GameLoop()
{
	/*
	// Fixed timer
	dDt = pHighResolutionTimer->Elapsed();
	if (dDt > 1000.0 / (double) Game::FPS) {
		pHighResolutionTimer->Start();
		Update();
		Render();
	}
	*/
	
	
	// Variable timer
	m_pHighResolutionTimer->Start();
	Update();
	Render();
	m_dt = m_pHighResolutionTimer->Elapsed();
	
}


WPARAM Game::Execute() 
{
	m_pHighResolutionTimer = new CHighResolutionTimer;
	m_gameWindow.Init(m_hInstance);

	if(!m_gameWindow.Hdc()) {
		return 1;
	}

	Initialise();

	m_pHighResolutionTimer->Start();

	
	MSG msg;

	while(1) {													
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) { 
			if(msg.message == WM_QUIT) {
				break;
			}

			TranslateMessage(&msg);	
			DispatchMessage(&msg);
		} else if (m_appActive) {
			GameLoop();
		} 
		else Sleep(200); // Do not consume processor power if application isn't active
	}

	m_gameWindow.Deinit();

	return(msg.wParam);
}

LRESULT Game::ProcessEvents(HWND window,UINT message, WPARAM w_param, LPARAM l_param) 
{
	LRESULT result = 0;

	switch (message) {


	case WM_ACTIVATE:
	{
		switch(LOWORD(w_param))
		{
			case WA_ACTIVE:
			case WA_CLICKACTIVE:
				m_appActive = true;
				m_pHighResolutionTimer->Start();
				break;
			case WA_INACTIVE:
				m_appActive = false;
				break;
		}
		break;
		}

	case WM_SIZE:
			RECT dimensions;
			GetClientRect(window, &dimensions);
			m_gameWindow.SetDimensions(dimensions);
		break;

	case WM_PAINT:
		PAINTSTRUCT ps;
		BeginPaint(window, &ps);
		EndPaint(window, &ps);
		break;

	case WM_KEYDOWN:
		switch(w_param) {
		case VK_ESCAPE:
			PostQuitMessage(0);
			break;
		case '1':
			m_pAudio->Pause_fighter_sound();
			break;
		case '2':
			m_pAudio->Unpause_fighter_sound();
			break;
		case '3':
			
			break;
		case '5':
			
			break;
		case '6':
			isDead = true;
			break;
		case 'Q':
			m_Speed += 1.f;
			break;
		case 'E':
			m_Speed -= 1.f;
			break;
		case 'L':
			m_pAudio->PlayEventSound();
			break;
		case 'M':
			if (!isMusic)
				isMusic = true;
			else
				isMusic = false;
			break;
		case 'N':
			if (!isNight)
				isNight = true;
			else
				isNight = false;
			break;
		case 'B':
			if (!isDynamic)
				isDynamic = true;
			else
				isDynamic = false;
			break;
		case 'F':
			if (!fog)
				fog = true;
			else
				fog = false;
			break;
		case 'V':
			if (!isAppearing)
				isAppearing = true;
			else
				isAppearing = false;
			break;
		case VK_F1:
			isFPview = false;
			isTPview = false;
			isTopDownview = false;
			m_pCamera->Set(m_pCamera->GetPosition(), glm::vec3(0.f, 1.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
			isFreeview = true;
			break;
		case VK_F2:
			isTPview = false;
			isTopDownview = false;
			isFreeview = false;
			isFPview = true;
			break;
		case VK_F3:
			isTopDownview = false;
			isFreeview = false;
			isFPview = false;
			isTPview = true;
			break;
		case VK_F4:
			isTPview = false;
			isFPview = false;
			isFreeview = false;
			isTopDownview = true;
			break;

		}
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		result = DefWindowProc(window, message, w_param, l_param);
		break;
	}

	return result;
}

Game& Game::GetInstance() 
{
	static Game instance;

	return instance;
}

void Game::SetHinstance(HINSTANCE hinstance) 
{
	m_hInstance = hinstance;
}

LRESULT CALLBACK WinProc(HWND window, UINT message, WPARAM w_param, LPARAM l_param)
{
	return Game::GetInstance().ProcessEvents(window, message, w_param, l_param);
}

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE, PSTR, int) 
{
	Game &game = Game::GetInstance();
	game.SetHinstance(hinstance);

	return game.Execute();
}
