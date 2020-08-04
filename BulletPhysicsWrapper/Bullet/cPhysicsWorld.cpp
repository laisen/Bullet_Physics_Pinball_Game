#include "cPhysicsWorld.h"

namespace nPhysics
{
	cPhysicsWorld::cPhysicsWorld()
	{
		mCollisionConfig = new btDefaultCollisionConfiguration();

		mDispatcher = new btCollisionDispatcher(mCollisionConfig);

		mOverlappingPairCache = new btDbvtBroadphase();
		mGhostPairCallback = new btGhostPairCallback();
		mOverlappingPairCache->getOverlappingPairCache()->setInternalGhostPairCallback(mGhostPairCallback);

		mSolver = new btSequentialImpulseConstraintSolver();

		mDynamicsWorld = new btDiscreteDynamicsWorld(mDispatcher, mOverlappingPairCache, mSolver, mCollisionConfig);

		mDynamicsWorld->setGravity(btVector3(0, -10, 7));		
	}

	cPhysicsWorld::~cPhysicsWorld()
	{		
		delete mDynamicsWorld;
		delete mSolver;
		delete mGhostPairCallback;
		delete mOverlappingPairCache;
		delete mDispatcher;
		delete mCollisionConfig;
	}

	void cPhysicsWorld::Update(float dt)
	{
		mDynamicsWorld->stepSimulation(dt, 10);
	}

	bool cPhysicsWorld::AddComponent(iPhysicsComponent* component)
	{
		if (!component)	return false;		// nothing to add

		switch (component->GetComponentType())
		{
		case eComponentType::ball:
			return AddRigidBodies(dynamic_cast<cBallComponent*>(component));
		case eComponentType::plane:
			return AddRigidBodies(dynamic_cast<cPlaneComponent*>(component));
		case eComponentType::box:
			return AddRigidBodies(dynamic_cast<cBoxComponent*>(component));
		case eComponentType::cylinder:
			return AddRigidBodies(dynamic_cast<cCylinderComponent*>(component));
		case eComponentType::spinner:
			return AddRigidBodies(dynamic_cast<cSpinnerComponent*>(component));
		case eComponentType::trigger:
			return AddRigidBodies(dynamic_cast<cTriggerComponent*>(component));
		case eComponentType::flipper:
			return AddRigidBodies(dynamic_cast<cFlipperComponent*>(component));
		case eComponentType::plunger:
			return AddRigidBodies(dynamic_cast<cPlungerComponent*>(component));
		default:
			break;
		}

		return false;
	}

	bool cPhysicsWorld::RemoveComponent(iPhysicsComponent* component)
	{
		// Null check
		if (!component)	return false;

		switch (component->GetComponentType())
		{
		case eComponentType::ball:			
			if (RemoveRigidBodies(dynamic_cast<cBallComponent*>(component)))
			{
				delete component;
				return true;
			}
			else
			{
				return false;
			}
		case eComponentType::plane:			
			if (RemoveRigidBodies(dynamic_cast<cPlaneComponent*>(component)))
			{
				delete component;
				return true;
			}
			else
			{
				return false;
			}
		case eComponentType::box:			
			if (RemoveRigidBodies(dynamic_cast<cBoxComponent*>(component)))
			{
				delete component;
				return true;
			}
			else
			{
				return false;
			}
		case eComponentType::cylinder:
			if (RemoveRigidBodies(dynamic_cast<cCylinderComponent*>(component)))
			{
				delete component;
				return true;
			}
			else
			{
				return false;
			}
		case eComponentType::spinner:
			if (RemoveRigidBodies(dynamic_cast<cSpinnerComponent*>(component)))
			{
				delete component;
				return true;
			}
			else
			{
				return false;
			}
		case eComponentType::trigger:
			if (RemoveRigidBodies(dynamic_cast<cTriggerComponent*>(component)))
			{
				delete component;
				return true;
			}
			else
			{
				return false;
			}
		case eComponentType::flipper:
			if (RemoveRigidBodies(dynamic_cast<cFlipperComponent*>(component)))
			{
				delete component;
				return true;
			}
			else
			{
				return false;
			}
		case eComponentType::plunger:
			if (RemoveRigidBodies(dynamic_cast<cPlungerComponent*>(component)))
			{
				delete component;
				return true;
			}
			else
			{
				return false;
			}
		default:
			break;
		}				

		return false;
	}

	bool cPhysicsWorld::AddRigidBodies(cBallComponent* component)
	{
		if (!component)	return false;		
		mDynamicsWorld->addRigidBody(component->mBody, 64, 128 + 256 + 512);		// Bitwise filtering collisions masks
		return true;
	}

	bool cPhysicsWorld::AddRigidBodies(cPlaneComponent* component)
	{
		if (!component)	return false;		
		mDynamicsWorld->addRigidBody(component->mBody, 128, 64);
		return true;
	}

	bool cPhysicsWorld::AddRigidBodies(cBoxComponent* component)
	{
		if (!component)	return false;		
		mDynamicsWorld->addRigidBody(component->mBody, 128, 64);
		return true;
	}

	bool cPhysicsWorld::AddRigidBodies(cCylinderComponent* component)
	{
		if (!component)	return false;
		mDynamicsWorld->addRigidBody(component->mBody, 128, 64);		
		mDynamicsWorld->addCollisionObject(component->mGhostObject, 128, 64);
		return true;
	}

	bool cPhysicsWorld::AddRigidBodies(cSpinnerComponent* component)
	{
		if (!component)	return false;		
		mDynamicsWorld->addRigidBody(component->mBody, 256, 64 + 512);
		mDynamicsWorld->addConstraint(component->mConstraint);
		return true;
	}

	bool cPhysicsWorld::AddRigidBodies(cTriggerComponent* component)
	{
		if (!component)	return false;		
		mDynamicsWorld->addCollisionObject(component->mGhostObject, 512, 64 + 256);		
		return true;
	}

	bool cPhysicsWorld::AddRigidBodies(cFlipperComponent* component)
	{
		if (!component)	return false;
		mDynamicsWorld->addRigidBody(component->mBody, 128, 64);
		mDynamicsWorld->addConstraint(component->mConstraint);
		return true;
	}

	bool cPhysicsWorld::AddRigidBodies(cPlungerComponent* component)
	{
		if (!component)	return false;
		mDynamicsWorld->addRigidBody(component->mBody, 128, 64);
		mDynamicsWorld->addConstraint(component->mConstraint);
		return true;
	}

	bool cPhysicsWorld::RemoveRigidBodies(cBallComponent* component)
	{
		if (!component)	return false;
		mDynamicsWorld->removeRigidBody(component->mBody);
		return true;
	}

	bool cPhysicsWorld::RemoveRigidBodies(cPlaneComponent* component)
	{
		if (!component)	return false;
		mDynamicsWorld->removeRigidBody(component->mBody);
		return true;
	}

	bool cPhysicsWorld::RemoveRigidBodies(cBoxComponent* component)
	{
		if (!component)	return false;
		mDynamicsWorld->removeRigidBody(component->mBody);
		return true;
	}

	bool cPhysicsWorld::RemoveRigidBodies(cCylinderComponent* component)
	{
		if (!component)	return false;
		mDynamicsWorld->removeRigidBody(component->mBody);
		mDynamicsWorld->removeCollisionObject(component->mGhostObject);
		return true;
	}

	bool cPhysicsWorld::RemoveRigidBodies(cSpinnerComponent* component)
	{
		if (!component)	return false;
		mDynamicsWorld->removeRigidBody(component->mBody);
		mDynamicsWorld->removeConstraint(component->mConstraint);
		return true;
	}

	bool cPhysicsWorld::RemoveRigidBodies(cTriggerComponent* component)
	{
		if (!component)	return false;
		mDynamicsWorld->removeCollisionObject(component->mGhostObject);
		return true;
	}

	bool cPhysicsWorld::RemoveRigidBodies(cFlipperComponent* component)
	{
		if (!component)	return false;
		mDynamicsWorld->removeRigidBody(component->mBody);
		mDynamicsWorld->removeConstraint(component->mConstraint);
		return true;
	}

	bool cPhysicsWorld::RemoveRigidBodies(cPlungerComponent* component)
	{
		if (!component)	return false;
		mDynamicsWorld->removeRigidBody(component->mBody);
		mDynamicsWorld->removeConstraint(component->mConstraint);
		return true;
	}
}


