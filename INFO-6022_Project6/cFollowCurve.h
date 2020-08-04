#pragma once

#include "iCommand.h"

class cFollowCurve : public iCommand
{
public:
	cFollowCurve()
	{
		this->m_UpdateHasBeenCalled = false;
	}
	
	virtual void Init(std::vector<sPair> vecDetails);
	virtual void SetGameObject(cGameObject* pGO);
	virtual void SetTarget(cGameObject* pTargetGO) { return; }
	virtual void Update(double deltaTime);
	virtual bool IsDone(void);
	virtual void Reset(void);
	virtual void setName(std::string name) { this->m_Name = name; }
	virtual std::string getName(void) { return this->m_Name; }
	virtual void SetLoopTimes(int times) { return; }
	virtual void AddCommandSerial(iCommand* pCommand) { return; }
	virtual void AddCommandParallel(iCommand* pCommand) { return; }

private:
	bool m_UpdateHasBeenCalled;
	std::string m_Name;
	cGameObject* m_pTheGO;
	glm::vec3 m_startPosition;
	glm::vec3 m_intermediatePoint;
	glm::vec3 m_endPosition;
	float m_TimeToFollow;
	double m_TimeElapsed;
	unsigned int m_SmoothLevel;
};