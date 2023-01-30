/* CLASS(ES) OVERVIEW
 * - Slip calculates, updates and provides access to longitudinal and lateral slip values
 * - Slip can be passed around as a single object
 * - Tyre calculates, updates and provides access to two force components
 * - Tyre encapsulates a PacejkaMagicFormula and Slip
*/

#ifndef TYRE_H
#define TYRE_H
#pragma once

#include <algorithm>
#include <glm/glm/vec2.hpp>
#include <glm/glm/trigonometric.hpp>
#include <glm/glm/gtc/constants.hpp>

#include "PacejkaMagicFormula.h"

namespace Internal {
	class Wheel;

	struct Slip {
	private:
		double
			mLongSlipSpeed = 0.0,     //m/s
			mLateralSlipSpeed = 0.0,  //m/s
			mLongitudinal = 0.0,      //0.0 -> 1.0, for longitudinal calculations
			mAngle_degs = 0.0;        //Degrees, for lateral calculations

	public:
		void update(glm::dvec2 tyreVel_lat_long, double tyreRotSpeed_radPerSec, double effectiveRollingRadius)
			/* Called by Tyre::update
			 * Recalculates longitudinal and lateral slip
			*/
		{
			//page 5/6 of http://mech.unibg.it/~lorenzi/VD&S/Matlab/Tire/tire_models_pac2002.pdf

			//Longitudinal
			mLongSlipSpeed = tyreVel_lat_long.y - tyreRotSpeed_radPerSec * effectiveRollingRadius;
			mLongitudinal = tyreVel_lat_long.y == 0.0 ? 0.0 : mLongSlipSpeed;

			//Lateral
			mLateralSlipSpeed = tyreVel_lat_long.x;
			mAngle_degs = tyreVel_lat_long.y == 0.0 ? (tyreVel_lat_long.x > 0.0 ? -90.0 : 90.0) : glm::degrees(atan(mLateralSlipSpeed / abs(tyreVel_lat_long.y)));
		}

		inline void reset() { mLongitudinal = mAngle_degs = 0.0; }
		inline bool isGenerated() const { return mAngle_degs != 0.0 || mLongitudinal != 0.0; }

		inline double getLongitudinal() const { return mLongitudinal; }
		inline double getAngle_degs() const { return mAngle_degs; }

	};

	class Tyre {
		friend class Wheel;
	private:
		Slip mSlip;

		glm::dvec2 mTotalForce_wheel;        //Wheel-space, x = lateral, y = longitudinal

		PacejkaMagicFormula mForceCalculator;

		const double
			mRubberDensity = 650.0,          //kg/m^3
			mDepth = 0.06544,                //m
			mTreadWidth = 0.2,		         //m
			mRollResistCoefficient = 0.0125, //dimensionless
			mAxialInertia = 0.0;			 //kg/m^2

		double
			mRollResistForce_long = 0.0,     //N
			mRollingSpeed = 0.0;			 //m/s

	public:
		Tyre(double wheelRimRadius);
		~Tyre() = default;

		void update(glm::dvec2 wheelVelocity_wheel, double verticalLoad, double camberAngle, double wheelRimRadius, double wheelRotSpeed_radPerSec);

		inline glm::dvec2 getTotalForce_wheel() const { return mTotalForce_wheel; }
		inline double getDepth() const { return mDepth; }
		inline double getAxialInertia() const { return mAxialInertia; }
		inline Slip getSlip() const { return mSlip; }

	};
}

#endif
