/* CLASS OVERVIEW
 * - References different sub components of the Car whose state updates depend on user input
 * - A layer between user input and the Car
 * - Where a relatively large amount of input handling takes place
*/

#ifndef CONTROLSYSTEM_H
#define CONTROLSYSTEM_H
#pragma once

#include <vector>
#include <Framework/Input/Input.h>

#include "TorqueGenerator.hpp"

namespace Internal {
	class Brake;
	class Wheel;

	class ControlSystem {
	private:
		Wheel
			*mLeftWheel = nullptr,
			*mRightWheel = nullptr;

		std::vector<Brake*> mBrakes;
		TorqueGenerator* mTorqueGenerator = nullptr;

		const double mSteeringRate = 5000.0;

		double
			mSteeringRatio = 0.0,
			mMaxAbsWheelAngle = 0.0,	     //degs
			mMaxAbsSteeringWheelAngle = 0.0, //degs
			mSteeringWheelAngle = 0.0;       //Inner tyre deflection angle (degs)

		bool mBrakesOn = false;

	public:
		ControlSystem() = default;
		~ControlSystem() = default;

		void update(double wheelBase, double frontAxleTrack);
		void handleInput(double dt);
		void attachWheels(Wheel* left, Wheel* right);
		void setMaxAbsWheelAngle(double maxAbsAngle);

		inline double getSteeringWheelAngle() const { return mSteeringWheelAngle; }
		inline void setSteeringRatio(double newRatio) { mSteeringRatio = newRatio; }
		inline void attachTorqueGenerator(TorqueGenerator* torqueGenerator) { mTorqueGenerator = torqueGenerator; }
		inline void attachBrakes(std::vector<Brake*> brakes) { mBrakes = brakes; }
		inline bool brakesOn() const { return mBrakesOn; }

	private:
		void updateSteeringAngle(double wheelBase, double frontAxleTrack);
		void handleSteeringInput(double dt);
		void handleSpeedInput();

	};
}

#endif
