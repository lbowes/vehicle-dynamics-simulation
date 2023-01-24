/* CLASS OVERVIEW
 * - Encapsulates a static Terrain instance
 * - Contains purely static data
 * - Can be easily accessed by #including this file anywhere  
*/

#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H
#pragma once

#include <glm/glm/vec2.hpp>
#include <glm/glm/vec3.hpp>
#include <glm/glm/geometric.hpp>

#include "Terrain.h"

namespace External {
	class Environment {
	public:
		static Terrain mTerrain;
		
		static const double
			mGravityAccel,
			mAirDensity;

	};
}

#endif
