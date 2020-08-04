#include "cPlungerComponent.h"
#include "nConvert.h"

namespace nPhysics
{
	cPlungerComponent::cPlungerComponent(const sPlungerDef& def)
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
		btTransform transformSlider;
		transformSlider.setIdentity();
		transformSlider.setRotation(btQuaternion(glm::radians(90.0f), glm::radians(0.0f), glm::radians(0.0f)));
		mConstraint = new btSliderConstraint(*mBody, transformSlider, false);
		mConstraint->setLowerLinLimit(-3.0f);
		mConstraint->setUpperLinLimit(3.0f);
		mConstraint->setMaxLinMotorForce(2000.0f);
		mConstraint->setPoweredLinMotor(true);
	}

	cPlungerComponent::~cPlungerComponent()
	{
		mBody->setUserPointer(0);
		delete mBody->getCollisionShape();
		delete mBody->getMotionState();
		delete mBody;
		delete mConstraint;
		mBody = 0;
		mConstraint = 0;
	}

	void cPlungerComponent::GetTransform(glm::mat4& transformOut)
	{
		btTransform transform;
		mBody->getMotionState()->getWorldTransform(transform);
		nConvert::ToSimple(transform, transformOut);
	}

	void cPlungerComponent::GetPosition(glm::vec3& positionOut)
	{
		return;
	}

	void cPlungerComponent::SetPosition(const glm::vec3& positionIn)
	{
		return;
	}

	void cPlungerComponent::GetMass(float& massOut)
	{
		return;
	}

	void cPlungerComponent::ApplyForce(const glm::vec3& force)
	{
		return;
	}

	void cPlungerComponent::SetUniqueID(int ID)
	{
		mBody->setUserIndex(ID);
	}

	int cPlungerComponent::GetUniqueID()
	{
		return mBody->getUserIndex();
	}

	bool cPlungerComponent::TriggeredBy(int ID)
	{
		return false;
	}

	void cPlungerComponent::SetMotorVelocity(float targetVelocity)
	{
		mBody->activate(true);		
		mConstraint->setTargetLinMotorVelocity(targetVelocity);
	}

	void cPlungerComponent::SetActive()
	{
		return;
	}
}