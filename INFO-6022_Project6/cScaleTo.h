#pragma once
#include "iCommand.h"

class cScaleTo : public iCommand
{
public:
	cScaleTo()
	{
		this->m_UpdateHasBeeCalled = false;
	}
	// Scales the object over time
	// Pass: 
	// - Ending Scale
	// - Number of seconds this will take
	// Initial scale is set on the 1st Update()
	virtual void Init(std::vector<sPair> vecDetails);
	virtual void SetGameObject(cGameObject* pGO);
	virtual void SetTarget(cGameObject* pTargetGO) { return; }
	virtual void Update(double deltaTime);
	virtual bool IsDone(void);
	virtual void Reset(void) { return; }
	virtual void setName(std::string name) { this->m_Name = name; }
	virtual std::string getName(void) { return this->m_Name; }
	virtual void SetLoopTimes(int times) { return; }	
	virtual void AddCommandSerial(iCommand* pCommand) { return; }
	virtual void AddCommandParallel(iCommand* pCommand) { return; }

private:

	// Flag to see if update has been called once
	bool m_UpdateHasBeeCalled;

	std::string m_Name;

	cGameObject* m_pTheGO;

	float m_startScale;
	float m_endScale;
	float m_TimeToChange;
	float m_ChangeSpeed;

	double m_ElapsedTime;

};