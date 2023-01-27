#include "Tyre.h"

namespace Internal {

	Tyre::Tyre(double wheelRimRadius) :
		/* Called by Wheel::Wheel
		*/
		mAxialInertia((glm::pi<double>() * mRubberDensity * mTreadWidth) / 2.0 * (pow(wheelRimRadius + mDepth, 4) - pow(wheelRimRadius, 4)))
	{ }

	void Tyre::update(glm::dvec2 wheelVelocity_wheel, double verticalLoad, double camberAngle, double wheelRimRadius, double wheelRotSpeed_radPerSec)
		/* Called by Wheel::update
		*/
	{
		double effectiveRollingRadius = wheelRimRadius + mDepth;
		mSlip.update(wheelVelocity_wheel, wheelRotSpeed_radPerSec, effectiveRollingRadius);
		mRollingSpeed = wheelRotSpeed_radPerSec * effectiveRollingRadius;

		mRollResistForce_long = wheelVelocity_wheel.y > 0.0 ? -mRollResistCoefficient * verticalLoad : wheelVelocity_wheel.y < 0.0 ? mRollResistCoefficient * verticalLoad : 0.0;

		mForceCalculator.updateForces(verticalLoad, mSlip.getLongitudinal() * 100.0, mSlip.getAngle_degs(), camberAngle);
		mTotalForce_wheel.x = mForceCalculator.getLateralForce();
		mTotalForce_wheel.y = mForceCalculator.getLongitudinalForce() + mRollResistForce_long;
	}

}