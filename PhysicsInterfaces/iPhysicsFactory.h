#pragma once
#include "iBallComponent.h"
#include "iPlaneComponent.h"
#include "iBoxComponent.h"
#include "iCylinderComponent.h"
#include "iSpinnerComponent.h"
#include "iTriggerComponent.h"
#include "iFlipperComponent.h"
#include "iPlungerComponent.h"
#include "iPhysicsWorld.h"

namespace nPhysics
{
	class iPhysicsFactory
	{
	public:
		virtual ~iPhysicsFactory() {}
		virtual iPhysicsWorld* CreateWorld() = 0;
		// components
		virtual iBallComponent* CreateBall(const sBallDef& def) = 0;
		virtual iPlaneComponent* CreatePlane(const sPlaneDef& def) = 0;
		virtual iBoxComponent* CreateBox(const sBoxDef& def) = 0;
		virtual iCylinderComponent* CreateCylinder(const sCylinderDef& def) = 0;
		virtual iSpinnerComponent* CreateSpinner(const sSpinnerDef& def) = 0;
		virtual iTriggerComponent* CreateTrigger(const sTriggerDef& def) = 0;
		virtual iFlipperComponent* CreateFlipper(const sFlipperDef& def) = 0;
		virtual iPlungerComponent* CreatePlunger(const sPlungerDef& def) = 0;
	};
}