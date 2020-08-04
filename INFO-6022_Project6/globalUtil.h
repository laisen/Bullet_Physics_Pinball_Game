#pragma once
#include <iostream>	
#include <fstream>
#include "GLCommon.h"
#include "GFLW_callbacks.h"
#include "cWindow.h"
#include "cCamera.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>		// glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp>		// glm::value_ptr
#include "cGameObject.h"
#include <vector>
#include <map>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/constants.hpp>
#include <sstream>
#include "cModelLoader.h"
#include "cVAOManager.h"
#include "cGameObject.h"
#include "cShaderManager.h"
#include "cBasicTextureManager.h"
#include "cParticleEmitter.h"
#include "cFBO.h"


#define BULLET_PHYSISCS
////////////////////////////////////////////

//#ifdef MY_PHYSISCS
//#include <cPhysicsFactory.h>
//#pragma comment(lib, "PhysicsWrapper.lib")
//#pragma comment(lib, "PhysicsLibrary.lib")
//typedef nPhysics::cPhysicsFactory MyPhysicsFactoryClass;
//#endif // MY_PHYSISCS

#ifdef BULLET_PHYSISCS
#include <Bullet/cPhysicsFactory.h>
#pragma comment(lib, "BulletPhysicsWrapper.lib")
typedef nPhysics::cPhysicsFactory MyPhysicsFactoryClass;
#endif // BULLET_PHYSISCS

extern nPhysics::iPhysicsFactory* gPhysicsFactory;
extern nPhysics::iPhysicsWorld* gPhysicsWorld;
extern std::vector<cGameObject*> controlTargets;
extern unsigned int controlIndex;
extern cGameObject* flipperRight;
extern cGameObject* flipperLeft;
extern cGameObject* plunger;
extern cGameObject* ball;

void InitPhysics();
void DestroyPhysics();


#define worldUp glm::vec3(0.0f, 1.0f, 0.0f)
#define worldRight glm::vec3(1.0f, 0.0f, 0.0f)
#define worldFront glm::vec3(0.0f, 0.0f, 1.0f)

extern std::map<std::string, cMesh> mapNameToMesh;
extern std::vector<sModelDrawInfo> g_vec_sModelDrawInfo;
extern GLuint shaderProgID;
extern cVAOManager* pTheVAOManager;
extern cBasicTextureManager* g_pTextureManager;

extern bool firstMouse;
extern float lastX, lastY;
extern bool g_MouseLeftButtonIsDown;
extern double g_mouseX, g_mouseY, g_scrollY;
extern float g_FOV;

extern cCamera camera;
extern glm::vec3 followCameraTarget;
extern glm::mat4 view;
extern glm::vec3 cameraVelocity;
extern float camerShakeAmplitude;
extern bool camerShakeCooldown;

extern float deltaTime;	// Time between current frame and last frame
extern float lastFrame; // Time of last frame
extern float lastTime;
extern float now;

extern glm::vec3 movePosition;


extern std::vector<cGameObject*> g_vec_pGameObjects;
extern std::map<std::string /*FriendlyName*/, cGameObject*> g_map_GameObjectsByFriendlyName;
extern std::vector<cGameObject*> g_vec_pDebugGameObjects;
extern std::vector<cGameObject*> g_vec_pTransparentGameObjects;
//extern std::vector<cParticle*> vecParticles;

extern double averageDeltaTime;

extern bool debugMode;
extern bool wireframeMode;
extern bool normalPassMode;
extern bool albedoPassMode;
extern bool useFollowCamera;


void processInput(GLFWwindow* window);

void mouse_callback(GLFWwindow* window, double xpos, double ypos);

cGameObject* pFindObjectByFriendlyName(std::string name);

cGameObject* pFindObjectByFriendlyNameMap(std::string name);

bool LoadConfig();

glm::vec4 RBGcircleUpdate(glm::vec4& colour);

//static glm::vec4 breathingColourBrightness(glm::vec4& baseColour)
//{
//	const float frequency = 0.1f;
//	glm::vec4 breathingColour;
//
//	breathingColour.r = baseColour.r * 0.5 * (1.0f + sin(glm::two_pi<float>() * glfwGetTime() * frequency));
//	breathingColour.g = baseColour.g * 0.5 * (1.0f + sin(glm::two_pi<float>() * glfwGetTime() * frequency));
//	breathingColour.b = baseColour.b * 0.5 * (1.0f + sin(glm::two_pi<float>() * glfwGetTime() * frequency));
//
//	return breathingColour;
//}

float lerp(float a, float b, float f);

float SmoothDamp(float current, float target, float& currentSpeed, float smoothTime, float deltaTime);

glm::vec3 SmoothFollow(glm::vec3 current, glm::vec3 target, glm::vec3& currentVelocity, float smoothTime, float deltaTime);

void BubbleSortSinglePass(std::vector<cGameObject*>& transparentGOs);

void DrawObject(glm::mat4 worldMatrix, cGameObject* pCurrentObject, GLint shaderProgID, cVAOManager* pVAOManager);

void DrawUpdate();

glm::quat RotationBetweenVectors(glm::vec3 start, glm::vec3 dest);

void CameraShake();

glm::vec3 QuadraticBezierCurve(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, float t /*0~1*/);

float easeInSine(float ratio);

float easeOutSine(float ratio);

float easeInOutSine(float ratio);

float easeInQuadratic(float ratio);

float easeOutQuadratic(float ratio);

float easeInOutQuadratic(float ratio);

float easeInQuartic(float ratio);

float easeOutQuartic(float ratio);

float easeInOutQuartic(float ratio);

void CheckTrigger(cGameObject* triggerObject, cGameObject* checkObject);
