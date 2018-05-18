/* CLASS OVERVIEW
 * - The main simulated object in the application and the root of multiple object hierarchies
 * - Referenced by the visual side of the application
 * - Responsible for updating all member objects, and then updating its own state using calculated forces and torques
*/

#ifndef CAR_H
#define CAR_H
#pragma once

#include <memory>
#include <Framework/Physics/RigidBody.h>
#include <Framework/Input/Input.h>

#include "Environment.h"
#include "WheelSystem.h"
#include "ControlSystem.h"

namespace Internal {
	class Car : public Framework::Physics::RigidBody {
	protected:
		ControlSystem mControlSystem;
		WheelSystem mWheelSystem;
		std::unique_ptr<TorqueGenerator> mTorqueGenerator;

		glm::dvec3
			mAerodynamicDrag_world,	//N
			mTotalForce_world,		//N
			mTotalTorque_world;		//Nm

		double
			mFrontalArea = 2.63,	 //m^2
			mDragCoefficient = 1.3,	 //(dimensionless)
			mWidth = 0.0,            //m
			mLength = 0.0;           //m

	public:
		Car();
		~Car() = default;

		void update(double t, double dt);
		void checkInput(double dt);
		void resetToTrackPosition();

		inline Framework::Physics::State& getState() { return mState; }
		inline WheelSystem& getWheelSystem() { return mWheelSystem; }
		inline ControlSystem& getControlSystem() { return mControlSystem; }
		inline TorqueGenerator& getTorqueGenerator() { return *mTorqueGenerator.get(); }
		inline glm::dvec3 getAeroDrag_world() { return mAerodynamicDrag_world; }

	private:
		void updateTotalForce_world();
		glm::dvec3 getForce_world(Framework::Physics::State& state, double t);
		void updateTotalTorque_world();
		glm::dvec3 getTorque_world(Framework::Physics::State& state, double t);
		void positionConstraints();
		void assemble();

	};
}

#endif
