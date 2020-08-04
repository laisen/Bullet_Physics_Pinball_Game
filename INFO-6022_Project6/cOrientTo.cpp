#include "cOrientTo.h"
#include <glm/gtc/quaternion.hpp>

extern float easeInOutSine(float ratio);
extern float easeInOutQuadratic(float ratio);
extern float easeInOutQuartic(float ratio);
extern bool useFollowCamera;
extern glm::vec3 followCameraTarget;

void cOrientTo::Init(std::vector<sPair> vecDetails)
{	
	glm::vec3 EulerAngle;
	EulerAngle.x = glm::radians(vecDetails[0].numData.x);
	EulerAngle.y = glm::radians(vecDetails[0].numData.y);
	EulerAngle.z = glm::radians(vecDetails[0].numData.z);
	this->m_endOrientation = glm::quat(EulerAngle);

	this->m_TimeToRotate = vecDetails[1].numData.x;
	this->m_SmoothLevel = vecDetails[2].numData.x;
	
	if (this->m_pTheGO->friendlyName == "camera")
	{
		useFollowCamera = true;
	}

	return;
}

void cOrientTo::Update(double deltaTime)
{
	if (!this->m_UpdateHasBeenCalled)
	{
		this->m_startOrentation = this->m_pTheGO->getQOrientation();
		//this->m_endOrientation = this->m_endOrientation * this->m_startOrentation;		

		this->m_UpdateHasBeenCalled = true;
	}	

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
		= glm::slerp(this->m_startOrentation, this->m_endOrientation,
			AmountOfRotationCompleted);

	this->m_pTheGO->setOrientation(qCurrentRotation);	

	this->m_TimeElapsed += deltaTime;

	if (this->m_pTheGO->friendlyName == "camera")
	{
		followCameraTarget = this->m_pTheGO->getUpdatedFront();		// Change LookAt view
	}

	return;
}

bool cOrientTo::IsDone(void)
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

void cOrientTo::Reset(void)
{
	this->m_TimeElapsed = 0.0;
	this->m_UpdateHasBeenCalled = false;
}

void cOrientTo::SetGameObject(cGameObject* pGO)
{
	this->m_pTheGO = pGO;
	return;
}
