#include "cScaleToRelative.h"

void cScaleToRelative::Init(std::vector<sPair> vecDetails)
{
	// Scales the object over time
	// Pass: 
	// - Ending Scale
	// - Number of seconds this will take
	// Initial scale is set on the 1st Update()

	this->m_endScale = vecDetails[0].numData.x;
	this->m_TimeToChange = vecDetails[1].numData.x;

	return;
}

void cScaleToRelative::SetGameObject(cGameObject* pGO)
{
	this->m_pTheGO = pGO;
	return;
}

void cScaleToRelative::Update(double deltaTime)
{
	if ( ! this->m_UpdateHasBeenCalled )
	{
		this->m_startScale = this->m_pTheGO->scale;
		this->m_ChangeSpeed = ( this->m_endScale - this->m_startScale ) / this->m_TimeToChange;
		this->m_UpdateHasBeenCalled = true;
	}

	this->m_pTheGO->scale += (this->m_ChangeSpeed * (float)deltaTime);

	this->m_ElapsedTime += deltaTime;

	return;
}

bool cScaleToRelative::IsDone(void)
{
	// See if we've reached the scale we want
	// (Note: you might want to compare this with a multiple of FLT_EPSION)
	//if ( fabs(this->m_endScale - this->m_pTheGO->scale) < FLT_EPSILON * 10.0f )
	//if ( fabs(this->m_endScale - this->m_pTheGO->scale) < 0.1f )
	//{
	//	return true;
	//}

	if ( this->m_ElapsedTime >= (double)this->m_TimeToChange ) 
	{
		return true;
	}

	return false;
}