#include "cTrigger.h"

extern std::vector<cGameObject*> g_vec_pDebugGameObjects;

cTrigger::cTrigger()
{
	this->m_CurrentSerialIndex = 0;
	this->m_LoopTimes = -1;
	return;
}

void cTrigger::SetGameObject(cGameObject* pGO)
{
	this->m_pTheGO = pGO;
	return;
}

void cTrigger::Init(std::vector<sPair> vecDetails)
{
	this->m_AABBCentre = vecDetails[0].numData;
	this->m_AABBlength = vecDetails[1].numData.x;
	this->m_AABBminXYZ = glm::vec3(this->m_AABBCentre.x - (this->m_AABBlength / 2.0f),
		this->m_AABBCentre.y - (this->m_AABBlength / 2.0f),
		this->m_AABBCentre.z - (this->m_AABBlength / 2.0f));

	cGameObject* cubeAABB = new cGameObject();
	cubeAABB->meshName = "cubeMesh";
	cubeAABB->friendlyName = "trigger";
	cubeAABB->positionXYZ = glm::vec3(this->m_AABBCentre);
	cubeAABB->scale = this->m_AABBlength / 2.0f;
	cubeAABB->isWireframe = true;
	cubeAABB->wireframeColour = glm::vec4(0.3f, 0.9f, 0.3f, 1.0f);
	g_vec_pDebugGameObjects.push_back(cubeAABB);
	this->m_pDebugAABBGO = cubeAABB;

	return;
}

void cTrigger::Update(double deltaTime)
{
	if (this->m_LoopTimes == 0)
	{
		return;		// Loop over
	}

	if (!GOinsideAABB())
	{
		this->m_pDebugAABBGO->wireframeColour = glm::vec4(0.3f, 0.9f, 0.3f, 1.0f);
		return;
	}
	else
	{
		this->m_pDebugAABBGO->wireframeColour = glm::vec4(0.9f, 0.3f, 0.3f, 1.0f);
	}
	
	if (this->vecSerial.begin() + this->m_CurrentSerialIndex != this->vecSerial.end())
	{
		iCommand* pCurrent = *(this->vecSerial.begin() + this->m_CurrentSerialIndex);
		if (!pCurrent->IsDone())
		{
			pCurrent->Update(deltaTime);
		}
		else
		{
			if (this->m_CurrentSerialIndex != vecSerial.size())
			{
				this->m_CurrentSerialIndex++;
			}
		}
	}

	for (std::vector< iCommand* >::iterator itCommand = this->vecParallel.begin();
		itCommand != this->vecParallel.end(); itCommand++)
	{
		iCommand* pCurrent = *itCommand;

		if (!pCurrent->IsDone())
		{
			pCurrent->Update(deltaTime);
		}
	}

	if (IsDone())
	{
		if (this->m_LoopTimes >= 1)
		{
			this->m_LoopTimes--;
			Reset();		// Make it repeatable
		}
		else if (this->m_LoopTimes == -1/*infinite loop code*/)
		{
			Reset();
		}
	}
	return;
}

bool cTrigger::IsDone(void)
{
	bool bAllDone = true;

	// Serial commands
	for (std::vector< iCommand* >::iterator itCommand = this->vecSerial.begin();
		itCommand != this->vecSerial.end(); itCommand++)
	{
		iCommand* pCurrent = *itCommand;

		if (!pCurrent->IsDone())
		{
			bAllDone = false;
		}
	}

	// Parallel commands
	for (std::vector< iCommand* >::iterator itCommand = this->vecParallel.begin();
		itCommand != this->vecParallel.end(); itCommand++)
	{
		iCommand* pCurrent = *itCommand;

		if (!pCurrent->IsDone())
		{
			bAllDone = false;
		}
	}

	return bAllDone;
}

void cTrigger::Reset(void)
{
	this->m_CurrentSerialIndex = 0;

	// Serial commands
	for (std::vector< iCommand* >::iterator itCommand = this->vecSerial.begin();
		itCommand != this->vecSerial.end(); itCommand++)
	{
		iCommand* pCurrent = *itCommand;

		pCurrent->Reset();
	}

	// Parallel commands
	for (std::vector< iCommand* >::iterator itCommand = this->vecParallel.begin();
		itCommand != this->vecParallel.end(); itCommand++)
	{
		iCommand* pCurrent = *itCommand;

		pCurrent->Reset();
	}
}

void cTrigger::AddCommandSerial(iCommand* pCommand)
{

	this->vecSerial.push_back(pCommand);

	return;
}

void cTrigger::AddCommandParallel(iCommand* pCommand)
{	
	this->vecParallel.push_back(pCommand);

	return;
}

bool cTrigger::GOinsideAABB()
{
	if (this->m_pTheGO->positionXYZ.x < this->m_AABBminXYZ.x )	return false;
	if (this->m_pTheGO->positionXYZ.x > this->m_AABBminXYZ.x + this->m_AABBlength)	return false;
	if (this->m_pTheGO->positionXYZ.y < this->m_AABBminXYZ.y)	return false;
	if (this->m_pTheGO->positionXYZ.y > this->m_AABBminXYZ.y + this->m_AABBlength)	return false;
	if (this->m_pTheGO->positionXYZ.z < this->m_AABBminXYZ.z)	return false;
	if (this->m_pTheGO->positionXYZ.z > this->m_AABBminXYZ.z + this->m_AABBlength)	return false;	

	return true;
}