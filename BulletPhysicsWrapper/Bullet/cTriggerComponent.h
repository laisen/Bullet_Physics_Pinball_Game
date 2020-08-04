#pragma once
#include <iTriggerComponent.h>
#include "btBulletDynamicsCommon.h"
#include "BulletCollision/CollisionDispatch/btGhostObject.h"

namespace nPhysics
{
	class cTriggerComponent : public iTriggerComponent
	{
		friend class cPhysicsWorld;

	public:
		cTriggerComponent(const sTriggerDef& def);
		virtual ~cTriggerComponent();
		virtual void GetTransform(glm::mat4& transformOut) override;
		virtual void GetPosition(glm::vec3& positionOut) override;
		virtual void SetPosition(const glm::vec3& positionIn) override;
		virtual void GetMass(float& massOut) override;
		virtual void ApplyForce(const glm::vec3& force) override;

		virtual void SetUniqueID(int ID) override;
		virtual int GetUniqueID() override;

		virtual bool TriggeredBy(int ID) override;

		virtual void SetMotorVelocity(float targetVelocity) override;
		virtual void SetActive() override;

	private:
		btPairCachingGhostObject* mGhostObject;

		cTriggerComponent() = delete;
		cTriggerComponent(const cTriggerComponent& other) = delete;
		cTriggerComponent& operator=(const cTriggerComponent& other) = delete;
	};
}
