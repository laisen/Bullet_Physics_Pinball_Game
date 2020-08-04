#pragma once
#include "..//INFO-6022_Project6/include/glm/glm.hpp"
//#include "..//INFO-6022_Project6/include/glm/gtx/quaternion.hpp"
#include "btBulletDynamicsCommon.h"
namespace nConvert
{
	inline btVector3 ToBullet(const glm::vec3 vec)
	{		
		return btVector3(vec.x, vec.y, vec.z);
	}

	inline void ToSimple(const btTransform& transformIn, glm::mat4& transformOut)
	{
		transformIn.getOpenGLMatrix(&transformOut[0][0]);
	}

}