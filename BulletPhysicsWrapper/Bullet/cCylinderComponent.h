#pragma once
class cCylinderComponent
{
};

#pragma once
#include <iCylinderComponent.h>
#include "btBulletDynamicsCommon.h"
#include "BulletCollision/CollisionDispatch/btGhostObject.h"

namespace nPhysics
{
	class cCylinderComponent : public iCylinderComponent
	{
		friend class cPhysicsWorld;

	public:
		cCylinderComponent(const sCylinderDef& def);
		virtual ~cCylinderComponent();
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
		btRigidBody* mBody;
		btPairCachingGhostObject* mGhostObject;

		cCylinderComponent() = delete;
		cCylinderComponent(const cCylinderComponent& other) = delete;
		cCylinderComponent& operator=(const cCylinderComponent& other) = delete;
	};
}
