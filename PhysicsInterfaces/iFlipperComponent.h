#pragma once
#include "iPhysicsComponent.h"

namespace nPhysics
{
	struct sFlipperDef
	{
		float Mass;
		glm::vec3 Position;
		glm::vec3 HalfExtents;
		glm::vec3 Rotation;
	};

	class iFlipperComponent : public iPhysicsComponent
	{
	public:
		virtual ~iFlipperComponent() {}

	protected:
		iFlipperComponent() : iPhysicsComponent(eComponentType::flipper) {}

	private:
		iFlipperComponent(const iFlipperComponent& other) = delete;
		iFlipperComponent& operator=(const iFlipperComponent& other) = delete;
	};
}