#include "globalUtil.h"
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
//#include <time.h>

nPhysics::iPhysicsFactory* gPhysicsFactory = 0;
nPhysics::iPhysicsWorld* gPhysicsWorld = 0;
std::vector<cGameObject*> controlTargets;
unsigned int controlIndex = 0;
int gamePoints = 0;
int lives = 3;
cGameObject* flipperRight;
cGameObject* flipperLeft;
cGameObject* plunger;
cGameObject* ball;
bool gameOver = false;

std::map<std::string, cMesh> mapNameToMesh;
std::vector<sModelDrawInfo> g_vec_sModelDrawInfo;
GLuint shaderProgID;
cVAOManager* pTheVAOManager = new cVAOManager();
cBasicTextureManager* g_pTextureManager = new cBasicTextureManager();
//cFBO* pTheFBO = new cFBO();
//cParticleEmitter* collisionParticles = new cParticleEmitter();
//cParticleEmitter* tailDustParticles = new cParticleEmitter();

bool firstMouse = true;
float lastX, lastY;
bool g_MouseLeftButtonIsDown = false;
double g_mouseX, g_mouseY, g_scrollY;
float g_FOV = 45.0f;

cCamera camera;
glm::vec3 followCameraTarget;
glm::mat4 view;
//float cameraSpeedX = 0.0f;
//float cameraSpeedY = 0.0f;
//float cameraSpeedZ = 0.0f;
glm::vec3 cameraVelocity = glm::vec3(0.0f, 0.0f, 0.0f);
float camerShakeAmplitude = 0.0f;
bool camerShakeCooldown = true;

float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame
float lastTime = 0.0f;
float now = 0.0f;

const float LightConstAtten = 0.0000001f;
//float LightLinearAtten = 0.02f;
//float LightQuadraticAtten = 0.0000001f; //0.0000001f;

glm::vec3 movePosition = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec4 circleRGB = glm::vec4(0.9f, 0.3f, 0.3f, 1.0f);
GLint circleDiffuseRGB;

std::vector<cGameObject*> g_vec_pGameObjects;
std::map<std::string /*FriendlyName*/, cGameObject*> g_map_GameObjectsByFriendlyName;
std::vector<cGameObject*> g_vec_pDebugGameObjects;
std::vector<cGameObject*> g_vec_pTransparentGameObjects;
//std::vector<cParticle*> vecParticles;
cGameObject* pFindObjectByFriendlyName(std::string name);

//cMesh transformedMesh;
double averageDeltaTime;
//cGameObject* pDebugSphere;
//cGameObject* pCube;
//cGameObject* pQuadImposter;

bool debugMode = true;
bool wireframeMode = false;
bool normalPassMode = false;
bool albedoPassMode = false;
bool useFollowCamera = true;


//const float SPEED_CHANGE = 0.2f;
//const float FREE_ANGLE_CHANGE = 150.0f;
//const float DRONE_ANGLE_CHANGE = 100.0f;


bool LoadConfig()
{
	// Load shader
	cShaderManager* pTheShaderManager = new cShaderManager();
	cShaderManager::cShader vertexShad;
	vertexShad.fileName = "assets/shaders/vertexShader01.glsl";
	cShaderManager::cShader fragShader;
	fragShader.fileName = "assets/shaders/fragmentShader01.glsl";
	if (!pTheShaderManager->createProgramFromFile("SimpleShader", vertexShad, fragShader))
	{
		std::cout << "Error: didn't compile the shader" << std::endl;
		std::cout << pTheShaderManager->getLastError();
		return -1;
	}
	shaderProgID = pTheShaderManager->getIDFromFriendlyName("SimpleShader");

	// Load mesh
	std::ifstream pathFile("config/mesh_path.txt");
	if (!pathFile.is_open())
	{
		std::cout << "LoadConfig() can't find the mesh path file." << std::endl;
		return false;
	}

	std::string line;
	int number_of_lines = 0;
	while (std::getline(pathFile, line))
		++number_of_lines;

	std::cout << number_of_lines << " meshes will be loaded by config." << std::endl;
	cModelLoader* pTheModelLoader = new cModelLoader();

	g_vec_sModelDrawInfo.resize(number_of_lines);

	pathFile.clear();
	pathFile.seekg(0, pathFile.beg);
	for (unsigned int index = 0; index != number_of_lines; index++)
	{
		std::string mesh_name, mesh_path, errStr;

		pathFile >> mesh_name >> mesh_path;
		//pTheModelLoader->LoadPlyModel(mesh_path.c_str(), mapNameToMesh[mesh_name]);

		if (pTheModelLoader->LoadModel_Assimp(mesh_path.c_str(), mapNameToMesh[mesh_name], errStr))
		{
			if (mesh_name == "sphereMesh")
			{
				mapNameToMesh[mesh_name].GenTextureCoordsSpherical();
			}
			pTheVAOManager->LoadModelIntoVAO(mesh_name, mapNameToMesh[mesh_name], g_vec_sModelDrawInfo[index], shaderProgID);
		}

	}

	delete pTheModelLoader;
	delete pTheShaderManager;

	// Load gameObject with physics_config
	std::ifstream GameObjectFile("config/GameObject_config.txt");
	if (!GameObjectFile.is_open())
	{
		std::cout << "LoadConfig() can't find the GameObject config file." << std::endl;
		return false;
	}
	line = "";
	number_of_lines = 0;
	while (std::getline(GameObjectFile, line))
		++number_of_lines;

	std::cout << number_of_lines - 1 << " GameObject(s) will be created by config." << std::endl;

	::g_vec_pGameObjects.resize(number_of_lines - 1);

	GameObjectFile.clear();
	GameObjectFile.seekg(0, GameObjectFile.beg);
	std::string discardLine;
	GameObjectFile >> discardLine;

	// Load physics_config
	std::ifstream physicsFile("config/physics_config.json");
	if (!physicsFile.is_open())
	{
		std::cout << "LoadConfig() can't find the physics_config file." << std::endl;
		return false;
	}
	rapidjson::IStreamWrapper isw(physicsFile);
	rapidjson::Document jsonDocument;
	jsonDocument.ParseStream(isw);
	assert(jsonDocument.IsObject());
	
	for (unsigned int index = 0; index != number_of_lines - 1; index++)
	{
		std::string mesh_name, friendly_name, physics_shape_type, texture_0, texture_1, texture_blend_mode;
		float positionX, positionY, positionZ,
			rotationX, rotationY, rotationZ,
			scaleX, scaleY, scaleZ,
			diffuseColourR, diffuseColourG, diffuseColourB, transparency;

		GameObjectFile >> mesh_name >> friendly_name
			>> positionX >> positionY >> positionZ
			>> rotationX >> rotationY >> rotationZ
			>> scaleX >> scaleY >> scaleZ
			>> diffuseColourR >> diffuseColourG >> diffuseColourB >> transparency
			>> physics_shape_type
			>> texture_0 >> texture_1 >> texture_blend_mode;

		::g_vec_pGameObjects[index] = new cGameObject();
		::g_vec_pGameObjects[index]->meshName = mesh_name;
		::g_vec_pGameObjects[index]->friendlyName = friendly_name;
		::g_vec_pGameObjects[index]->positionXYZ = glm::vec3(positionX, positionY, positionZ);
		::g_vec_pGameObjects[index]->setOrientation(glm::vec3(rotationX, rotationY, rotationZ));
		::g_vec_pGameObjects[index]->scaleXYZ = glm::vec3(scaleX, scaleY, scaleZ);
		::g_vec_pGameObjects[index]->diffuseColour = glm::vec4(diffuseColourR, diffuseColourG, diffuseColourB, transparency);
		std::map<std::string, eShapeTypes> stringToPSTEnum = {{"AABB", AABB},
																{"SPHERE", SPHERE},
																{"CAPSULE", CAPSULE},
																{"PLANE", PLANE},
																{"MESH", MESH},
																{"BOX", BOX},
																{"CYLINDER", CYLINDER},
																{"SPINNER", SPINNER},
																{"TRIGGER", TRIGGER},
																{"FLIPPER", FLIPPER},
																{"PLUNGER", PLUNGER},
																{"UNKNOWN", UNKNOWN} };
		::g_vec_pGameObjects[index]->physicsShapeType = stringToPSTEnum[physics_shape_type];
		::g_vec_pGameObjects[index]->textures[0] = texture_0;
		::g_vec_pGameObjects[index]->textures[1] = texture_1;
		std::map<std::string, eTextureBlend> stringToTBMEnum = { {"NO_TEXTURE", NO_TEXTURE}, {"BASE", BASE}, {"DIFFUSE_COLOUR", DIFFUSE_COLOUR}, {"COLOUR", COLOUR}, {"MULTIPLY", MULTIPLY}, {"AVERAGE", AVERAGE}, {"SOFT_LIGHT", SOFT_LIGHT}, {"IMPOSTER", IMPOSTER}, {"CUBE_MAP", CUBE_MAP} };
		::g_vec_pGameObjects[index]->textureBlendMode = stringToTBMEnum[texture_blend_mode];
		::g_vec_pGameObjects[index]->UpdateWorldMatrix();
		::g_vec_pGameObjects[index]->CalculateAABBMinMax();

		if (g_vec_pGameObjects[index]->physicsShapeType == SPHERE)
		{
			controlTargets.push_back(g_vec_pGameObjects[index]);
			if (!jsonDocument.HasMember(friendly_name.c_str()))
			{
				printf("physics_config json file can't find the object member -> %s\n", friendly_name.c_str());
				return false;
			}
			assert(jsonDocument[friendly_name.c_str()].IsObject());
			rapidjson::Value& ballComponent = jsonDocument[friendly_name.c_str()];

			nPhysics::sBallDef defBall;

			assert(ballComponent["Mass"].IsDouble());
			defBall.Mass = static_cast<float>(ballComponent["Mass"].GetDouble());
			assert(ballComponent["Radius"].IsDouble());
			defBall.Radius = static_cast<float>(ballComponent["Radius"].GetDouble());
			assert(ballComponent["Position"].IsArray());
			rapidjson::Value& posArray = ballComponent["Position"];
			defBall.Position = glm::vec3(static_cast<float>(posArray[0].GetDouble()),
				static_cast<float>(posArray[1].GetDouble()),
				static_cast<float>(posArray[2].GetDouble()));

			nPhysics::iPhysicsComponent* pBall = gPhysicsFactory->CreateBall(defBall);
			pBall->SetUniqueID(g_vec_pGameObjects[index]->getUniqueID());
			gPhysicsWorld->AddComponent(pBall);
			g_vec_pGameObjects[index]->physicsComponent = pBall;
			g_vec_pGameObjects[index]->SPHERE_radius = defBall.Radius;
		}

		if (g_vec_pGameObjects[index]->physicsShapeType == PLANE)
		{
			if (!jsonDocument.HasMember(friendly_name.c_str()))
			{
				printf("physics_config json file can't find the object member -> %s\n", friendly_name.c_str());
				return false;
			}
			assert(jsonDocument[friendly_name.c_str()].IsObject());
			rapidjson::Value& planeComponent = jsonDocument[friendly_name.c_str()];

			nPhysics::sPlaneDef defPlane;

			assert(planeComponent["Constant"].IsDouble());
			defPlane.Constant = static_cast<float>(planeComponent["Constant"].GetDouble());			
			assert(planeComponent["Normal"].IsArray());
			rapidjson::Value& norArray = planeComponent["Normal"];
			defPlane.Normal = glm::vec3(static_cast<float>(norArray[0].GetDouble()),
				static_cast<float>(norArray[1].GetDouble()),
				static_cast<float>(norArray[2].GetDouble()));

			nPhysics::iPhysicsComponent* pPlane = gPhysicsFactory->CreatePlane(defPlane);
			pPlane->SetUniqueID(g_vec_pGameObjects[index]->getUniqueID());
			gPhysicsWorld->AddComponent(pPlane);
			g_vec_pGameObjects[index]->physicsComponent = pPlane;
		}

		if (g_vec_pGameObjects[index]->physicsShapeType == BOX)
		{
			if (!jsonDocument.HasMember(friendly_name.c_str()))
			{
				printf("physics_config json file can't find the object member -> %s\n", friendly_name.c_str());
				return false;
			}
			assert(jsonDocument[friendly_name.c_str()].IsObject());
			rapidjson::Value& boxComponent = jsonDocument[friendly_name.c_str()];

			nPhysics::sBoxDef defBox;

			assert(boxComponent["Mass"].IsDouble());
			defBox.Mass = static_cast<float>(boxComponent["Mass"].GetDouble());
			assert(boxComponent["Position"].IsArray());
			rapidjson::Value& posArray = boxComponent["Position"];
			defBox.Position = glm::vec3(static_cast<float>(posArray[0].GetDouble()),
				static_cast<float>(posArray[1].GetDouble()),
				static_cast<float>(posArray[2].GetDouble()));
			assert(boxComponent["HalfExtents"].IsArray());
			rapidjson::Value& scaleArray = boxComponent["HalfExtents"];
			defBox.HalfExtents = glm::vec3(static_cast<float>(scaleArray[0].GetDouble()),
				static_cast<float>(scaleArray[1].GetDouble()),
				static_cast<float>(scaleArray[2].GetDouble()));
			assert(boxComponent["Rotation"].IsArray());
			rapidjson::Value& rotArray = boxComponent["Rotation"];
			defBox.Rotation = glm::vec3(static_cast<float>(rotArray[0].GetDouble()),
				static_cast<float>(rotArray[1].GetDouble()),
				static_cast<float>(rotArray[2].GetDouble()));

			nPhysics::iPhysicsComponent* pBox = gPhysicsFactory->CreateBox(defBox);
			pBox->SetUniqueID(g_vec_pGameObjects[index]->getUniqueID());
			gPhysicsWorld->AddComponent(pBox);
			g_vec_pGameObjects[index]->physicsComponent = pBox;
		}

		if (g_vec_pGameObjects[index]->physicsShapeType == CYLINDER)
		{
			if (!jsonDocument.HasMember(friendly_name.c_str()))
			{
				printf("physics_config json file can't find the object member -> %s\n", friendly_name.c_str());
				return false;
			}
			assert(jsonDocument[friendly_name.c_str()].IsObject());
			rapidjson::Value& cylinderComponent = jsonDocument[friendly_name.c_str()];

			nPhysics::sCylinderDef defCylinder;

			assert(cylinderComponent["Mass"].IsDouble());
			defCylinder.Mass = static_cast<float>(cylinderComponent["Mass"].GetDouble());
			assert(cylinderComponent["Radius"].IsDouble());
			defCylinder.Radius = static_cast<float>(cylinderComponent["Radius"].GetDouble());
			assert(cylinderComponent["HalfHeight"].IsDouble());
			defCylinder.HalfHeight = static_cast<float>(cylinderComponent["HalfHeight"].GetDouble());
			assert(cylinderComponent["Position"].IsArray());
			rapidjson::Value& posArray = cylinderComponent["Position"];
			defCylinder.Position = glm::vec3(static_cast<float>(posArray[0].GetDouble()),
				static_cast<float>(posArray[1].GetDouble()),
				static_cast<float>(posArray[2].GetDouble()));
			assert(cylinderComponent["Rotation"].IsArray());
			rapidjson::Value& rotArray = cylinderComponent["Rotation"];
			defCylinder.Rotation = glm::vec3(static_cast<float>(rotArray[0].GetDouble()),
				static_cast<float>(rotArray[1].GetDouble()),
				static_cast<float>(rotArray[2].GetDouble()));

			nPhysics::iPhysicsComponent* pCylinder = gPhysicsFactory->CreateCylinder(defCylinder);
			pCylinder->SetUniqueID(g_vec_pGameObjects[index]->getUniqueID());
			gPhysicsWorld->AddComponent(pCylinder);
			g_vec_pGameObjects[index]->physicsComponent = pCylinder;
		}

		if (g_vec_pGameObjects[index]->physicsShapeType == SPINNER)
		{
			if (!jsonDocument.HasMember(friendly_name.c_str()))
			{
				printf("physics_config json file can't find the object member -> %s\n", friendly_name.c_str());
				return false;
			}
			assert(jsonDocument[friendly_name.c_str()].IsObject());
			rapidjson::Value& spinnerComponent = jsonDocument[friendly_name.c_str()];

			nPhysics::sSpinnerDef defSpinner;

			assert(spinnerComponent["Mass"].IsDouble());
			defSpinner.Mass = static_cast<float>(spinnerComponent["Mass"].GetDouble());
			assert(spinnerComponent["Radius"].IsDouble());
			defSpinner.Radius = static_cast<float>(spinnerComponent["Radius"].GetDouble());
			assert(spinnerComponent["BetweenHeight"].IsDouble());
			defSpinner.BetweenHeight = static_cast<float>(spinnerComponent["BetweenHeight"].GetDouble());
			assert(spinnerComponent["Position"].IsArray());
			rapidjson::Value& posArray = spinnerComponent["Position"];
			defSpinner.Position = glm::vec3(static_cast<float>(posArray[0].GetDouble()),
				static_cast<float>(posArray[1].GetDouble()),
				static_cast<float>(posArray[2].GetDouble()));
			assert(spinnerComponent["Rotation"].IsArray());
			rapidjson::Value& rotArray = spinnerComponent["Rotation"];
			defSpinner.Rotation = glm::vec3(static_cast<float>(rotArray[0].GetDouble()),
				static_cast<float>(rotArray[1].GetDouble()),
				static_cast<float>(rotArray[2].GetDouble()));

			nPhysics::iPhysicsComponent* pSpinner = gPhysicsFactory->CreateSpinner(defSpinner);
			pSpinner->SetUniqueID(g_vec_pGameObjects[index]->getUniqueID());
			gPhysicsWorld->AddComponent(pSpinner);
			g_vec_pGameObjects[index]->physicsComponent = pSpinner;
		}

		if (g_vec_pGameObjects[index]->physicsShapeType == TRIGGER)
		{
			if (!jsonDocument.HasMember(friendly_name.c_str()))
			{
				printf("physics_config json file can't find the object member -> %s\n", friendly_name.c_str());
				return false;
			}
			assert(jsonDocument[friendly_name.c_str()].IsObject());
			rapidjson::Value& triggerComponent = jsonDocument[friendly_name.c_str()];

			nPhysics::sTriggerDef defTrigger;

			assert(triggerComponent["Position"].IsArray());
			rapidjson::Value& posArray = triggerComponent["Position"];
			defTrigger.Position = glm::vec3(static_cast<float>(posArray[0].GetDouble()),
				static_cast<float>(posArray[1].GetDouble()),
				static_cast<float>(posArray[2].GetDouble()));
			assert(triggerComponent["HalfExtents"].IsArray());
			rapidjson::Value& scaleArray = triggerComponent["HalfExtents"];
			defTrigger.HalfExtents = glm::vec3(static_cast<float>(scaleArray[0].GetDouble()),
				static_cast<float>(scaleArray[1].GetDouble()),
				static_cast<float>(scaleArray[2].GetDouble()));

			nPhysics::iPhysicsComponent* pTrigger = gPhysicsFactory->CreateTrigger(defTrigger);
			gPhysicsWorld->AddComponent(pTrigger);
			pTrigger->SetUniqueID(g_vec_pGameObjects[index]->getUniqueID());
			g_vec_pGameObjects[index]->physicsComponent = pTrigger;
			g_vec_pGameObjects[index]->isVisible = false;
		}

		if (g_vec_pGameObjects[index]->physicsShapeType == FLIPPER)
		{
			if (!jsonDocument.HasMember(friendly_name.c_str()))
			{
				printf("physics_config json file can't find the object member -> %s\n", friendly_name.c_str());
				return false;
			}
			assert(jsonDocument[friendly_name.c_str()].IsObject());
			rapidjson::Value& flipperComponent = jsonDocument[friendly_name.c_str()];

			nPhysics::sFlipperDef defFlipper;

			assert(flipperComponent["Mass"].IsDouble());
			defFlipper.Mass = static_cast<float>(flipperComponent["Mass"].GetDouble());
			assert(flipperComponent["Position"].IsArray());
			rapidjson::Value& posArray = flipperComponent["Position"];
			defFlipper.Position = glm::vec3(static_cast<float>(posArray[0].GetDouble()),
				static_cast<float>(posArray[1].GetDouble()),
				static_cast<float>(posArray[2].GetDouble()));
			assert(flipperComponent["HalfExtents"].IsArray());
			rapidjson::Value& scaleArray = flipperComponent["HalfExtents"];
			defFlipper.HalfExtents = glm::vec3(static_cast<float>(scaleArray[0].GetDouble()),
				static_cast<float>(scaleArray[1].GetDouble()),
				static_cast<float>(scaleArray[2].GetDouble()));
			assert(flipperComponent["Rotation"].IsArray());
			rapidjson::Value& rotArray = flipperComponent["Rotation"];
			defFlipper.Rotation = glm::vec3(static_cast<float>(rotArray[0].GetDouble()),
				static_cast<float>(rotArray[1].GetDouble()),
				static_cast<float>(rotArray[2].GetDouble()));

			nPhysics::iPhysicsComponent* pFlipper = gPhysicsFactory->CreateFlipper(defFlipper);
			pFlipper->SetUniqueID(g_vec_pGameObjects[index]->getUniqueID());
			gPhysicsWorld->AddComponent(pFlipper);
			g_vec_pGameObjects[index]->physicsComponent = pFlipper;
		}

		if (g_vec_pGameObjects[index]->physicsShapeType == PLUNGER)
		{
			if (!jsonDocument.HasMember(friendly_name.c_str()))
			{
				printf("physics_config json file can't find the object member -> %s\n", friendly_name.c_str());
				return false;
			}
			assert(jsonDocument[friendly_name.c_str()].IsObject());
			rapidjson::Value& plungerComponent = jsonDocument[friendly_name.c_str()];

			nPhysics::sPlungerDef defPlunger;

			assert(plungerComponent["Mass"].IsDouble());
			defPlunger.Mass = static_cast<float>(plungerComponent["Mass"].GetDouble());
			assert(plungerComponent["Position"].IsArray());
			rapidjson::Value& posArray = plungerComponent["Position"];
			defPlunger.Position = glm::vec3(static_cast<float>(posArray[0].GetDouble()),
				static_cast<float>(posArray[1].GetDouble()),
				static_cast<float>(posArray[2].GetDouble()));
			assert(plungerComponent["HalfExtents"].IsArray());
			rapidjson::Value& scaleArray = plungerComponent["HalfExtents"];
			defPlunger.HalfExtents = glm::vec3(static_cast<float>(scaleArray[0].GetDouble()),
				static_cast<float>(scaleArray[1].GetDouble()),
				static_cast<float>(scaleArray[2].GetDouble()));
			assert(plungerComponent["Rotation"].IsArray());
			rapidjson::Value& rotArray = plungerComponent["Rotation"];
			defPlunger.Rotation = glm::vec3(static_cast<float>(rotArray[0].GetDouble()),
				static_cast<float>(rotArray[1].GetDouble()),
				static_cast<float>(rotArray[2].GetDouble()));

			nPhysics::iPhysicsComponent* pPlunger = gPhysicsFactory->CreatePlunger(defPlunger);
			pPlunger->SetUniqueID(g_vec_pGameObjects[index]->getUniqueID());
			gPhysicsWorld->AddComponent(pPlunger);
			g_vec_pGameObjects[index]->physicsComponent = pPlunger;

		}
	}

	//pFindObjectByFriendlyName("frontPlane")->isVisible = false;
	//pFindObjectByFriendlyName("backPlane")->isVisible = false;
	//pFindObjectByFriendlyName("leftPlane")->isVisible = false;
	//pFindObjectByFriendlyName("rightPlane")->isVisible = false;

	controlTargets[controlIndex]->diffuseColour = glm::vec4(0.8f, 0.8f, 0.2f, 1.0f);


	// For collision particles
	//pCube = new cGameObject();
	//pCube->meshName = "cubeMesh";
	//pCube->friendlyName = "cubeParticle";
	//pCube->diffuseColour = glm::vec4(1.0f, 0.25f, 0.0f, 1.0f);

	
	// Transfer transparent GameObjects
	std::vector<cGameObject*>::iterator itTransfer = g_vec_pGameObjects.begin();
	while (itTransfer != g_vec_pGameObjects.end())
	{
		cGameObject* pCurGameObject = *itTransfer;	// Dereference iterator, giving us the original type

		if (pCurGameObject->diffuseColour.a < 1.0f || pCurGameObject->useTransparentMap == true)
		{
			g_vec_pTransparentGameObjects.push_back(pCurGameObject);
			itTransfer = g_vec_pGameObjects.erase(itTransfer);
		}
		else
		{
			++itTransfer;
		}
	}

	// load camera init position
	std::ifstream cameraFile("config/camera_config.txt");
	if (!cameraFile.is_open())
	{
		std::cout << "Can't find the camera config file." << std::endl;
		return false;
	}
	float cameraPositionX, cameraPositionY, cameraPositionZ;
	cameraFile >> discardLine >> cameraPositionX >> cameraPositionY >> cameraPositionZ;
	camera.Position = glm::vec3(cameraPositionX, cameraPositionY, cameraPositionZ);	

	// Load light config
	std::ifstream lightFile("config/light_config.txt");
	if (!lightFile.is_open())
	{
		std::cout << "LoadConfig() can't find the light config file." << std::endl;
		return false;
	}
	line = "";
	number_of_lines = 0;
	while (std::getline(lightFile, line))
		++number_of_lines;

	std::cout << number_of_lines - 1 << " light(s) will be created by config." << std::endl;
	lightFile.clear();
	lightFile.seekg(0, lightFile.beg);
	lightFile >> discardLine;
	glUseProgram(shaderProgID);
	for (unsigned int index = 0; index != number_of_lines - 1; index++)
	{
		float positionX, positionY, positionZ,
			diffuseR, diffuseG, diffuseB,
			specularR, specularG, specularB,
			linearAtten, quadraticAtten,
			directionX, directionY, directionZ,
			lightType, innerAngle, outerAngle,
			lightSwitch;

		lightFile >> positionX >> positionY >> positionZ
			>> diffuseR >> diffuseG >> diffuseB
			>> specularR >> specularG >> specularB
			>> linearAtten >> quadraticAtten
			>> directionX >> directionY >> directionZ
			>> lightType >> innerAngle >> outerAngle
			>> lightSwitch;

		std::ostringstream oss;
		oss << "theLights[" << index << "].position";
		GLint position = glGetUniformLocation(shaderProgID, oss.str().c_str());

		oss.clear();
		oss.str("");
		oss << "theLights[" << index << "].diffuse";
		GLint diffuse = glGetUniformLocation(shaderProgID, oss.str().c_str());

		oss.clear();
		oss.str("");
		oss << "theLights[" << index << "].specular";
		GLint specular = glGetUniformLocation(shaderProgID, oss.str().c_str());

		oss.clear();
		oss.str("");
		oss << "theLights[" << index << "].atten";
		GLint atten = glGetUniformLocation(shaderProgID, oss.str().c_str());

		oss.clear();
		oss.str("");
		oss << "theLights[" << index << "].direction";
		GLint direction = glGetUniformLocation(shaderProgID, oss.str().c_str());

		oss.clear();
		oss.str("");
		oss << "theLights[" << index << "].param1";
		GLint param1 = glGetUniformLocation(shaderProgID, oss.str().c_str());

		oss.clear();
		oss.str("");
		oss << "theLights[" << index << "].param2";
		GLint param2 = glGetUniformLocation(shaderProgID, oss.str().c_str());

		glUniform4f(position, positionX, positionY, positionZ, 1.0f);
		glUniform4f(diffuse, diffuseR, diffuseG, diffuseB, 1.0f);
		glUniform4f(specular, specularR, specularG, specularB, 1.0f);
		glUniform4f(atten, LightConstAtten, linearAtten, quadraticAtten, 1000000.0f);
		glm::vec3 normalizedDirection = glm::normalize(glm::vec3(directionX, directionY, directionZ));
		glUniform4f(direction, normalizedDirection.x, normalizedDirection.y, normalizedDirection.z, 1.0f);
		glUniform4f(param1, lightType, innerAngle, outerAngle, 1.0f);
		glUniform4f(param2, lightSwitch, 0.0f, 0.0f, 1.0f);
	}
	

	// Load texture config
	std::ifstream textureFile("config/texture_config.txt");
	if (!textureFile.is_open())
	{
		std::cout << "LoadConfig() can't find the texture config file." << std::endl;
		return false;
	}
	line = "";
	number_of_lines = 0;
	while (std::getline(textureFile, line))
		++number_of_lines;
	std::cout << number_of_lines - 1 << " texture(s) will be loaded by config." << std::endl;
	textureFile.clear();
	textureFile.seekg(0, textureFile.beg);
	std::string basePath;
	textureFile >> basePath;
	g_pTextureManager->SetBasePath(basePath);
	for (unsigned int index = 0; index != number_of_lines - 1; index++)
	{
		std::string fileName;
		textureFile >> fileName;
		g_pTextureManager->Create2DTextureFromBMPFile(fileName, true);
	}

	// Load cube map config
	//std::ifstream cubemapFile("config/cubemap_config.txt");
	//if (!cubemapFile.is_open())
	//{
	//	std::cout << "LoadConfig() can't find the cube map config file." << std::endl;
	//	return false;
	//}	
	//basePath = "";
	//cubemapFile >> basePath;
	//g_pTextureManager->SetBasePath(basePath);
	//std::string cubemapRight, cubemapLeft, cubemapTop, cubemapDown, cubemapFront, cubemapBack, errorString;
	//cubemapFile >> cubemapRight >> cubemapLeft >> cubemapTop >> cubemapDown >> cubemapFront >> cubemapBack;
	//if (::g_pTextureManager->CreateCubeTextureFromBMPFiles("skyBoxTextures",
	//	cubemapRight, cubemapLeft,
	//	cubemapTop, cubemapDown,
	//	cubemapFront, cubemapBack, true, errorString))
	//{
	//	std::cout << "skybox textures loaded" << std::endl;
	//}
	//else
	//{
	//	std::cout << "skybox textures not loaded. " << errorString << std::endl;
	//}	


	// Load Lua scripts	
	//std::ifstream LuaFile("config/LuaScripts.lua");
	//if (!LuaFile.is_open())
	//{
	//	std::cout << "LoadConfig() can't find the Lua scripts file." << std::endl;
	//	return false;
	//}
	//line = "";
	//number_of_lines = 0;
	//std::stringstream LuaScripts;
	//while (std::getline(LuaFile, line))
	//{
	//	LuaScripts << line;
	//	std::cout << line << std::endl;
	//	++number_of_lines;
	//}
	//std::cout << number_of_lines << " Lua line(s) loaded by config." << std::endl;
	//::p_LuaScripts->SetObjectVector(&(::g_vec_pGameObjects));
	//::p_LuaScripts->RunScriptImmediately(LuaScripts.str());	


	//std::string FBOError;
	//if (pTheFBO->init(1920, 1080, FBOError))
	//{
	//	std::cout << "Frame buffer is OK" << std::endl;
	//}
	//else
	//{
	//	std::cout << "FBO Error: " << FBOError << std::endl;
	//}

	// Collision particle
	//cParticleEmitter::sParticleCreationSettings collisionParticleSettings;
	//collisionParticleSettings.minLifeSeconds = 0.3f;
	//collisionParticleSettings.maxLifeInSeconds = 1.5f;
	//collisionParticleSettings.particleCreationVolume = cParticleEmitter::sParticleCreationSettings::SPHERE;
	//collisionParticleSettings.minStartingScale = 0.1f;
	//collisionParticleSettings.maxStartingScale = 0.3f;
	//collisionParticleSettings.minScaleChangePercentPerSecond = 0.0f;
	//collisionParticleSettings.maxScaleChangePercentPerSecond = 0.0f;
	//collisionParticleSettings.numParticlesPerSecond = 30.0f;
	//collisionParticleSettings.isImposterFaceCamera = false;
	//collisionParticleSettings.bFadeOutOverLifetime = true;
	//collisionParticleSettings.minVelocity = glm::vec3(-30.0f, -30.0f, -30.0f);
	//collisionParticleSettings.maxVelocity = glm::vec3(30.0f, 30.0f, 30.0f);
	//collisionParticleSettings.minRotationalChangePerSecond = glm::quat( glm::vec3(45.0f, 45.0f, 45.0f) );
	//collisionParticleSettings.maxRotationalChangePerSecond = glm::quat( glm::vec3(360.0f, 360.0f, 360.0f) );
	//collisionParticles->Initialize(collisionParticleSettings);
	////collisionParticles->enableNewParticles();
	

	return true;
}

void InitPhysics()
{
	gPhysicsFactory = new MyPhysicsFactoryClass;
	gPhysicsWorld = gPhysicsFactory->CreateWorld();
}

void DestroyPhysics()
{
	if (gPhysicsWorld)
	{
		delete gPhysicsWorld;
		gPhysicsWorld = 0;
	}

	if (gPhysicsFactory)
	{
		delete gPhysicsFactory;
		gPhysicsFactory = 0;
	}

}

void processInput(GLFWwindow* window)
{
	if (gameOver)	return;

	glfwGetCursorPos(window, &g_mouseX, &g_mouseY);	

	if (!useFollowCamera)
	{
		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
			camera.ProcessKeyboard(FORWARD, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
			camera.ProcessKeyboard(BACKWARD, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
			camera.ProcessKeyboard(LEFT, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
			camera.ProcessKeyboard(RIGHT, deltaTime);
	}

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		controlTargets[controlIndex]->physicsComponent->ApplyForce(glm::vec3(0.0f, 0.0f, -50.0f));
		//movePosition.z -= 0.3f;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		controlTargets[controlIndex]->physicsComponent->ApplyForce(glm::vec3(0.0f, 0.0f, 50.0f));
		//movePosition.z += 0.3f;
	}

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		controlTargets[controlIndex]->physicsComponent->ApplyForce(glm::vec3(-50.0f, 0.0f, 0.0f));
		//movePosition.x -= 0.3f;
	}

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		controlTargets[controlIndex]->physicsComponent->ApplyForce(glm::vec3(50.0f, 0.0f, 0.0f));
		//movePosition.x += 0.3f;
	}

	if (glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS)
	{
		flipperRight->physicsComponent->SetMotorVelocity(-200.0f);
	}
	else
	{
		flipperRight->physicsComponent->SetMotorVelocity(200.0f);
	}

	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		flipperLeft->physicsComponent->SetMotorVelocity(200.0f);
	}
	else
	{
		flipperLeft->physicsComponent->SetMotorVelocity(-200.0f);
	}

	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		plunger->physicsComponent->SetMotorVelocity(5.0f);
		ball->physicsComponent->SetActive();
	}
	else
	{
		plunger->physicsComponent->SetMotorVelocity(-300.0f);
	}

	
}


void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	if (!useFollowCamera)
	{
		camera.ProcessMouseMovement(xoffset, yoffset);
	}
	//camera.ProcessMouseMovement(xoffset, yoffset);
}

cGameObject* pFindObjectByFriendlyName(std::string name)
{
	for (unsigned int index = 0; index != g_vec_pGameObjects.size(); index++)
	{
		if (::g_vec_pGameObjects[index]->friendlyName == name)
		{
			return ::g_vec_pGameObjects[index];
		}
	}

	for (unsigned int index = 0; index != g_vec_pTransparentGameObjects.size(); index++)
	{
		if (::g_vec_pTransparentGameObjects[index]->friendlyName == name)
		{
			return ::g_vec_pTransparentGameObjects[index];
		}
	}

	return NULL;
}

cGameObject* pFindObjectByFriendlyNameMap(std::string name)
{
	return ::g_map_GameObjectsByFriendlyName[name];
}

glm::vec4 RBGcircleUpdate(glm::vec4& colour)
{
	float circleSpeed = 0.005f;
	float upperBound = 0.9f;
	float lowerBound = 0.3f;

	if (colour.r >= upperBound && colour.g <= lowerBound && colour.b < upperBound)
	{
		colour.r = upperBound;
		colour.g = lowerBound;
		colour.b += circleSpeed;
	}
	else if (colour.r > lowerBound && colour.g <= lowerBound && colour.b >= upperBound)
	{
		colour.r -= circleSpeed;
		colour.g = lowerBound;
		colour.b = upperBound;
	}
	else if (colour.r <= lowerBound && colour.g < upperBound && colour.b >= upperBound)
	{
		colour.r = lowerBound;
		colour.g += circleSpeed;
		colour.b = upperBound;
	}
	else if (colour.r <= lowerBound && colour.g >= upperBound && colour.b > lowerBound)
	{
		colour.r = lowerBound;
		colour.g = upperBound;
		colour.b -= circleSpeed;
	}
	else if (colour.r < upperBound && colour.g >= upperBound && colour.b <= lowerBound)
	{
		colour.r += circleSpeed;
		colour.g = upperBound;
		colour.b = lowerBound;
	}
	else if (colour.r >= upperBound && colour.g > lowerBound && colour.b <= lowerBound)
	{
		colour.r = upperBound;
		colour.g -= circleSpeed;
		colour.b = lowerBound;
	}

	return colour;
}

float lerp(float a, float b, float f)
{
	return a + f * (b - a);
}

float SmoothDamp(float current, float target, float& currentSpeed, float smoothTime, float deltaTime)
{
	float num = 2.0f / smoothTime;
	float num2 = num * deltaTime;
	float num3 = 1.0f / (1.0f + num2 + 0.48f * num2 * num2 + 0.235f * num2 * num2 * num2);
	float num4 = current - target;
	float num5 = target;
	target = current - num4;
	float num7 = (currentSpeed + num * num4) * deltaTime;
	currentSpeed = (currentSpeed - num * num7) * num3;
	float num8 = target + (num4 + num7) * num3;
	if (num5 - current > 0.0f == num8 > num5)
	{
		num8 = num5;
		currentSpeed = (num8 - num5) / deltaTime;
	}
	return num8;
}

glm::vec3 SmoothFollow(glm::vec3 current, glm::vec3 target, glm::vec3& currentVelocity, float smoothTime, float deltaTime)
{
	glm::vec3 result;
	result.x = SmoothDamp(current.x, target.x, currentVelocity.x, smoothTime, deltaTime);
	result.y = SmoothDamp(current.y, target.y, currentVelocity.y, smoothTime, deltaTime);
	result.z = SmoothDamp(current.z, target.z, currentVelocity.z, smoothTime, deltaTime);

	return result;
}

void BubbleSortSinglePass(std::vector<cGameObject*>& transparentGOs)
{
	if (transparentGOs.size() != 0)
	{
		for (unsigned int index = 0; index != (transparentGOs.size() - 1); index++)
		{
			glm::vec3 ObjA = transparentGOs[index]->positionXYZ;
			glm::vec3 ObjB = transparentGOs[index + 1]->positionXYZ;

			if (glm::distance2(ObjA, camera.Position) < glm::distance2(ObjB, camera.Position))
			{
				// Out of order, so swap the positions...
				cGameObject* pTemp = transparentGOs[index];
				transparentGOs[index] = transparentGOs[index + 1];
				transparentGOs[index + 1] = pTemp;
			}
		}
	}
}

void DrawUpdate()
{
	for (int index = 0; index != ::g_vec_pGameObjects.size(); index++)
	{
		DrawObject(glm::mat4(1.0f), g_vec_pGameObjects[index], shaderProgID, pTheVAOManager);
	}

	//BubbleSortSinglePass(g_vec_pTransparentGameObjects);

	//for (unsigned int i = 0; i < g_vec_pTransparentGameObjects.size(); i++)
	//{
	//	DrawObject(glm::mat4(1.0f), g_vec_pTransparentGameObjects[i], shaderProgID, pTheVAOManager);
	//}

	//collisionParticles->getParticles(vecParticles);
	//for (std::vector<cParticle*>::iterator itPart = vecParticles.begin();
	//	itPart != vecParticles.end(); itPart++)
	//{
	//	pCube->positionXYZ = (*itPart)->location;
	//	pCube->scale = (*itPart)->scale;
	//	pCube->setOrientation((*itPart)->qOrientation);
	//	pCube->diffuseColour.a = (*itPart)->colourRGBA.a;
	//	DrawObject(glm::mat4(1.0f), pCube, shaderProgID, pTheVAOManager);
	//}

	//tailDustParticles->getParticles(vecParticles, camera.Position);
	//for (std::vector<cParticle*>::iterator itPart = vecParticles.begin();
	//	itPart != vecParticles.end(); itPart++)
	//{		
	//	pQuadImposter->positionXYZ = (*itPart)->location;
	//	pQuadImposter->scale = (*itPart)->scale;
	//	pQuadImposter->setOrientation((*itPart)->qOrientation);
	//	pQuadImposter->diffuseColour.a = (*itPart)->colourRGBA.a;
	//	DrawObject(glm::mat4(1.0f), pQuadImposter, shaderProgID, pTheVAOManager);
	//}
}

glm::quat RotationBetweenVectors(glm::vec3 start, glm::vec3 dest)
{
	start = glm::normalize(start);
	dest = glm::normalize(dest);

	float cosTheta = glm::dot(start, dest);
	glm::vec3 rotationAxis;

	if (cosTheta < -1 + 0.001f)
	{
		// special case when vectors in opposite directions:
		// there is no "ideal" rotation axis
		// So guess one; any will do as long as it's perpendicular to start
		rotationAxis = glm::cross(-worldFront, start);
		if (glm::length2(rotationAxis) < 0.01) // bad luck, they were parallel, try again!
			rotationAxis = glm::cross(worldRight, start);

		rotationAxis = normalize(rotationAxis);
		return glm::angleAxis(glm::radians(180.0f), rotationAxis);
	}

	rotationAxis = glm::cross(start, dest);

	float s = sqrt((1 + cosTheta) * 2);
	float invs = 1 / s;

	return glm::quat(s * 0.5f, rotationAxis.x * invs, rotationAxis.y * invs, rotationAxis.z * invs);
}

void CameraShake()
{
	const float frequency = 10.0f;

	if (camerShakeAmplitude <= 0.0f)
	{
		camerShakeAmplitude = 0.0f;
		camera.Position = glm::vec3(0.0f, 65.0f, 55.0f);
		return;
	}

	camera.Position.y += sin(glm::two_pi<float>() * glfwGetTime() * frequency) * camerShakeAmplitude;
	camera.Position.x += cos(glm::two_pi<float>() * glfwGetTime() * frequency) * camerShakeAmplitude;
	camera.Position.z += sin(glm::pi<float>() * glfwGetTime() * frequency) * camerShakeAmplitude;
	camerShakeAmplitude -= 0.01f;
}

void CamShake(glm::vec3& eyeOut)
{
	const float frequency = 10.0f;

	if (camerShakeAmplitude <= 0.0f)
	{
		camerShakeAmplitude = 0.0f;
		return;
	}

	eyeOut.y += sin(glm::two_pi<float>() * glfwGetTime() * frequency) * camerShakeAmplitude;
	eyeOut.x += cos(glm::two_pi<float>() * glfwGetTime() * frequency) * camerShakeAmplitude;
	eyeOut.z += sin(glm::pi<float>() * glfwGetTime() * frequency) * camerShakeAmplitude;
	camerShakeAmplitude -= 0.01f;
}

glm::vec3 QuadraticBezierCurve(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, float t /*0~1*/)
{
	glm::vec3 result;

	result.x = (1 - t) * (1 - t) * p1.x + 2 * (1 - t) * t * p2.x + t * t * p3.x;
	result.y = (1 - t) * (1 - t) * p1.y + 2 * (1 - t) * t * p2.y + t * t * p3.y;
	result.z = (1 - t) * (1 - t) * p1.z + 2 * (1 - t) * t * p2.z + t * t * p3.z;

	return result;
}

float easeInSine(float ratio)
{
	return 1.0f - cos(ratio * (glm::pi<float>() / 2.0f));
}

float easeOutSine(float ratio)
{
	return sin(ratio * (glm::pi<float>() / 2.0f));
}

float easeInOutSine(float ratio)
{
	ratio = ratio * 2.0f;
	return (ratio < 1.0f) ? 0.5f * easeInSine(ratio) : 0.5f * easeOutSine(ratio - 1.0f) + 0.5f;
}

float easeInQuadratic(float ratio)
{
	return ratio * ratio;
}

float easeOutQuadratic(float ratio)
{
	ratio = 1.0f - ratio;
	return 1.0f - ratio * ratio;
}

float easeInOutQuadratic(float ratio)
{
	ratio = ratio * 2.0f;
	return (ratio < 1.0f) ? 0.5f * easeInQuadratic(ratio) : 0.5f * easeOutQuadratic(ratio - 1.0f) + 0.5f;
}

float easeInQuartic(float ratio)
{
	return ratio * ratio * ratio * ratio;
}

float easeOutQuartic(float ratio)
{
	ratio = 1.0f - ratio;
	return 1.0f - ratio * ratio * ratio * ratio;
}

float easeInOutQuartic(float ratio)
{
	ratio = ratio * 2.0f;
	return (ratio < 1.0f) ? 0.5f * easeInQuartic(ratio) : 0.5f * easeOutQuartic(ratio - 1.0f) + 0.5f;
}

void CheckTrigger(cGameObject* triggerObject, cGameObject* checkObject)
{
	now = clock() / float(CLOCKS_PER_SEC);

	// Reset object colour
	if (now - triggerObject->timeStamp >= 0.5f)
	{
		triggerObject->diffuseColour = glm::vec4(0.3f, 0.6f, 0.3f, 1.0f);
		if (triggerObject->friendlyName == "spinTrigger")
		{
			checkObject->diffuseColour = glm::vec4(0.3f, 0.6f, 0.3f, 1.0f);
		}
	}

	if (!triggerObject->triggerActive)
	{
		if (triggerObject->physicsComponent->TriggeredBy(checkObject->physicsComponent->GetUniqueID()))
		{
			//printf("Ball in!\n");
			triggerObject->triggerActive = true;
			if (triggerObject->friendlyName == "deathTrigger")		// Flippers failed to catch the ball
			{
				checkObject->physicsComponent->SetPosition(glm::vec3(12.75f, 2.0f, 13.0f));
				lives--;
				printf("%d lives left.\n", lives);
				camerShakeAmplitude = 1.0f;
				if (lives == 0)
				{
					printf("Game Over! Total points: %d. Press R to restart.\n", gamePoints);					
					gameOver = true;
					gamePoints = 0;
					plunger->diffuseColour = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
					flipperRight->diffuseColour = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
					flipperLeft->diffuseColour = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
					return;
				}
			}
		}
	}
	else
	{
		if (!triggerObject->physicsComponent->TriggeredBy(checkObject->physicsComponent->GetUniqueID()))
		{
			//printf("Ball out!\n");
			triggerObject->triggerActive = false;			

			// Handle getting points
			if (triggerObject->friendlyName == "bumper1")
			{
				gamePoints += 10;	
				printf("10 points bumper! Current game points: %d.\n", gamePoints);
			}

			if (triggerObject->friendlyName == "bumper2")
			{
				gamePoints += 5;			
				printf("5 points bumper! Current game points: %d.\n", gamePoints);
			}

			if (triggerObject->friendlyName == "spinTrigger")
			{
				gamePoints += 1;
				printf("1 point spinner! Current game points: %d.\n", gamePoints);
				checkObject->diffuseColour = glm::vec4(0.6f, 0.3f, 0.3f, 1.0f);
			}
			
			triggerObject->timeStamp = clock() / float(CLOCKS_PER_SEC);
			triggerObject->diffuseColour = glm::vec4(0.6f, 0.3f, 0.3f, 1.0f);			
		}
	}
	
}
