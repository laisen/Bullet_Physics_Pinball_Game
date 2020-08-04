#pragma once
#include "iCommand.h"

class cCommandGroup : public iCommand
{
public: 
	cCommandGroup();
	virtual void SetGameObject(cGameObject* pGO) { return; }
	virtual void SetTarget(cGameObject* pTargetGO) { return; }
	virtual void Init(std::vector<sPair> vecDetails) { return; }
	virtual void Update(double deltaTime);
	virtual bool IsDone(void);
	virtual void Reset(void);
	virtual void setName(std::string name) { this->m_Name = name; }
	virtual std::string getName(void) { return this->m_Name; }
	virtual void SetLoopTimes(int times) { this->m_LoopTimes = times; }
	virtual void AddCommandSerial(iCommand* pCommand);
	virtual void AddCommandParallel(iCommand* pCommand);

private:
	std::string m_Name;

	std::vector< iCommand* > vecSerial;
	std::vector< iCommand* > vecParallel;

	unsigned int m_CurrentSerialIndex;
	int m_LoopTimes;
};
