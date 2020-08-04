#pragma once

#include "iCommand.h"

class cFollowObject : public iCommand
{
public:
	//cFollowObject()
	//{
	//	this->m_UpdateHasBeenCalled = false;
	//}

	virtual void Init(std::vector<sPair> vecDetails);
	virtual void SetGameObject(cGameObject* pGO);
	virtual void SetTarget(cGameObject* pTargetGO);
	virtual void Update(double deltaTime);
	virtual bool IsDone(void);		// Update could be never done
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
	cGameObject* m_pTargetGO;	
	glm::vec3 m_offsetFromTargetXYZ;		// The offset of that
	float m_followDistance;		// the closest we’ll get to the object
	float m_maxFollowSpeed;		// min speed is zero
	float m_slowdownDistance;
	float m_slowdownRegion;
	glm::vec3 m_originalStartPosition;
};

