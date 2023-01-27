/* CLASS OVERVIEW
 * Produces a force using a Spring object, that can be updated and accessed
 */

#ifndef SUSPENSION_H
#define SUSPENSION_H
#pragma once

#include <Framework/Physics/Spring.hpp>

namespace Internal {
	class Suspension {
	private:
		const double
			mSpringConstant = 49000.0,
			mDamping = 3000.0;

		Framework::Physics::Spring mSpring;

		glm::dvec3 mForce_world;

	public:
		Suspension() :
			/* Called during WheelInterface::WheelInterface
			*/
			mSpring(mSpringConstant, 0.0, mDamping)
		{ }

		~Suspension() = default;

		void update(double verticalRoadVelocity_car, double terrainOverlap, glm::dvec3 lineOfAction_world)
			/* Called by WheelInterface::update
			*/
		{
			mSpring.update(-terrainOverlap, terrainOverlap ? verticalRoadVelocity_car : 0.0);
			mForce_world = terrainOverlap ? normalize(lineOfAction_world) * mSpring.getForce() : glm::dvec3(0.0);
		}

		inline void neutralise() { mSpring.update(mSpring.getRestLength(), 0.0); }
		inline glm::dvec3 getForce_world() const { return mForce_world; }
		inline double getLength() const { return mSpring.getCurrentLength(); }
		inline Framework::Physics::Spring& getSpring() { return mSpring; }

	};
}

#endif
