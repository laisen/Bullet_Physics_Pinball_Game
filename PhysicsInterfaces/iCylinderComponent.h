#pragma once
#include "iPhysicsComponent.h"

namespace nPhysics
{
	struct sCylinderDef
	{
		float Mass;
		float Radius;
		float HalfHeight;
		glm::vec3 Position;
		glm::vec3 Rotation;
	};

	class iCylinderComponent : public iPhysicsComponent
	{
	public:
		virtual ~iCylinderComponent() {}

	protected:
		iCylinderComponent() : iPhysicsComponent(eComponentType::cylinder) {}

	private:
		iCylinderComponent(const iCylinderComponent& other) = delete;
		iCylinderComponent& operator=(const iCylinderComponent& other) = delete;
	};
}