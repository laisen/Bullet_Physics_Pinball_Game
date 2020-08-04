#include "cFollowCurve.h"

extern glm::vec3 QuadraticBezierCurve(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, float t /*0~1*/);
extern float easeInOutSine(float ratio);
extern float easeInOutQuadratic(float ratio);
extern float easeInOutQuartic(float ratio);
extern bool useFollowCamera;
extern glm::vec3 followCameraTarget;

void cFollowCurve::Init(std::vector<sPair> vecDetails)
{	
	this->m_startPosition = vecDetails[0].numData;
	this->m_intermediatePoint = vecDetails[1].numData;
	this->m_endPosition = vecDetails[2].numData;
	this->m_TimeToFollow = vecDetails[3].numData.x;
	this->m_SmoothLevel = vecDetails[4].numData.x;

	if (this->m_pTheGO->friendlyName == "camera")
	{
		useFollowCamera = true;
	}

	return;
}

void cFollowCurve::Update(double deltaTime)
{
	if (!this->m_UpdateHasBeenCalled)
	{
		this->m_pTheGO->positionXYZ = this->m_startPosition;
		this->m_UpdateHasBeenCalled = true;
	}

	float AmountOfFollowCompleted = (float)this->m_TimeElapsed / this->m_TimeToFollow;

	if (this->m_SmoothLevel == 1)
	{
		AmountOfFollowCompleted = easeInOutSine(AmountOfFollowCompleted);
	}
	if (this->m_SmoothLevel == 2)
	{
		AmountOfFollowCompleted = easeInOutQuadratic(AmountOfFollowCompleted);
	}
	if (this->m_SmoothLevel == 3)
	{
		AmountOfFollowCompleted = glm::smoothstep(0.0f, 1.0f, AmountOfFollowCompleted);
	}
	if (this->m_SmoothLevel == 4)
	{
		AmountOfFollowCompleted = easeInOutQuartic(AmountOfFollowCompleted);
	}

	this->m_pTheGO->positionXYZ = QuadraticBezierCurve(this->m_startPosition,
		this->m_intermediatePoint,
		this->m_endPosition,
		AmountOfFollowCompleted);

	this->m_TimeElapsed += deltaTime;

	if (this->m_pTheGO->friendlyName == "camera")
	{
		followCameraTarget = this->m_pTheGO->getUpdatedFront();		// Change LookAt view
	}

	return;
}

bool cFollowCurve::IsDone(void)
{
	if (this->m_UpdateHasBeenCalled == false)
	{
		return false;
	}

	if (this->m_TimeElapsed >= this->m_TimeToFollow)
	{
		//this->m_pTheGO->positionXYZ;
		return true;
	}
	return false;
}

void cFollowCurve::Reset(void)
{
	this->m_TimeElapsed = 0.0;
	this->m_UpdateHasBeenCalled = false;
}

void cFollowCurve::SetGameObject(cGameObject* pGO)
{
	this->m_pTheGO = pGO;
	return;
}
