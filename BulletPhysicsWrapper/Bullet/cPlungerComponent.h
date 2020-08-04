#pragma once

#include "iPlungerComponent.h"
#include "btBulletDynamicsCommon.h"

namespace nPhysics
{
	class cPlungerComponent : public iPlungerComponent
	{
		friend class cPhysicsWorld;

	public:
		cPlungerComponent(const sPlungerDef& def);
		virtual ~cPlungerComponent();
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
		btSliderConstraint* mConstraint;

		cPlungerComponent() = delete;
		cPlungerComponent(const cPlungerComponent& other) = delete;
		cPlungerComponent& operator=(const cPlungerComponent& other) = delete;
	};
}
