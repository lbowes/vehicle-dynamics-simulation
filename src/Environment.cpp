#include "Environment.h"

namespace External {

	Terrain Environment::mTerrain;

	const double
		Environment::mGravityAccel = 9.80665, //m/s^2  - gravitational acceleration at sea level
		Environment::mAirDensity = 1.225;     //kg/m^3 - air density at sea level
}