#pragma once
#include "iFlipperComponent.h"
#include "btBulletDynamicsCommon.h"

namespace nPhysics
{
	class cFlipperComponent : public iFlipperComponent
	{
		friend class cPhysicsWorld;

	public:
		cFlipperComponent(const sFlipperDef& def);
		virtual ~cFlipperComponent();
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
		btHingeConstraint* mConstraint;

		cFlipperComponent() = delete;
		cFlipperComponent(const cFlipperComponent& other) = delete;
		cFlipperComponent& operator=(const cFlipperComponent& other) = delete;
	};
}
