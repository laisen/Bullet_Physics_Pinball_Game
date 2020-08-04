#pragma once
#include "iCommand.h"

class cTrigger : public iCommand
{
public:
	cTrigger();
	virtual void SetGameObject(cGameObject* pGO);
	virtual void SetTarget(cGameObject* pTargetGO) { return; }
	virtual void Init(std::vector<sPair> vecDetails);
	virtual void Update(double deltaTime);
	virtual bool IsDone(void);
	virtual void Reset(void);
	virtual void setName(std::string name) { this->m_Name = name; }
	virtual std::string getName(void) { return this->m_Name; }
	virtual void SetLoopTimes(int times) { this->m_LoopTimes = times; }
	virtual void AddCommandSerial(iCommand* pCommand);
	virtual void AddCommandParallel(iCommand* pCommand);
	bool GOinsideAABB();

private:
	std::string m_Name;
	cGameObject* m_pTheGO;
	cGameObject* m_pDebugAABBGO;
	std::vector< iCommand* > vecSerial;
	std::vector< iCommand* > vecParallel;
	glm::vec3 m_AABBCentre;
	glm::vec3 m_AABBminXYZ;
	float m_AABBlength;		// Cube
	unsigned int m_CurrentSerialIndex;
	int m_LoopTimes;
};