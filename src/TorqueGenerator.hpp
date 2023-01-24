/* CLASS OVERVIEW
 * - Calculates, updates and provides access to a torque
*/

#ifndef TORQUEGENERATOR_H
#define TORQUEGENERATOR_H
#pragma once

#include <cmath>
#include <algorithm>

namespace Internal {
	class TorqueGenerator {
	private:
		const double
			mMaxOutputForwardTorque = 0.0,
			mMaxOutputReverseTorque = 0.0,
			mGeneratorTorque = 100.0;
		
		double
			mOutputTorque = 0.0,
			mRPM = 0.0,
			mInertia = 0.0,
			mThrottle = 0.0;

		bool mReverseMode = false;

	public:
		TorqueGenerator(double maxForwardTorque, double maxReverseTorque) :
			/* Called by Car::assemble
			*/
			mMaxOutputForwardTorque(maxForwardTorque),
			mMaxOutputReverseTorque(maxReverseTorque)
		{ }

		~TorqueGenerator() = default;

		void update() 
			/* Called by Car::update
			*/
		{
			//Prevents the torque generator from rotating so quickly that brakes become ineffective
			if (mReverseMode)
				mOutputTorque = -(std::max)((1.0 - pow(mRPM / 1000.0, 2.0)), 0.0) * mMaxOutputReverseTorque * mThrottle;
			else
				mOutputTorque = (std::max)((1.0 - pow(mRPM / 4000.0, 2.0)), 0.0) * mMaxOutputForwardTorque * mThrottle;
		}

		inline void updateRPM(double newRPM) { mRPM = newRPM; }
		inline void setThrottle(double newThrottle) { mThrottle = newThrottle < 0.0 ? 0.0 : newThrottle > 1.0 ? 1.0 : newThrottle; }
		inline void toggleReverse() { mReverseMode = !mReverseMode; }
		inline double getOutputTorque() const { return mOutputTorque; }
		inline bool reverseModeOn() const { return mReverseMode; }

	};
}

#endif
