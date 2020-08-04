#pragma once
#include <string>
#include <vector>
#include "cGameObject.h"
#include <glm/glm.hpp>
#include <glm/vec4.hpp>

struct sPair
{
	sPair() {};
	sPair(std::string stringData, glm::vec4 numData)
	{
		this->stringData = stringData;
		this->numData = numData;
	}
	sPair(std::string stringData)
	{
		this->stringData = stringData;
	}
	sPair(glm::vec4 numData)
	{
		this->numData = numData;
	}
	sPair(glm::vec3 numData)
	{
		this->numData = glm::vec4( numData, 1.0f );
	}
	sPair(float num)
	{
		this->numData.x = num;
	}
	std::string stringData;
	glm::vec4 numData;
};

// This is the base interface for all commands and command groups
class iCommand
{
public:
	virtual ~iCommand() {};
	virtual void SetGameObject( cGameObject* pGO ) = 0;
	virtual void SetTarget(cGameObject* pTargetGO) = 0;
	virtual void Init( std::vector<sPair> vecDetails ) = 0;
	virtual void Update( double deltaTime ) = 0;
	virtual bool IsDone(void) = 0;
	virtual void Reset(void) = 0;
	virtual void setName(std::string name) = 0;
	virtual std::string getName(void) = 0;
	virtual void SetLoopTimes(int times) = 0;
	
	virtual void AddCommandSerial( iCommand* pCommand ) = 0;
	virtual void AddCommandParallel( iCommand* pCommand ) = 0;	


};