#include "globalUtil.h"
#include "DrawObjectUtilLoop.h"
#include "cLowPassFilter.h"
#include "DebugRenderer/cDebugRenderer.h"
#include <time.h>		/* time */
#include "c3rdPersonCamera.h"


int main(void)
{	

	cWindow* myWindow = new cWindow();
	myWindow->createWindow();
	// center mouse position
	lastX = myWindow->windowWidth / 2.0f, lastY = myWindow->windowHeight / 2.0f;
	
	c3rdPersonCamera Cam;
	Cam.OnWindowResize(myWindow->windowWidth, myWindow->windowHeight);	

	// Set up new physics world & factory
	InitPhysics();

	// Load all external files
	LoadConfig();

	//cDebugRenderer* pDebugRenderer = new cDebugRenderer();
	//pDebugRenderer->initialize();	

	glUseProgram(shaderProgID);

	GLint eyeLocation_UL = glGetUniformLocation(shaderProgID, "eyeLocation");	
	GLint matView_UL = glGetUniformLocation(shaderProgID, "matView");
	GLint matProj_UL = glGetUniformLocation(shaderProgID, "matProj");

	cLowPassFilter avgDeltaTimeThingy;		
	
	ball = pFindObjectByFriendlyName("ball");
	cGameObject* deathTrigger = pFindObjectByFriendlyName("deathTrigger");
	cGameObject* bumper1Trigger = pFindObjectByFriendlyName("bumper1");
	cGameObject* bumper2Trigger = pFindObjectByFriendlyName("bumper2");
	cGameObject* spinTrigger = pFindObjectByFriendlyName("spinTrigger");
	cGameObject* spinner = pFindObjectByFriendlyName("spinner1");
	flipperRight = pFindObjectByFriendlyName("flipperRight");
	flipperLeft = pFindObjectByFriendlyName("flipperLeft");
	plunger = pFindObjectByFriendlyName("plunger");
	
	printf("Press down arrow to launch the ball and control keys for both flippers, you have three lives.\n");

	// Main loop start
	while (!glfwWindowShouldClose(myWindow->window))
	{

		float ratio;
		int width, height;
		glfwGetFramebufferSize(myWindow->window, &width, &height);
		ratio = width / (float)height;

		//Cam.OnWindowResize(width, height);

		//glm::mat4 projection = glm::perspective(0.5f, ratio, 0.1f, 2000.0f);

		// camera/view transformation
		//view = camera.GetViewMatrix();

		Cam.OnWindowResize(width, height);
		glm::mat4 projection;
		Cam.GetProjectionMatrix(projection);
		Cam.GetViewMatrix(view);
		//glm::mat4 targetTransform;
		//controlTargets[controlIndex]->physicsComponent->GetTransform(targetTransform);
		glm::vec3 targetTransform(0.0f, 0.0f, 5.0f);
		Cam.SetTargetTransform(targetTransform);
		Cam.Update(deltaTime);

		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		const double SOME_HUGE_TIME = 0.1;	// 100 ms;
		if (deltaTime > SOME_HUGE_TIME)
		{
			deltaTime = SOME_HUGE_TIME;
		}

		avgDeltaTimeThingy.addValue(deltaTime);

		processInput(myWindow->window);		

		glUseProgram(shaderProgID);

		glViewport(0, 0, width, height);
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (useFollowCamera)
		{			
			//camera.Position = SmoothFollow(camera.Position,
			//	glm::vec3(controlTargets[controlIndex]->positionXYZ.x,
			//		40.0f * controlTargets[controlIndex]->SPHERE_radius,
			//		controlTargets[controlIndex]->positionXYZ.z + 40.0f * controlTargets[controlIndex]->SPHERE_radius),
			//	cameraVelocity,
			//	1.0f,
			//	averageDeltaTime);
		}
		
		//CameraShake();
		glm::vec3 eye;
		Cam.GetEyePosition(eye);
		glUniform4f(eyeLocation_UL, eye.x, eye.y, eye.z, 1.0f);
		//glUniform4f(eyeLocation_UL, camera.Position.x, camera.Position.y, camera.Position.z, 1.0f);	

		glUniformMatrix4fv(matView_UL, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(matProj_UL, 1, GL_FALSE, glm::value_ptr(projection));
				
		
		//now = clock() / float(CLOCKS_PER_SEC);
		//if (now - lastTime >= 0.5f)
		//{			
		//	//collisionParticles->disableNewParticles();
		//	//camerShakeCooldown = true;
		//	lastTime = now;
		//}		
		
		averageDeltaTime = avgDeltaTimeThingy.getAverage();

		gPhysicsWorld->Update((float)averageDeltaTime);		

		// Trigger zones section
		CheckTrigger(deathTrigger, ball);
		CheckTrigger(bumper1Trigger, ball);
		CheckTrigger(bumper2Trigger, ball);
		CheckTrigger(spinTrigger, spinner);

		DrawUpdate();	

		//if (debugMode)
		//{
		//	//pDebugSphere->isVisible = true;
		//	for (int index = 0; index != ::g_vec_pDebugGameObjects.size(); index++)
		//	{
		//		DrawObject(glm::mat4(1.0f), g_vec_pDebugGameObjects[index], shaderProgID, pTheVAOManager);
		//	}
		//	pDebugRenderer->RenderDebugObjects(view, projection, 0.01f);			
		//}
		//else
		//{
		//	//pDebugSphere->isVisible = false;			
		//}		
				

		glfwSwapBuffers(myWindow->window);
		
		glfwPollEvents();
	}

	DestroyPhysics();
	glfwDestroyWindow(myWindow->window);
	glfwTerminate();
	for (size_t i = 0; i != g_vec_pGameObjects.size(); i++)
	{
		delete g_vec_pGameObjects[i];
	}	
	for (size_t i = 0; i != g_vec_pTransparentGameObjects.size(); i++)
	{
		delete g_vec_pTransparentGameObjects[i];
	}
	for (size_t i = 0; i != g_vec_pDebugGameObjects.size(); i++)
	{
		delete g_vec_pDebugGameObjects[i];
	}
	std::cout << g_vec_pGameObjects.size() + g_vec_pTransparentGameObjects.size() + g_vec_pDebugGameObjects.size()
		<< " GameObject(s) in total." << std::endl;

	delete pTheVAOManager;	
	delete myWindow;
	exit(EXIT_SUCCESS);
}