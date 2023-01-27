/* CLASS OVERVIEW
 * - Encapsulates everything relating to the Wheels
 * - Provides the total force and torque produced by the Tyres and Suspension
 * - Owns Axles and connects them to WheelInterfaces
 */

#ifndef WHEELSYSTEM_H
#define WHEELSYSTEM_H
#pragma once

#include <vector>
#include <algorithm>
#include <glm/glm/vec3.hpp>

#include "WheelInterface.h"

namespace Internal {
	class ControlSystem;

	class WheelSystem {
		friend class Axle;
	public:
		enum AxlePos : unsigned char { FRONT, REAR };
		enum Side : unsigned char { LEFT, RIGHT };

	private:
		const double
			mWheelBase = 2.96,          //m
			mTrack_front = 1.66116,     //m
			mTrack_rear = 1.69926,      //m
			mWheelHeight = 0.13;        //m

		double mMinTurningRadius = 0.0; //m

		glm::dvec3
			mTotalForce_world,
			mTotalTorque_world;

		std::vector<WheelInterface> mWheelInterfaces;

		Axle
			mFrontAxle,
			mRearAxle;

	public:
		WheelSystem();
		~WheelSystem() = default;

		void update(Framework::Physics::State& carState, glm::dvec3 carAcceleration_car, double dt);
		void bindControlSystem(ControlSystem& controlSystem);
		void reset();

		inline double getWheelBase() const { return mWheelBase; }
		inline WheelInterface& getWheelInterface(AxlePos pos, Side side) { return mWheelInterfaces[pos * 2 + side]; }
		inline WheelInterface* getWheelInterface(unsigned char index) { return (index >= 0 && index < mWheelInterfaces.size()) ? &mWheelInterfaces[index] : (WheelInterface*)nullptr; }
		inline WheelInterface* operator[](unsigned char index) { return (index >= 0 && index < mWheelInterfaces.size()) ? &mWheelInterfaces[index] : (WheelInterface*)nullptr; }
		inline std::vector<WheelInterface>& getAllWheelInterfaces() { return mWheelInterfaces; }
		inline Axle& getAxle(AxlePos pos) { return pos == AxlePos::FRONT ? mFrontAxle : mRearAxle; }
		inline glm::dvec3 getTotalForce_world() const { return mTotalForce_world; }
		inline glm::dvec3 getTotalTorque_world() const { return mTotalTorque_world; }

		inline void setMinimumTurnRadius(double minTurnRadius) { mMinTurningRadius = minTurnRadius; }

	private:
		void positionWheelInterfaces();
		void updateAxles();
		void updateAllWheelInterfaces(Framework::Physics::State& carState, glm::dvec3 carAcceleration_car, double dt);
		double recalcLoad(AxlePos position, Side side, Framework::Physics::Mass& carMass_car, glm::dvec3 carAcceleration_car, double carCMHeightAboveGround);
		double recalcCarCmHeightAboveGround(Framework::Physics::State& carState);
		void updateTotalForce_world();
		void updateTotalTorque_world(glm::dvec3 carPosition_world, glm::dmat4 carToWorldTransform_car);

		inline AxlePos calcAxleFromIndex(unsigned char index) const { return index < 2 ? FRONT : REAR; }
		inline Side calcSideFromIndex(unsigned char index) const { return index % 2 == 0 ? LEFT : RIGHT; }

	};
}


#endif
