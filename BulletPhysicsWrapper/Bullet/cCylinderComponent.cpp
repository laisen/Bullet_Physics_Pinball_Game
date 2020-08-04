#include "cCylinderComponent.h"
#include "nConvert.h"

namespace nPhysics
{
	cCylinderComponent::cCylinderComponent(const sCylinderDef& def)
	{
		btCollisionShape* shape = new btCylinderShape(btVector3(def.Radius, def.HalfHeight, def.Radius));

		btTransform transform;
		transform.setIdentity();
		transform.setOrigin(nConvert::ToBullet(def.Position));
		transform.setRotation(btQuaternion(def.Rotation.y, def.Rotation.x, def.Rotation.z));

		btScalar mass(def.Mass);

		bool isDynamic = (mass != 0.0f);

		btVector3 localInertia(0, 0, 0);
		if (isDynamic)
		{
			shape->calculateLocalInertia(mass, localInertia);
		}

		btDefaultMotionState* myMotionState = new btDefaultMotionState(transform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, shape, localInertia);
		rbInfo.m_restitution = 7.0f;
		//rbInfo.m_rollingFriction = 0.382f;
		mBody = new btRigidBody(rbInfo);
		//mBody->setFriction(1.0f);
		//mBody->setRollingFriction(0.1f);
		//mBody->setSpinningFriction(0.1f);
		//mBody->setAnisotropicFriction(shape->getAnisotropicRollingFrictionDirection(), btCollisionObject::CF_ANISOTROPIC_ROLLING_FRICTION);
		mBody->setUserPointer(this);

		mGhostObject = new btPairCachingGhostObject();
		mGhostObject->setUserPointer(this);
		mGhostObject->setCollisionShape(shape);
		mGhostObject->setWorldTransform(transform);
		mGhostObject->setCollisionFlags(mGhostObject->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
	}

	cCylinderComponent::~cCylinderComponent()
	{
		mBody->setUserPointer(0);
		delete mBody->getCollisionShape();
		delete mBody->getMotionState();
		delete mBody;
		mBody = 0;
		mGhostObject->setUserPointer(0);
		delete mGhostObject->getCollisionShape();
		delete mGhostObject;
		mGhostObject = 0;
	}

	void cCylinderComponent::GetTransform(glm::mat4& transformOut)
	{
		btTransform transform;
		mBody->getMotionState()->getWorldTransform(transform);
		nConvert::ToSimple(transform, transformOut);
	}

	void cCylinderComponent::GetPosition(glm::vec3& positionOut)
	{
		return;
	}

	void cCylinderComponent::SetPosition(const glm::vec3& positionIn)
	{
		return;
	}

	void cCylinderComponent::GetMass(float& massOut)
	{
		return;
	}

	void cCylinderComponent::ApplyForce(const glm::vec3& force)
	{
		return;
	}

	void cCylinderComponent::SetUniqueID(int ID)
	{
		mBody->setUserIndex(ID);
	}

	int cCylinderComponent::GetUniqueID()
	{
		return mBody->getUserIndex();
	}

	bool cCylinderComponent::TriggeredBy(int ID)
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

	void cCylinderComponent::SetMotorVelocity(float targetVelocity)
	{
		return;
	}

	void cCylinderComponent::SetActive()
	{
		return;
	}

}
