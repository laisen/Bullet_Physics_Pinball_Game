#pragma once
#include "..//INFO-6022_Project6/include/glm/glm.hpp"
#include "eComponentType.h"

namespace nPhysics
{
	class iPhysicsComponent
	{
	public:
		virtual ~iPhysicsComponent() {}
		inline const eComponentType& GetComponentType() { return mComponentType; }

		virtual void GetTransform(glm::mat4& transformOut) = 0;
		virtual void GetPosition(glm::vec3& positionOut) = 0;
		virtual void SetPosition(const glm::vec3& positionIn) = 0;
		virtual void GetMass(float& massOut) = 0;
		virtual void ApplyForce(const glm::vec3& force) = 0;
		virtual void SetUniqueID(int ID) = 0;
		virtual int GetUniqueID() = 0;
		virtual bool TriggeredBy(int ID) = 0;
		virtual void SetMotorVelocity(float targetVelocity) = 0;
		virtual void SetActive() = 0;

	protected:
		iPhysicsComponent(eComponentType componentType)
			: mComponentType(componentType) {}
	private:
		eComponentType mComponentType;
		
		iPhysicsComponent() = delete;
		iPhysicsComponent(const iPhysicsComponent& other) = delete;
		iPhysicsComponent& operator=(const iPhysicsComponent& other) = delete;
	};
}