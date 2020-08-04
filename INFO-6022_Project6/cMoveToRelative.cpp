#include "cMoveToRelative.h"

extern bool useFollowCamera;
extern glm::vec3 followCameraTarget;

void cMoveToRelative::Init(std::vector<sPair> vecDetails)
{
	this->m_relativePosition =  vecDetails[0].numData;
	this->m_TimeToMove = vecDetails[1].numData.x;
	this->m_TimeToEaseIn = vecDetails[1].numData.y;
	this->m_TimeToEaseOut = vecDetails[1].numData.z;
	// Handle wrong timing input
	if (this->m_TimeToEaseIn + this->m_TimeToEaseOut > this->m_TimeToMove)
	{
		this->m_TimeToEaseIn = this->m_TimeToMove * (this->m_TimeToEaseIn / (this->m_TimeToEaseIn + this->m_TimeToEaseOut));
		this->m_TimeToEaseOut = this->m_TimeToMove * (this->m_TimeToEaseOut / (this->m_TimeToEaseIn + this->m_TimeToEaseOut));
	}

	this->m_TimeElapsed = 0.0;
	// The rest of the information is calculated on the 1st update.

	if (this->m_pTheGO->friendlyName == "camera")
	{
		useFollowCamera = true;
	}

	return;
}

void cMoveToRelative::SetGameObject(cGameObject* pGO)
{
	this->m_pTheGO = pGO;
	return;
}

void cMoveToRelative::Update(double deltaTime)
{
	// 1st time Update has been called? 
	if ( ! this->m_UpdateHasBeenCalled)
	{
		this->m_startPosition = this->m_pTheGO->positionXYZ;
		this->m_endPosition = this->m_startPosition + this->m_relativePosition;

		this->m_totalDistance = glm::distance(this->m_startPosition, this->m_endPosition);

		float speedMax = (this->m_totalDistance * 2.0f)
			/ (this->m_TimeToMove * 2.0f - this->m_TimeToEaseIn - this->m_TimeToEaseOut);

		this->m_direction = glm::normalize(this->m_endPosition - this->m_startPosition);

		this->m_velocityMax = this->m_direction * speedMax;

		this->m_UpdateHasBeenCalled = true;
	}

	glm::vec3 deltaStep;

	if ((float)this->m_TimeElapsed < this->m_TimeToEaseIn)		// Ease in motion stage
	{
		float timeRatio = this->m_TimeElapsed / this->m_TimeToEaseIn;
		float easeInRatio = glm::smoothstep(0.0f, 1.0f, timeRatio);
		glm::vec3 velocityEaseIn = this->m_velocityMax * easeInRatio;
		deltaStep = velocityEaseIn * (float)deltaTime;
	}
	else if ((float)this->m_TimeElapsed > this->m_TimeToMove - this->m_TimeToEaseOut)		// Ease out motion stage
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

bool cMoveToRelative::IsDone(void)
{
	// Info is set up on the 1st update, so we need to call Update
	if (this->m_UpdateHasBeenCalled == false)
	{
		return false;
	}

	//float distance = glm::distance(this->m_pTheGO->positionXYZ,
	//							   this->m_startPosition);

	//if (distance >= this->m_totalDistance)
	//{
	//	return true;
	//}

	if (this->m_TimeElapsed >= this->m_TimeToMove)
	{
		return true;
	}

	return false;
}

void cMoveToRelative::Reset(void)
{
	this->m_TimeElapsed = 0.0;
	this->m_UpdateHasBeenCalled = false;
}