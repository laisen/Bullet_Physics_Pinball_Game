#pragma once
#include "iPhysicsComponent.h"

namespace nPhysics
{
	struct sTriggerDef
	{		
		glm::vec3 Position;
		glm::vec3 HalfExtents;
	};

	class iTriggerComponent : public iPhysicsComponent
	{
	public:
		virtual ~iTriggerComponent() {}

	protected:
		iTriggerComponent() : iPhysicsComponent(eComponentType::trigger) {}

	private:
		iTriggerComponent(const iTriggerComponent& other) = delete;
		iTriggerComponent& operator=(const iTriggerComponent& other) = delete;
	};
}