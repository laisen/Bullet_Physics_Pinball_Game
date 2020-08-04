#include "cFollowObject.h"

extern bool useFollowCamera;
extern glm::vec3 followCameraTarget;

void cFollowObject::Init(std::vector<sPair> vecDetails)
{	
	this->m_offsetFromTargetXYZ = vecDetails[0].numData;
	this->m_followDistance = vecDetails[1].numData.x;
	this->m_slowdownDistance = vecDetails[2].numData.x;
	this->m_maxFollowSpeed = vecDetails[3].numData.x;
	this->m_slowdownRegion = this->m_slowdownDistance - this->m_followDistance;
	
	if (this->m_pTheGO->friendlyName == "camera")
	{
		useFollowCamera = true;
	}

	return;
}

void cFollowObject::Update(double deltaTime)
{
	if (!this->m_UpdateHasBeenCalled)
	{
		this->m_originalStartPosition = this->m_pTheGO->positionXYZ;		
		this->m_UpdateHasBeenCalled = true;
	}

	glm::vec3 finalPositionXYZ = this->m_pTargetGO->positionXYZ + this->m_offsetFromTargetXYZ;

	glm::vec3 vecDirection = glm::normalize(finalPositionXYZ - this->m_pTheGO->positionXYZ);

	float distanceToTarget = glm::distance(this->m_pTheGO->positionXYZ, finalPositionXYZ);

	glm::vec3 velocityXYZ;

	if (distanceToTarget > this->m_slowdownDistance)
	{
		// Full speed in the direction I need to go
		velocityXYZ = vecDirection * this->m_maxFollowSpeed;
	}
	else if (distanceToTarget < this->m_followDistance)
	{
		// Velocity is zero 
		velocityXYZ = glm::vec3(0.0f, 0.0f, 0.0f);
	}
	else
	{	
		float toFollowDistance = distanceToTarget - this->m_followDistance;
		
		float speedRatio = toFollowDistance / this->m_slowdownRegion;
		
		float easeSpeedRatio = glm::smoothstep(0.0f, 1.0f, speedRatio);

		velocityXYZ = vecDirection * this->m_maxFollowSpeed * easeSpeedRatio;
	}

	glm::vec3 deltaStep = velocityXYZ * (float)deltaTime;

	this->m_pTheGO->positionXYZ += deltaStep;

	if (this->m_pTheGO->friendlyName == "camera")
	{
		followCameraTarget = this->m_pTargetGO->positionXYZ;		// Change LookAt view
	}

	return;
}

bool cFollowObject::IsDone(void)
{
	// Take too long to approach ending place due to tiny velocity
	// So don't put it in sequence before other commands
	glm::vec3 finalPositionXYZ = this->m_pTargetGO->positionXYZ + this->m_offsetFromTargetXYZ;
	if (glm::distance(this->m_pTheGO->positionXYZ, finalPositionXYZ) - 0.5f /*threshold*/<= this->m_followDistance)
	{		
		return true;
	}
	return false;
}

void cFollowObject::Reset(void)
{
	// It seems don't want reset FollowObject command, just call update without checking done
	this->m_pTheGO->positionXYZ = this->m_originalStartPosition;
	this->m_UpdateHasBeenCalled = false;
}

void cFollowObject::SetGameObject(cGameObject* pGO)
{
	this->m_pTheGO = pGO;
	return;
}

void cFollowObject::SetTarget(cGameObject* pTargetGO)
{
	this->m_pTargetGO = pTargetGO;
	return;
}
