#include "cMoveTo.h"

extern bool useFollowCamera;
extern glm::vec3 followCameraTarget;

void cMoveTo::Init(std::vector<sPair> vecDetails)
{
	this->m_startPosition = vecDetails[0].numData;
	this->m_endPosition = vecDetails[1].numData;
	this->m_TimeToMove = vecDetails[2].numData.x;
	this->m_TimeToEaseIn = vecDetails[2].numData.y;
	this->m_TimeToEaseOut = vecDetails[2].numData.z;

	// Handle wrong timing input
	if (this->m_TimeToEaseIn + this->m_TimeToEaseOut > this->m_TimeToMove)
	{
		this->m_TimeToEaseIn = this->m_TimeToMove * (this->m_TimeToEaseIn / (this->m_TimeToEaseIn + this->m_TimeToEaseOut));
		this->m_TimeToEaseOut = this->m_TimeToMove * (this->m_TimeToEaseOut / (this->m_TimeToEaseIn + this->m_TimeToEaseOut));
	}
	
	this->m_totalDistance = glm::distance(this->m_startPosition, this->m_endPosition);
	
	float speedMax = (this->m_totalDistance * 2.0f)
		/ (this->m_TimeToMove * 2.0f - this->m_TimeToEaseIn - this->m_TimeToEaseOut);

	this->m_direction = glm::normalize(this->m_endPosition - this->m_startPosition);

	this->m_velocityMax = this->m_direction * speedMax;

	this->m_TimeElapsed = 0.0;

	if (this->m_pTheGO->friendlyName == "camera")
	{
		useFollowCamera = true;
	}

	return;
}

void cMoveTo::SetGameObject(cGameObject* pGO)
{
	this->m_pTheGO = pGO;
	return;
}

void cMoveTo::Update(double deltaTime)
{
	if (!this->m_UpdateHasBeenCalled)
	{
		this->m_pTheGO->positionXYZ = this->m_startPosition;
		this->m_UpdateHasBeenCalled = true;
	}

	glm::vec3 deltaStep;

	if ( (float)this->m_TimeElapsed < this->m_TimeToEaseIn )		// Ease in motion stage
	{
		float timeRatio = this->m_TimeElapsed / this->m_TimeToEaseIn;
		float easeInRatio = glm::smoothstep(0.0f, 1.0f, timeRatio);
		glm::vec3 velocityEaseIn = this->m_velocityMax * easeInRatio;
		deltaStep = velocityEaseIn * (float)deltaTime;
	}	
	else if ( (float)this->m_TimeElapsed > this->m_TimeToMove - this->m_TimeToEaseOut )		// Ease out motion stage
	{
		float timeRatio = (this->m_TimeToMove - this->m_TimeElapsed) / this->m_TimeToEaseOut;
		float easeOutRatio = glm::smoothstep(0.0f, 1.0f, timeRatio);
		glm::vec3 velocityEaseOut = this->m_velocityMax * easeOutRatio;
		deltaStep = velocityEaseOut * (float)deltaTime;
	}
	else		// Uniform motion stage
	{
		deltaStep = this->m_velocityMax * (float)deltaTime;
	}

	this->m_pTheGO->positionXYZ += deltaStep;

	this->m_TimeElapsed += deltaTime;

	if (this->m_pTheGO->friendlyName == "camera")
	{
		followCameraTarget = this->m_endPosition;		// Change LookAt view
	}

	return;
}

bool cMoveTo::IsDone(void)
{
	if (this->m_UpdateHasBeenCalled == false)
	{
		return false;
	}

	//float distance = glm::distance( this->m_pTheGO->positionXYZ, 
	//							    this->m_startPosition );

	//if ( distance >= this->m_totalDistance )
	//{
	//	return true;
	//}

	if (this->m_TimeElapsed >= this->m_TimeToMove)
	{
		return true;
	}

	return false;
}

void cMoveTo::Reset(void)
{
	this->m_TimeElapsed = 0.0;
	this->m_UpdateHasBeenCalled = false;
}