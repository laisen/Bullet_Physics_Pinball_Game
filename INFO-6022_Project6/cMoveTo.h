#pragma once
#include "iCommand.h"

class cMoveTo : public iCommand
{
public:
	// Pass: 
	// - Start location (vec3)
	// - End location (vec3)
	// - Number of seconds to move (x)
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
	glm::vec3 m_endPosition;
	float m_TimeToMove;
	float m_TimeToEaseIn;
	float m_TimeToEaseOut;
	glm::vec3 m_velocityMax;
	glm::vec3 m_direction;
	float m_totalDistance;
	double m_TimeElapsed;
};