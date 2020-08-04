#pragma once
#include <iPlaneComponent.h>
#include "btBulletDynamicsCommon.h"

namespace nPhysics
{
	class cPlaneComponent : public iPlaneComponent
	{
		friend class cPhysicsWorld;
		
	public:
		cPlaneComponent(const sPlaneDef& def);
		virtual ~cPlaneComponent();
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

		cPlaneComponent() = delete;
		cPlaneComponent(const cPlaneComponent& other) = delete;
		cPlaneComponent& operator=(const cPlaneComponent& other) = delete;
	};
}


