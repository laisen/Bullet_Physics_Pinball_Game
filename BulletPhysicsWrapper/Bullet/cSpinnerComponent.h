#pragma once
#include "iSpinnerComponent.h"
#include "btBulletDynamicsCommon.h"

namespace nPhysics
{
	class cSpinnerComponent : public iSpinnerComponent
	{
		friend class cPhysicsWorld;

	public:
		cSpinnerComponent(const sSpinnerDef& def);
		virtual ~cSpinnerComponent();
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

		cSpinnerComponent() = delete;
		cSpinnerComponent(const cSpinnerComponent& other) = delete;
		cSpinnerComponent& operator=(const cSpinnerComponent& other) = delete;
	};
}
