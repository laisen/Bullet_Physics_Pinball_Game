#include "cOrientToRelative.h"
#include <glm/gtc/quaternion.hpp>

//#define worldUp glm::vec3(0.0f, 1.0f, 0.0f)
//#define worldRight glm::vec3(1.0f, 0.0f, 0.0f)
//#define worldFront glm::vec3(0.0f, 0.0f, 1.0f)
extern float easeInOutSine(float ratio);
extern float easeInOutQuadratic(float ratio);
extern float easeInOutQuartic(float ratio);
extern bool useFollowCamera;
extern glm::vec3 followCameraTarget;

void cOrientToRelative::Init(std::vector<sPair> vecDetails)
{
	glm::vec3 EulerAngle;
	EulerAngle.x = glm::radians(vecDetails[0].numData.x);
	EulerAngle.y = glm::radians(vecDetails[0].numData.y);
	EulerAngle.z = glm::radians(vecDetails[0].numData.z);
	this->m_relativeOrientation = glm::quat(EulerAngle);

	this->m_TimeToRotate = vecDetails[1].numData.x;
	this->m_SmoothLevel = vecDetails[2].numData.x;

	if (this->m_pTheGO->friendlyName == "camera")
	{
		useFollowCamera = true;
	}

	//glm::vec3 EulerAngleSpeed;
	//EulerAngleSpeed.x = glm::radians(vecDetails[0].numData.x) / this->m_TimeToRotate;
	//EulerAngleSpeed.y = glm::radians(vecDetails[0].numData.y) / this->m_TimeToRotate;
	//EulerAngleSpeed.z = glm::radians(vecDetails[0].numData.z) / this->m_TimeToRotate;
	//this->m_rotationalSpeed = glm::quat(EulerAngleSpeed);

	//this->m_rotationalSpeedEuler = glm::vec3(glm::radians(vecDetails[0].numData.x) / this->m_TimeToRotate,
	//	glm::radians(vecDetails[0].numData.y) / this->m_TimeToRotate,
	//	glm::radians(vecDetails[0].numData.z) / this->m_TimeToRotate);

	return;
}

void cOrientToRelative::Update(double deltaTime)
{
	if ( ! this->m_UpdateHasBeenCalled)
	{
		this->m_startOrentation = this->m_pTheGO->getQOrientation();
		this->m_endOrientation = this->m_relativeOrientation * this->m_startOrentation;
		//this->m_endOrientation = this->m_startOrentation * this->m_endOrientation;
		this->m_UpdateHasBeenCalled = true;
	}	
	  
	// get a number between 0.0 and 1.0f
	float AmountOfRotationCompleted = (float)this->m_TimeElapsed / this->m_TimeToRotate;
	if (this->m_SmoothLevel == 1)
	{
		AmountOfRotationCompleted = easeInOutSine(AmountOfRotationCompleted);
	}
	if (this->m_SmoothLevel == 2)
	{
		AmountOfRotationCompleted = easeInOutQuadratic(AmountOfRotationCompleted);
	}
	if (this->m_SmoothLevel == 3)
	{		
		AmountOfRotationCompleted = glm::smoothstep(0.0f, 1.0f, AmountOfRotationCompleted);
	}
	if (this->m_SmoothLevel == 4)
	{
		AmountOfRotationCompleted = easeInOutQuartic(AmountOfRotationCompleted);
	}

	glm::quat qCurrentRotation 
		= glm::slerp( this->m_startOrentation, this->m_endOrientation, 
					  AmountOfRotationCompleted );

	this->m_pTheGO->setOrientation(qCurrentRotation);
	//**********************************************************************
	//glm::vec3 deltaStep = this->m_rotationalSpeedEuler * (float)deltaTime;

	//glm::quat MagicQuaternionX = glm::angleAxis(deltaStep.x, worldRight);
	//glm::quat MagicQuaternionY = glm::angleAxis(deltaStep.y, worldUp);
	//glm::quat MagicQuaternionZ = glm::angleAxis(deltaStep.z, worldFront);
	//
	////this->m_pTheGO->updateOrientation( glm::quat(deltaStep) );
	//this->m_pTheGO->updateOrientation(MagicQuaternionX);
	//this->m_pTheGO->updateOrientation(MagicQuaternionY);
	//this->m_pTheGO->updateOrientation(MagicQuaternionZ);	

	this->m_TimeElapsed += deltaTime;

	if (this->m_pTheGO->friendlyName == "camera")
	{
		followCameraTarget = this->m_pTheGO->getUpdatedFront();		// Change LookAt view
	}

	return;
}

bool cOrientToRelative::IsDone(void)
{
	if (this->m_UpdateHasBeenCalled == false)
	{
		return false;
	}

	if (this->m_TimeElapsed >= this->m_TimeToRotate)
	{
		return true;
	}
	return false;
}

void cOrientToRelative::Reset(void)
{
	this->m_UpdateHasBeenCalled = false;
	this->m_TimeElapsed = 0.0;
}

void cOrientToRelative::SetGameObject(cGameObject* pGO)
{
	this->m_pTheGO = pGO;
	return;
}
