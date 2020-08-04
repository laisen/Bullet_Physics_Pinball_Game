#include "GFLW_callbacks.h"
#include <stdio.h>
#include <iostream>
#include "cGameObject.h"
#include "cParticleEmitter.h"
#include "cCamera.h"
#include <iPhysicsFactory.h>

cGameObject* pFindObjectByFriendlyName(std::string name);
extern cGameObject* plunger;
extern cGameObject* flipperRight;
extern cGameObject* flipperLeft;
extern double averageDeltaTime;
extern float camerShakeAmplitude;
extern bool debugMode;
extern bool wireframeMode;
extern bool normalPassMode;
extern bool albedoPassMode;
extern bool useFollowCamera;
extern bool g_MouseLeftButtonIsDown;
extern bool gameOver;
extern int lives;
extern double g_scrollY;
extern float g_FOV;
//extern cCamera camera;
bool isOnlyAltKeyDown(int mods);

extern std::vector<cGameObject*> controlTargets;
extern unsigned int controlIndex;

void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);

	if (key == GLFW_KEY_TAB && action == GLFW_PRESS)
	{
		controlTargets[controlIndex]->diffuseColour = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		controlIndex = (controlIndex >= controlTargets.size() - 1) ? 0 : (controlIndex + 1);
		controlTargets[controlIndex]->diffuseColour = glm::vec4(0.8f, 0.8f, 0.2f, 1.0f);
	}
	
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
	{		
		float mass;
		controlTargets[controlIndex]->physicsComponent->GetMass(mass);
		controlTargets[controlIndex]->physicsComponent->ApplyForce(glm::vec3(0.0f, 1000.0f * mass, 0.0f));
	}

	if (key == GLFW_KEY_R && action == GLFW_PRESS)
	{
		if (gameOver == true)
		{
			gameOver = false;
			lives = 3;
			plunger->diffuseColour = glm::vec4(0.6f, 0.6f, 0.0f, 1.0f);
			flipperRight->diffuseColour = glm::vec4(0.6f, 0.6f, 0.0f, 1.0f);
			flipperLeft->diffuseColour = glm::vec4(0.6f, 0.6f, 0.0f, 1.0f);
		}
	}
	

	//cGameObject* controlGameObject = pFindObjectByFriendlyName("spaceman");	

	//if (key == GLFW_KEY_R && action == GLFW_PRESS)
	//{
	//	controlGameObject->setOrientation(glm::vec3(90.0f, 180.0f, 0.0f));
	//	controlGameObject->positionXYZ = glm::vec3(0.0f, 100.0f, 250.0f);
	//}
	//	

	//if ( (key == GLFW_KEY_UP && action == GLFW_PRESS) || (key == GLFW_KEY_DOWN && action == GLFW_PRESS) )
	//{
	//	UP_DOWN_PRESS = true;
	//	//printf("UP_DOWN_PRESS = true\n");
	//}
	//if ( (key == GLFW_KEY_UP && action == GLFW_RELEASE) || (key == GLFW_KEY_DOWN && action == GLFW_RELEASE) )
	//{
	//	UP_DOWN_PRESS = false;
	//	//printf("UP_DOWN_PRESS = false\n");
	//	tailDustParticles->disableNewParticles();
	//}
	//if ((key == GLFW_KEY_LEFT && action == GLFW_PRESS) || (key == GLFW_KEY_RIGHT && action == GLFW_PRESS))
	//{
	//	LEFT_RIGHT_PRESS = true;
	//	//printf("LEFT_RIGHT_PRESS = true\n");
	//}

	//if ((key == GLFW_KEY_Z && action == GLFW_PRESS) || (key == GLFW_KEY_X && action == GLFW_PRESS))
	//{
	//	Z_X_PRESS = true;
	//	//printf("Z_X_PRESS = true\n");
	//}

	//if (isOnlyAltKeyDown(mods))
	//{		

	//}
	
	//if  ((key == GLFW_KEY_LEFT && action == GLFW_RELEASE) || (key == GLFW_KEY_RIGHT && action == GLFW_RELEASE) )
	//{
	//	LEFT_RIGHT_PRESS = false;
	//	//printf("LEFT_RIGHT_PRESS = false\n");
	//	tailDustParticles->disableNewParticles();
	//}	
	//if ((key == GLFW_KEY_Z && action == GLFW_RELEASE) || (key == GLFW_KEY_X && action == GLFW_RELEASE))
	//{
	//	Z_X_PRESS = false;
	//	//printf("Z_X_PRESS = false\n");
	//	tailDustParticles->disableNewParticles();
	//}	

	//if (key == GLFW_KEY_1 && action == GLFW_PRESS)
	//{
	//	dragOn = !dragOn;
	//	dragOn ? printf("Resistant drag force enabled.\n") : printf("Resistant drag force disabled.\n");
	//}
	if (key == GLFW_KEY_2 && action == GLFW_PRESS)
	{
		debugMode = !debugMode;
		debugMode ? printf("Debug mode enabled.\n") : printf("Debug mode disabled.\n");
	}
	if (key == GLFW_KEY_3 && action == GLFW_PRESS)
	{
		wireframeMode = !wireframeMode;
		wireframeMode ? printf("Wireframe mode enabled.\n") : printf("Wireframe mode disabled.\n");
		normalPassMode = false;
		albedoPassMode = false;
	}
	if (key == GLFW_KEY_4 && action == GLFW_PRESS)
	{
		normalPassMode = !normalPassMode;
		normalPassMode ? printf("Normal pass render mode enabled.\n") : printf("Normal pass render mode disabled.\n");
		wireframeMode = false;
		albedoPassMode = false;
	}
	if (key == GLFW_KEY_5 && action == GLFW_PRESS)
	{
		albedoPassMode = !albedoPassMode;
		albedoPassMode ? printf("Albedo pass render mode enabled.\n") : printf("Albedo pass render mode disabled.\n");
		wireframeMode = false;
		normalPassMode = false;
	}
	//if (key == GLFW_KEY_6 && action == GLFW_PRESS)
	//{
	//	droneMode = !droneMode;
	//	droneMode ? printf("Drone mode enabled.\n") : printf("Drone mode disabled. Back to free rotation mode.\n");
	//}

	//if (key == GLFW_KEY_F && action == GLFW_PRESS)
	//{
	//	useFollowCamera = !useFollowCamera;
	//	useFollowCamera ? printf("Follow camera enabled.\n") : printf("Follow camera disabled.\n");
	//	if (!useFollowCamera) // Use fly camera
	//	{
	//		camera.Position = glm::vec3(controlGameObject->positionXYZ.x,
	//			controlGameObject->positionXYZ.y + 50.0f,
	//			controlGameObject->positionXYZ.z + 50.0f);
	//		camera.Front = glm::normalize(controlGameObject->positionXYZ - camera.Position);
	//		camera.Right = glm::normalize(glm::cross(camera.Front, glm::vec3(0.0f, 1.0f, 0.0f)));
	//		camera.Up = glm::normalize(glm::cross(camera.Right, camera.Front));
	//		camera.Yaw = 0.0f;
	//		camera.Pitch = -45.0f;
	//		//view = 
	//	}
	//	else // Use Follow camera
	//	{
	//		camera.Front = glm::normalize(controlGameObject->positionXYZ - camera.Position);
	//		camera.Right = glm::normalize(glm::cross(camera.Front, glm::vec3(0.0f, 1.0f, 0.0f)));
	//		camera.Up = glm::normalize(glm::cross(camera.Right, camera.Front));
	//		camera.Yaw = 0.0f;
	//		camera.Pitch = -45.0f;
	//	}
	//}
	
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		g_MouseLeftButtonIsDown = true;
	}
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
	{
		g_MouseLeftButtonIsDown = false;
	}
	//std::cout << "g_MouseLeftButtonIsDown: " << g_MouseLeftButtonIsDown << std::endl;
	return;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	// A regular mouse wheel returns the y value	
	//g_scrollY = yoffset;
	//std::cout << "Mouse wheel: " << yoffset << std::endl;

	if (g_FOV >= 5.0f && g_FOV <= 60.0f)
		g_FOV -= yoffset;
	else if (g_FOV < 5.0f)
		g_FOV = 5.0f;
	else if (g_FOV > 60.0f)
		g_FOV = 60.0f;

	//std::cout << "g_FOV: " << g_FOV << std::endl;
	return;
}

bool isOnlyAltKeyDown(int mods)
{
	if (mods == GLFW_MOD_ALT)
	{
		return true;
	}
	return false;
}