#pragma once
#include "iPhysicsComponent.h"

namespace nPhysics
{
	struct sBoxDef
	{
		float Mass;
		glm::vec3 Position;
		glm::vec3 HalfExtents;
		glm::vec3 Rotation;
	};

	class iBoxComponent : public iPhysicsComponent
	{
	public:
		virtual ~iBoxComponent() {}

	protected:
		iBoxComponent() : iPhysicsComponent(eComponentType::box) {}

	private:
		iBoxComponent(const iBoxComponent& other) = delete;
		iBoxComponent& operator=(const iBoxComponent& other) = delete;
	};
}