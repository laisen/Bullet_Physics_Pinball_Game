#include "cPhysicsFactory.h"
#include "cPhysicsWorld.h"

namespace nPhysics
{
	cPhysicsFactory::cPhysicsFactory()
	{

	}

	cPhysicsFactory::~cPhysicsFactory()
	{

	}

	iPhysicsWorld* cPhysicsFactory::CreateWorld()
	{		
		return new cPhysicsWorld();
	}

	iBallComponent* cPhysicsFactory::CreateBall(const sBallDef& def)
	{
		return new cBallComponent(def);
	}

	iPlaneComponent* cPhysicsFactory::CreatePlane(const sPlaneDef& def)
	{
		return new cPlaneComponent(def);
	}

	iBoxComponent* cPhysicsFactory::CreateBox(const sBoxDef& def)
	{
		return new cBoxComponent(def);
	}

	iCylinderComponent* cPhysicsFactory::CreateCylinder(const sCylinderDef& def)
	{
		return new cCylinderComponent(def);
	}

	iSpinnerComponent* cPhysicsFactory::CreateSpinner(const sSpinnerDef& def)
	{
		return new cSpinnerComponent(def);
	}

	iTriggerComponent* cPhysicsFactory::CreateTrigger(const sTriggerDef& def)
	{
		return new cTriggerComponent(def);
	}

	iFlipperComponent* cPhysicsFactory::CreateFlipper(const sFlipperDef& def)
	{
		return new cFlipperComponent(def);
	}

	iPlungerComponent* cPhysicsFactory::CreatePlunger(const sPlungerDef& def)
	{
		return new cPlungerComponent(def);
	}

}

