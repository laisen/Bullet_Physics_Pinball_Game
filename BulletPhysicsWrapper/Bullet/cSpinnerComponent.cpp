#include "cSpinnerComponent.h"
#include "nConvert.h"

namespace nPhysics
{
	cSpinnerComponent::cSpinnerComponent(const sSpinnerDef& def)
	{		
		btCollisionShape* shape = new btCapsuleShapeX(def.Radius, def.BetweenHeight);

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

		mConstraint = new btHingeConstraint(*mBody, btVector3(0.0f, 0.0f, 0.0f), btVector3(0.0f, 1.0f, 0.0f));
		mConstraint->enableAngularMotor(true, 0.0f, 0.2f);
	}

	cSpinnerComponent::~cSpinnerComponent()
	{
		mBody->setUserPointer(0);
		delete mBody->getCollisionShape();
		delete mBody->getMotionState();
		delete mBody;
		delete mConstraint;
		mBody = 0;
		mConstraint = 0;
	}

	void cSpinnerComponent::GetTransform(glm::mat4& transformOut)
	{
		btTransform transform;
		mBody->getMotionState()->getWorldTransform(transform);
		nConvert::ToSimple(transform, transformOut);
	}

	void cSpinnerComponent::GetPosition(glm::vec3& positionOut)
	{
		return;
	}

	void cSpinnerComponent::SetPosition(const glm::vec3& positionIn)
	{
		return;
	}

	void cSpinnerComponent::GetMass(float& massOut)
	{
		return;
	}

	void cSpinnerComponent::ApplyForce(const glm::vec3& force)
	{
		return;
	}

	void cSpinnerComponent::SetUniqueID(int ID)
	{
		mBody->setUserIndex(ID);
	}

	int cSpinnerComponent::GetUniqueID()
	{
		return mBody->getUserIndex();
	}

	bool cSpinnerComponent::TriggeredBy(int ID)
	{
		return false;
	}

	void cSpinnerComponent::SetMotorVelocity(float targetVelocity)
	{
		return;
	}

	void cSpinnerComponent::SetActive()
	{
		return;
	}

}