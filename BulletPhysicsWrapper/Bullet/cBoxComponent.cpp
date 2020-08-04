#include <cBoxComponent.h>
#include "nConvert.h"

namespace nPhysics
{
	cBoxComponent::cBoxComponent(const sBoxDef& def)
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
	}

	cBoxComponent::~cBoxComponent()
	{
		mBody->setUserPointer(0);
		delete mBody->getCollisionShape();
		delete mBody->getMotionState();
		delete mBody;
		mBody = 0;
	}

	void cBoxComponent::GetTransform(glm::mat4& transformOut)
	{
		btTransform transform;
		mBody->getMotionState()->getWorldTransform(transform);
		nConvert::ToSimple(transform, transformOut);
	}

	void cBoxComponent::GetPosition(glm::vec3& positionOut)
	{
		//mBody->GetPosition(positionOut);
		return;
	}

	void cBoxComponent::SetPosition(const glm::vec3& positionIn)
	{
		return;
	}

	void cBoxComponent::GetMass(float& massOut)
	{
		massOut = mBody->getMass();
	}

	void cBoxComponent::ApplyForce(const glm::vec3& force)
	{
		mBody->activate(true);
		mBody->applyCentralForce(nConvert::ToBullet(force));
	}

	void cBoxComponent::SetUniqueID(int ID)
	{
		mBody->setUserIndex(ID);
	}

	int cBoxComponent::GetUniqueID()
	{
		return mBody->getUserIndex();
	}

	bool cBoxComponent::TriggeredBy(int ID)
	{		
		return false;
	}

	void cBoxComponent::SetMotorVelocity(float targetVelocity)
	{
		return;
	}

	void cBoxComponent::SetActive()
	{
		return;
	}
}
