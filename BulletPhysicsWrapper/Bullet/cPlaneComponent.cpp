#include "cPlaneComponent.h"
#include "nConvert.h"

namespace nPhysics
{
	cPlaneComponent::cPlaneComponent(const sPlaneDef& def)
	{
		btCollisionShape* shape = new btStaticPlaneShape(nConvert::ToBullet(def.Normal), def.Constant);

		btTransform transform;
		transform.setIdentity();
		
		btDefaultMotionState* myMotionState = new btDefaultMotionState(transform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(0.0f, myMotionState, shape, btVector3(0, 0, 0));
		rbInfo.m_restitution = 1.0f;
		mBody = new btRigidBody(rbInfo);
		mBody->setUserPointer(this);
	}

	cPlaneComponent::~cPlaneComponent()
	{
		mBody->setUserPointer(0);
		delete mBody->getCollisionShape();
		delete mBody->getMotionState();
		delete mBody;
		mBody = 0;
	}

	void cPlaneComponent::GetTransform(glm::mat4& transformOut)
	{
		btTransform transform;
		mBody->getMotionState()->getWorldTransform(transform);
		nConvert::ToSimple(transform, transformOut);
	}

	void cPlaneComponent::GetPosition(glm::vec3& positionOut)
	{
		return;
	}

	void cPlaneComponent::SetPosition(const glm::vec3& positionIn)
	{
		return;
	}

	void cPlaneComponent::GetMass(float& massOut)
	{
		return;
	}

	void cPlaneComponent::ApplyForce(const glm::vec3& force)
	{
		return;
	}
	void cPlaneComponent::SetUniqueID(int ID)
	{
		mBody->setUserIndex(ID);
	}

	int cPlaneComponent::GetUniqueID()
	{
		return mBody->getUserIndex();
	}
	
	bool cPlaneComponent::TriggeredBy(int ID)
	{
		return false;
	}

	void cPlaneComponent::SetMotorVelocity(float targetVelocity)
	{
		return;
	}

	void cPlaneComponent::SetActive()
	{
		return;
	}
}

