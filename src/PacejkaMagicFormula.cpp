#include "PacejkaMagicFormula.h"
#include "Tyre.h"

namespace Internal {

	float                                 //default values
		PacejkaMagicFormula::b0 = 1.4,    //1.65
		PacejkaMagicFormula::b1 = 80.0,   //0
		PacejkaMagicFormula::b2 = 1700.0, //1100
		PacejkaMagicFormula::b3 = 0.0,    //0
		PacejkaMagicFormula::b4 = 300.0,  //300
		PacejkaMagicFormula::b5 = 0.0,    //0
		PacejkaMagicFormula::b6 = 0.0,    //0
		PacejkaMagicFormula::b7 = 0.0,    //0
		PacejkaMagicFormula::b8 = -2.0,   //-2
		PacejkaMagicFormula::b9 = 0.0,    //0
		PacejkaMagicFormula::b10 = 0.0,   //0
		PacejkaMagicFormula::b11 = 0.0,   //0
		PacejkaMagicFormula::b12 = 0.0,   //0
		PacejkaMagicFormula::b13 = 0.0;   //0

	float			                    //default values
		PacejkaMagicFormula::a0 = 1.4,	//1.4
		PacejkaMagicFormula::a1 = 80.0,	//0
		PacejkaMagicFormula::a2 = 1700, //1100
		PacejkaMagicFormula::a3 = 1100, //1100
		PacejkaMagicFormula::a4 = 50.0,	//10
		PacejkaMagicFormula::a5 = 0.0,	//0
		PacejkaMagicFormula::a6 = 0.0,	//0
		PacejkaMagicFormula::a7 = -2.0,	//-2
		PacejkaMagicFormula::a8 = 0.0,	//0
		PacejkaMagicFormula::a9 = 0.0,	//0
		PacejkaMagicFormula::a10 = 0.0,	//0
		PacejkaMagicFormula::a11 = 0.0,	//0
		PacejkaMagicFormula::a12 = 0.0,	//0
		PacejkaMagicFormula::a13 = 0.0,	//0
		PacejkaMagicFormula::a14 = 0.0, //0
		PacejkaMagicFormula::a15 = 0.0, //0
		PacejkaMagicFormula::a16 = 0.0, //0
		PacejkaMagicFormula::a17 = 0.0; //0

	PacejkaMagicFormula::PacejkaMagicFormula()
		/* Called during Tyre::Tyre
		*/
	{
		setToDriftingTyreParams();
	}

	void PacejkaMagicFormula::updateForces(double verticalLoad_N, double slipPercent_0_to_100, double slipAngle_degs, double camberAngle_degs)
		/* Called by Tyre::update
		*/
	{
		//http://www.edy.es/dev/docs/pacejka-94-parameters-explained-a-comprehensive-guide/

		updateLongitudinalForce(verticalLoad_N / 1000.0, slipPercent_0_to_100);
		updateLateralForce(verticalLoad_N / 1000.0, slipAngle_degs, camberAngle_degs);
	}

	void PacejkaMagicFormula::setToRoadTyreParams()
		/* Called by UILayer::tyreParameters
		*/
	{
		//Longitudinal
		b0 = 1.4,
		b1 = 80.0,
		b2 = 1700.0,
		b3 = 0.0,
		b4 = 140.0,
		b5 = 0.67,
		b6 = 0.0,
		b7 = 0.273,
		b8 = -2.0,
		b9 = 0.698,
		b10 = 0.0,
		b11 = 0.0,
		b12 = 0.0,
		b13 = 0.0;

		//Lateral
		a0 = 1.4,
		a1 = 0.0,
		a2 = 1700.0,
		a3 = 2000.0,
		a4 = 18.5,
		a5 = 0.0,
		a6 = 0.0,
		a7 = -2.0,
		a8 = 1.0,
		a9 = 0.0,
		a10 = 0.0,
		a11 = 0.0,
		a12 = 0.0,
		a13 = 0.0,
		a14 = 0.0,
		a15 = 0.0,
		a16 = 0.0,
		a17 = 0.0;
	}

	void PacejkaMagicFormula::setToDriftingTyreParams()
		/* Called by
		 * - PacejkaMagicFormula::PacejkaMagicFormula
		 * - UILayer::tyreParameters
		*/
	{
		//Longitudinal
		b0 = 1.4,
		b1 = 80.0,
		b2 = 1700.0,
		b3 = 0.0,
		b4 = 300.0,
		b5 = 1.0,
		b6 = 0.0,
		b7 = 0.0,
		b8 = -2.0,
		b9 = 0.0,
		b10 = 0.0,
		b11 = 0.0,
		b12 = 0.0,
		b13 = 0.0;

		//Lateral
		a0 = 1.4,
		a1 = 80.0,
		a2 = 1700,
		a3 = 1100,
		a4 = 50.0,
		a5 = 0.0,
		a6 = 0.0,
		a7 = -2.0,
		a8 = 0.0,
		a9 = 0.0,
		a10 = 0.0,
		a11 = 0.0,
		a12 = 0.0,
		a13 = 0.0,
		a14 = 0.0,
		a15 = 0.0,
		a16 = 0.0,
		a17 = 0.0;
	}

	void PacejkaMagicFormula::updateLongitudinalForce(double load_kN, double slipAsPercent)
		/* Called by PacejkaMagicFormula::updateForces
		*/
	{
		if (load_kN == 0.0 || abs(slipAsPercent == 0.0)) {
			mLongitudinalForce = 0.0;
			return;
		}

		double loadSquared_kN = pow(load_kN, 2.0);

		C = b0;
		D = load_kN * (b1 * load_kN + b2);
		BCD = (b0 * loadSquared_kN + b4 * load_kN) * exp(-b5 * load_kN);
		B = BCD / (C * D);
		E = (b6 * loadSquared_kN + b7 * load_kN + b8) * (1.0 - b13 * sign(slipAsPercent + H));
		H = b9 * load_kN + b10;
		V = b11 * load_kN + b12;
		Bx1 = B * (slipAsPercent + H);

		mLongitudinalForce = D * sin(C * atan(Bx1 - E * (Bx1 - atan(Bx1)))) + V;
	}

	void PacejkaMagicFormula::updateLateralForce(double load_kN, double slipAngle_degs, double camberAngle_degs)
		/* Called by PacejkaMagicFormula::updateForces
		*/
	{
		if (load_kN == 0.0 || abs(slipAngle_degs == 0.0)) {
			mLateralForce = 0.0;
			return;
		}

		C = a0;
		D = load_kN * (a1 * load_kN + a2) * (1.0 - a15 * pow(camberAngle_degs, 2.0));
		BCD = a3 * sin(atan(load_kN / a4) * 2.0) * (1.0 - a5 * abs(camberAngle_degs));
		B = BCD / (C * D);
		E = (a6 * load_kN + a7) * (1.0 - (a16 * camberAngle_degs + a17) * sign(slipAngle_degs + H));
		H = a8 * load_kN + a9 + a10 * camberAngle_degs;
		V = a11 * load_kN + a12 + (a13 * load_kN + a14) * camberAngle_degs * load_kN;
		Bx1 = B * (slipAngle_degs + H);

		mLateralForce = D * sin(C * atan(Bx1 - E * (Bx1 - atan(Bx1)))) + V;
	}

}