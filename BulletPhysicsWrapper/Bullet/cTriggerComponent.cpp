#include "cTriggerComponent.h"
#include "nConvert.h"

namespace nPhysics
{
	cTriggerComponent::cTriggerComponent(const sTriggerDef& def)
	{
		mGhostObject = new btPairCachingGhostObject();
		mGhostObject->setUserPointer(this);
		mGhostObject->setCollisionShape( new btBoxShape(nConvert::ToBullet(def.HalfExtents)) );

		btTransform transform;
		transform.setIdentity();
		transform.setOrigin(nConvert::ToBullet(def.Position));
		mGhostObject->setWorldTransform(transform);

		mGhostObject->setCollisionFlags(mGhostObject->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);


	}

	cTriggerComponent::~cTriggerComponent()
	{
		mGhostObject->setUserPointer(0);
		delete mGhostObject->getCollisionShape();		
		delete mGhostObject;
		mGhostObject = 0;
	}

	void cTriggerComponent::GetTransform(glm::mat4& transformOut)
	{
		btTransform transform(mGhostObject->getWorldTransform());		
		nConvert::ToSimple(transform, transformOut);
	}

	void cTriggerComponent::GetPosition(glm::vec3& positionOut)
	{
		return;
	}

	void cTriggerComponent::SetPosition(const glm::vec3& positionIn)
	{
		return;
	}

	void cTriggerComponent::GetMass(float& massOut)
	{
		return;
	}

	void cTriggerComponent::ApplyForce(const glm::vec3& force)
	{
		return;
	}

	void cTriggerComponent::SetUniqueID(int ID)
	{
		mGhostObject->setUserIndex(ID);
	}

	int cTriggerComponent::GetUniqueID()
	{
		return mGhostObject->getUserIndex();
	}

	bool cTriggerComponent::TriggeredBy(int ID)
	{
		int num = mGhostObject->getNumOverlappingObjects();
		for (size_t i = 0; i < num; i++)
		{
			if (mGhostObject->getOverlappingObject(i)->getUserIndex() == ID)
			{
				return true;
			}
		}
		return false;
	}

	void cTriggerComponent::SetMotorVelocity(float targetVelocity)
	{
		return;
	}

	void cTriggerComponent::SetActive()
	{
		return;
	}


}