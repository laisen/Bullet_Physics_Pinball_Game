#pragma once
#include <iPhysicsWorld.h>
#include "btBulletDynamicsCommon.h"
#include "BulletCollision/CollisionDispatch/btGhostObject.h"
#include "cBallComponent.h"
#include "cPlaneComponent.h"
#include "cBoxComponent.h"
#include "cCylinderComponent.h"
#include "cSpinnerComponent.h"
#include "cTriggerComponent.h"
#include "cFlipperComponent.h"
#include "cPlungerComponent.h"

namespace nPhysics
{
	class cPhysicsWorld : public iPhysicsWorld
	{
	public: 
		cPhysicsWorld();
		virtual ~cPhysicsWorld();

		virtual void Update(float dt) override;
		virtual bool AddComponent(iPhysicsComponent* component) override;
		virtual bool RemoveComponent(iPhysicsComponent* component) override;

	private:		

		bool AddRigidBodies(cBallComponent* component);
		bool AddRigidBodies(cPlaneComponent* component);
		bool AddRigidBodies(cBoxComponent* component);
		bool AddRigidBodies(cCylinderComponent* component);
		bool AddRigidBodies(cSpinnerComponent* component);
		bool AddRigidBodies(cTriggerComponent* component);
		bool AddRigidBodies(cFlipperComponent* component);
		bool AddRigidBodies(cPlungerComponent* component);

		bool RemoveRigidBodies(cBallComponent* component);
		bool RemoveRigidBodies(cPlaneComponent* component);
		bool RemoveRigidBodies(cBoxComponent* component);
		bool RemoveRigidBodies(cCylinderComponent* component);
		bool RemoveRigidBodies(cSpinnerComponent* component);
		bool RemoveRigidBodies(cTriggerComponent* component);
		bool RemoveRigidBodies(cFlipperComponent* component);
		bool RemoveRigidBodies(cPlungerComponent* component);

		btDefaultCollisionConfiguration* mCollisionConfig;
		btCollisionDispatcher* mDispatcher;
		btBroadphaseInterface* mOverlappingPairCache;
		btGhostPairCallback* mGhostPairCallback;
		btSequentialImpulseConstraintSolver* mSolver;
		btDiscreteDynamicsWorld* mDynamicsWorld;

		cPhysicsWorld(const cPhysicsWorld& other) = delete;
		cPhysicsWorld& operator=(const cPhysicsWorld& other) = delete;
	};


}


