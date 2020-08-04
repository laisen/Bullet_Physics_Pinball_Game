#pragma once
#include "..//INFO-6022_Project6/include/glm/glm.hpp"
#include "iPhysicsComponent.h"

namespace nPhysics
{
	struct sBallDef
	{
		float Mass;
		float Radius;
		glm::vec3 Position;
	};

	class iBallComponent : public iPhysicsComponent
	{
	public:
		virtual ~iBallComponent() {}

	protected:
		iBallComponent() : iPhysicsComponent(eComponentType::ball) {}

	private:
		iBallComponent(const iBallComponent& other) = delete;
		iBallComponent& operator=(const iBallComponent& other) = delete;
	};
}