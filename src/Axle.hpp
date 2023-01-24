/* CLASS OVERVIEW
 * - Responsible for calculating, storing and providing access to a total torque value
*/

#ifndef AXLE_H
#define AXLE_H
#pragma once

#include <Framework/Physics/RigidBody.h>

#include "TorqueGenerator.hpp"

namespace Internal {
	class Axle {
	private:
		double
			mLength = 0.0,                           //m, equivalent to the (front or rear) 'track' of the car
			mLongDisplacement_car = 0.0,             //m, longitudinal displacement of axle from car's origin (positive for front, negative for rear)
			mTransferredTorque = 0.0,                //Nm, sign represents direction
			mRPM = 0.0;                              //Rotations per minute

		TorqueGenerator* mTorqueGenerator = nullptr; //Should remain a null pointer for lazy axles

	public:
		Axle() = default;
		~Axle() = default;

		void update(double RPM, double totalCounterTorque) 
			/* Called by WheelSystem::updateAxles
			 * Calculates a total torque value based on counter torque passed in and drive torque
			*/
		{
			//Reset the torque before recalculating it again
			mTransferredTorque = 0.0;

			//Add drive torque from the torque generator (if this isn't a lazy axle)
			if (mTorqueGenerator) {
				mTorqueGenerator->updateRPM(RPM);
				mTransferredTorque -= mTorqueGenerator->getOutputTorque();
			}

			//Add resiting torque from wheel traction and brakes
			mTransferredTorque += totalCounterTorque;
		}
		
		inline double getTransferredTorque() const { return mTransferredTorque; }
		inline double getLongDisplacement_car() const { return mLongDisplacement_car; }
		inline double getLength() const { return mLength; }
		inline void attachTorqueGenerator(TorqueGenerator* newTorqueGenerator) { mTorqueGenerator = newTorqueGenerator; }
		inline void setLongDisplacement_car(double longDisplacement_car) { mLongDisplacement_car = longDisplacement_car; }
		inline void setLength(double length) { mLength = length; }

	};
}

#endif
