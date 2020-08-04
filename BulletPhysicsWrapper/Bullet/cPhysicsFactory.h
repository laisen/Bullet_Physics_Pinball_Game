#pragma once
#include <iPhysicsFactory.h>

namespace nPhysics
{
	class cPhysicsFactory : public iPhysicsFactory
	{
	public:
		cPhysicsFactory();
		virtual ~cPhysicsFactory();

		virtual iPhysicsWorld* CreateWorld() override;		
		virtual iBallComponent* CreateBall(const sBallDef& def) override;
		virtual iPlaneComponent* CreatePlane(const sPlaneDef& def) override;
		virtual iBoxComponent* CreateBox(const sBoxDef& def) override;
		virtual iCylinderComponent* CreateCylinder(const sCylinderDef& def) override;
		virtual iSpinnerComponent* CreateSpinner(const sSpinnerDef& def) override;
		virtual iTriggerComponent* CreateTrigger(const sTriggerDef& def) override;
		virtual iFlipperComponent* CreateFlipper(const sFlipperDef& def) override;
		virtual iPlungerComponent* CreatePlunger(const sPlungerDef& def) override;

	private:
		cPhysicsFactory(const cPhysicsFactory& other) = delete;
		cPhysicsFactory& operator=(const cPhysicsFactory& other) = delete;
	};
}


