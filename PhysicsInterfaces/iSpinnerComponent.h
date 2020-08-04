#pragma once
#include "iPhysicsComponent.h"

namespace nPhysics
{
	struct sSpinnerDef
	{
		float Mass;
		float Radius;
		float BetweenHeight;
		glm::vec3 Position;
		//glm::vec3 HalfExtents;
		glm::vec3 Rotation;
	};

	class iSpinnerComponent : public iPhysicsComponent
	{
	public:
		virtual ~iSpinnerComponent() {}

	protected:
		iSpinnerComponent() : iPhysicsComponent(eComponentType::spinner) {}

	private:
		iSpinnerComponent(const iSpinnerComponent& other) = delete;
		iSpinnerComponent& operator=(const iSpinnerComponent& other) = delete;
	};
}