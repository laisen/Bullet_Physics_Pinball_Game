#include "cFlipperComponent.h"
#include "nConvert.h"

namespace nPhysics
{
	cFlipperComponent::cFlipperComponent(const sFlipperDef& def)
	{
		btCollisionShape* shape = new btBoxShape(nConvert::ToBullet(def.HalfExtents));

		btTransform transform;
		transform.setIdentity();
		transform.setOrigin(nConvert::ToBullet(def.Position));
		transform.setRotation(btQuaternion(glm::radians(def.Rotation.y), glm::radians(def.Rotation.x), glm::radians(def.Rotation.z)));

		btScalar mass(def.Mass);

		bool isDynamic = (mass != 0.0f);

		btVector3 localInertia(0, 0, 0);
		if (isDynamic)
		{
			shape->calculateLocalInertia(mass, localInertia);
		}

		btDefaultMotionState* myMotionState = new btDefaultMotionState(transform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, shape, localInertia);
		rbInfo.m_restitution = 1.0f;
		mBody = new btRigidBody(rbInfo);
		mBody->setUserPointer(this);
		
		if (def.Position.x > 0.0f)		// Right flipper
		{
			mConstraint = new btHingeConstraint(*mBody, btVector3(def.HalfExtents.x, 0.0f, 0.0f), btVector3(0.0f, 1.0f, 0.0f));
		}
		else		// Left flipper
		{
			mConstraint = new btHingeConstraint(*mBody, btVector3(-def.HalfExtents.x, 0.0f, 0.0f), btVector3(0.0f, 1.0f, 0.0f));
		}
		mConstraint->setLimit(glm::radians(-30.0f), glm::radians(30.0f));				
		mConstraint->enableMotor(true);
		mConstraint->setMaxMotorImpulse(1000.0f);		
	}

	cFlipperComponent::~cFlipperComponent()
	{
		mBody->setUserPointer(0);
		delete mBody->getCollisionShape();
		delete mBody->getMotionState();
		delete mBody;
		delete mConstraint;
		mBody = 0;
		mConstraint = 0;
	}
	void cFlipperComponent::GetTransform(glm::mat4& transformOut)
	{
		btTransform transform;
		mBody->getMotionState()->getWorldTransform(transform);
		nConvert::ToSimple(transform, transformOut);
	}
	void cFlipperComponent::GetPosition(glm::vec3& positionOut)
	{
		return;
	}

	void cFlipperComponent::SetPosition(const glm::vec3& positionIn)
	{
		return;
	}
	void cFlipperComponent::GetMass(float& massOut)
	{
		return;
	}

	void cFlipperComponent::ApplyForce(const glm::vec3& force)
	{
		return;
	}

	void cFlipperComponent::SetUniqueID(int ID)
	{
		mBody->setUserIndex(ID);
	}

	int cFlipperComponent::GetUniqueID()
	{
		return mBody->getUserIndex();
	}

	bool cFlipperComponent::TriggeredBy(int ID)
	{
		return false;
	}

	void cFlipperComponent::SetMotorVelocity(float targetVelocity)
	{
		// Make it to flip
		mBody->activate(true);		
		mConstraint->setMotorTargetVelocity(targetVelocity);
		
	}

	void cFlipperComponent::SetActive()
	{
		return;
	}
}