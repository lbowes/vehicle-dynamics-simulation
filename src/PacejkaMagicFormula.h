/* CLASS OVERVIEW
 * - Implementation of Pacejka's Magic Formula for calculating tyre force (lon and lat) from slip (ratio and angle),
 *   a detailed description of which can be found at http://www.edy.es/dev/docs/pacejka-94-parameters-explained-a-comprehensive-guide/
*/

#ifndef PACEJKAMAGICFORMULA_H
#define PACEJKAMAGICFORMULA_H
#pragma once

namespace Internal {
	struct Slip;

	class PacejkaMagicFormula {
	public:
		//Longitudinal parameters
		static float b0, b1, b2, b3, b4, b5, b6, b7, b8, b9, b10, b11, b12, b13;

		//Lateral parameters
		static float a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17;

	private:
		float
			C = 0.0,
			D = 0.0,
			BCD = 0.0,
			B = 0.0,
			E = 0.0,
			H = 0.0,
			V = 0.0,
			Bx1 = 0.0;

		double
			mLongitudinalForce = 0.0,
			mLateralForce = 0.0;

	public:
		PacejkaMagicFormula();
		~PacejkaMagicFormula() = default;

		void updateForces(double verticalLoad_N, double slipPercent0_to_100, double slipAngle_degs, double camberAngle_degs);
		static void setToRoadTyreParams();
		static void setToDriftingTyreParams();

		inline double getLongitudinalForce() const { return mLongitudinalForce; }
		inline double getLateralForce() const { return mLateralForce; }

	private:
		void updateLongitudinalForce(double load_kN, double slipAsPercent);
		void updateLateralForce(double load_kN, double slipAngle_degs, double camberAngle_degs);

		//Implementation of the mathematical sign() function
		inline int sign(double val) { return (0.0 < val) - (val < 0.0); }

	};
}

#endif
