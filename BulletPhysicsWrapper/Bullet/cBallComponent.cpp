#include "cBallComponent.h"
#include "nConvert.h"

namespace nPhysics
{
	cBallComponent::cBallComponent(const sBallDef& def)
	{
		btCollisionShape* shape = new btSphereShape(def.Radius);

		btTransform transform;
		transform.setIdentity();
		transform.setOrigin(nConvert::ToBullet(def.Position));
		
		btScalar mass(def.Mass);

		bool isDynamic = (mass != 0.0f);

		btVector3 localInertia(0, 0, 0);
		if (isDynamic)
		{
			shape->calculateLocalInertia(mass, localInertia);
		}

		btDefaultMotionState* myMotionState = new btDefaultMotionState(transform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, shape, localInertia);
		rbInfo.m_restitution = 0.4f;
		//rbInfo.m_rollingFriction = 0.382f;
		mBody = new btRigidBody(rbInfo);
		mBody->setFriction(0.3f);
		mBody->setRollingFriction(0.1f);
		mBody->setSpinningFriction(0.1f);
		mBody->setAnisotropicFriction(shape->getAnisotropicRollingFrictionDirection(), btCollisionObject::CF_ANISOTROPIC_ROLLING_FRICTION);
		mBody->setUserPointer(this);	
		
	}

	cBallComponent::~cBallComponent()
	{
		mBody->setUserPointer(0);
		delete mBody->getCollisionShape();
		delete mBody->getMotionState();
		delete mBody;
		mBody = 0;
	}

	void cBallComponent::GetTransform(glm::mat4& transformOut)
	{
		btTransform transform;
		mBody->getMotionState()->getWorldTransform(transform);
		nConvert::ToSimple(transform, transformOut);
	}

	void cBallComponent::GetPosition(glm::vec3& positionOut)
	{
		//mBody->GetPosition(positionOut);
		return;
	}

	void cBallComponent::SetPosition(const glm::vec3& positionIn)
	{		
		mBody->setLinearVelocity(btVector3(0.0f, 0.0f, 0.0f));
		mBody->setAngularVelocity(btVector3(0.0f, 0.0f, 0.0f));
		btTransform transform;
		transform.setIdentity();
		transform.setOrigin(nConvert::ToBullet(positionIn));		
		mBody->proceedToTransform(transform);		
	}

	void cBallComponent::GetMass(float& massOut)
	{		
		massOut = mBody->getMass();
	}

	void cBallComponent::ApplyForce(const glm::vec3& force)
	{
		mBody->activate(true);
		mBody->applyCentralForce(nConvert::ToBullet(force));
	}

	void cBallComponent::SetUniqueID(int ID)
	{
		mBody->setUserIndex(ID);
	}

	int cBallComponent::GetUniqueID()
	{
		return mBody->getUserIndex();
	}

	bool cBallComponent::TriggeredBy(int ID)
	{
		return false;
	}

	void cBallComponent::SetMotorVelocity(float targetVelocity)
	{
		return;
	}

	void cBallComponent::SetActive()
	{
		mBody->activate(true);
	}
}

