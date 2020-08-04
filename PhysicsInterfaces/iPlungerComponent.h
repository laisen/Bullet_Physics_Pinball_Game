#pragma once
#include "iPhysicsComponent.h"

namespace nPhysics
{
	struct sPlungerDef
	{
		float Mass;
		glm::vec3 Position;
		glm::vec3 HalfExtents;
		glm::vec3 Rotation;
	};

	class iPlungerComponent : public iPhysicsComponent
	{
	public:
		virtual ~iPlungerComponent() {}

	protected:
		iPlungerComponent() : iPhysicsComponent(eComponentType::plunger) {}

	private:
		iPlungerComponent(const iPlungerComponent& other) = delete;
		iPlungerComponent& operator=(const iPlungerComponent& other) = delete;
	};
}