/* CLASS OVERVIEW
 * - Encapsulates and updates all per-wheel components
 * - Provides a level of abstraction around the Wheel
 * - Manages the vertical motion of the Wheel in car space (with access to Suspension)
 */

#ifndef WHEELINTERFACE_H
#define WHEELINTERFACE_H
#pragma once

#include <algorithm>
#include <Framework/Physics/Spring.hpp>
#include <Framework/Physics/State.hpp>

#include "Wheel.h"
#include "Axle.hpp"
#include "Brake.hpp"
#include "Suspension.hpp"

namespace Internal {
	class WheelInterface {
	private:
		Axle& mConnectedAxle;
		Wheel mWheel;
		Brake mBrake;
		Suspension mSuspension;

		glm::dvec3
			mPosition_car,
			mPosition_world,
			mVelocity_world;

		double mLoad = 0.0; //N

		bool mCollisionRegistered = false;

	public:
		WheelInterface(Axle& connectedAxle);
		~WheelInterface() = default;

		void update(Framework::Physics::State& carState, double load, double dt);
		void setPosition_car(glm::dvec3 newPosition_car);
		void reset();

		inline Wheel& getWheel() { return mWheel; }
		inline Brake& getBrake() { return mBrake; }
		inline Suspension& getSuspension() { return mSuspension; }
		inline glm::dvec3 getPosition_car() const { return mPosition_car; }
		inline glm::dvec3 getPosition_world() const { return mPosition_world; }
		inline glm::dvec3 getVelocity_world() const { return mVelocity_world; }
		inline double getLoad() const { return mLoad; }
		inline bool collisionRegistered() const { return mCollisionRegistered; }

	private:
		void updateWheel(glm::dmat4 carToWorldRotation_car, glm::dvec3 terrainNormalUnderWheel, double terrainOverlap, double dt);

	};
}

#endif
