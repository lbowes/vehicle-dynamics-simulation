/* CLASS OVERVIEW
 * Produces a torque that can be updated and accessed
*/

#ifndef BRAKE_H
#define BRAKE_H
#pragma once

namespace Internal {
	class Brake {
	private:
		const double mDistToWheelCentre = 0.16;  //m
		
		double
			mCompressionForce = 0.0,  //Nm
			mTorqueMagnitude = 0.0,   //Nm
			mTravelPercentage = 0.0;  //0.0 = released, 1.0 = full contact, *directionless*

	public:
		Brake() = default;
		~Brake() = default;

		void update() 
			/* Called by WheelInterface::update
			*/
		{
			//This function uses the travel percentage to generate a compression force.
			//This is just the *magnitude*, independent of direction: the WheelInterface uses this value as it knows about direction.

			mCompressionForce = mTravelPercentage * 9000.0;
			mTorqueMagnitude = mCompressionForce * mDistToWheelCentre;
		}

		inline void setTravelPercentage(double newTravelPercent) { mTravelPercentage = newTravelPercent < 0.0 ? 0.0 : newTravelPercent > 1.0 ? 1.0 : newTravelPercent; }
		inline double getTorqueMagnitude() const { return mTorqueMagnitude; }

	};
}

#endif
