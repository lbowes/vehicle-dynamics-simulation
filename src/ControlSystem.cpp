#include "ControlSystem.h"
#include "Wheel.h"
#include "Brake.hpp"

namespace Internal {

	void ControlSystem::update(double wheelBase, double frontAxleTrack)
		/* Called by Car::update
		*/
	{
		updateSteeringAngle(wheelBase, frontAxleTrack);
	}

	void ControlSystem::handleInput(double dt)
		/* Called by Car::checkInput
		 * Called once per Car update
		*/
	{
		handleSteeringInput(dt);
		handleSpeedInput();
	}

	void ControlSystem::attachWheels(Wheel* left, Wheel* right)
		/* Called by WheelSystem::bindControlSystem
		 * Called once at load time
		*/
	{
		mLeftWheel = left;
		mRightWheel = right;
	}

	void ControlSystem::setMaxAbsWheelAngle(double maxAbsAngle)
		/* Called by Car::assemble
		 * Called once at load time
		*/
	{
		mMaxAbsWheelAngle = maxAbsAngle;
		mMaxAbsSteeringWheelAngle = maxAbsAngle * mSteeringRatio;
	}

	void ControlSystem::updateSteeringAngle(double wheelBase, double frontAxleTrack)
		/* Called by ControlSystem::update
		 * Responsible for updating the steering angles of both attached wheels
		*/
	{
		using namespace glm;

		//Using the current steering wheel's angle, this can set the deflection angles of the two front wheels
		//accounting for Ackermann geometry and the steering ratio.
		double
			leftWheelAngle = 0.0,
			rightWheelAngle = 0.0;

		//Turning right
		if (mSteeringWheelAngle > 0.0) {
			rightWheelAngle = -mSteeringWheelAngle / mSteeringRatio;
			leftWheelAngle = -degrees(asin(wheelBase / sqrt(pow((((wheelBase * sin(radians(90.0 - abs(rightWheelAngle)))) / (sin(radians(abs(rightWheelAngle))))) + frontAxleTrack), 2) + pow(wheelBase, 2))));
		}
		//Turning left
		else if (mSteeringWheelAngle < 0.0) {
			leftWheelAngle = -mSteeringWheelAngle / mSteeringRatio;
			rightWheelAngle = degrees(asin(wheelBase / sqrt(pow((((wheelBase * sin(radians(90.0 - leftWheelAngle))) / (sin(radians(leftWheelAngle)))) + frontAxleTrack), 2) + pow(wheelBase, 2))));
		}

		//Car's wheels are toe-out during steering, hence why two different angles are required
		mLeftWheel->setSteeringAngle(leftWheelAngle);
		mRightWheel->setSteeringAngle(rightWheelAngle);
	}

	void ControlSystem::handleSteeringInput(double dt)
		/* Called by ControlSystem::handleInput
		 * Responsible for handling the input for just the steering wheel
		*/
	{
		bool
			steerLeft = Framework::Input::isKeyPressed(GLFW_KEY_RIGHT),
			steerRight = Framework::Input::isKeyPressed(GLFW_KEY_LEFT);

		if (steerLeft || steerRight) {
			double targetAngle = 0.0;

			if (steerLeft) {
				targetAngle = mSteeringWheelAngle + mSteeringRate * dt;
				mSteeringWheelAngle = targetAngle > mMaxAbsSteeringWheelAngle ? mMaxAbsSteeringWheelAngle : targetAngle;
			}
			if (steerRight) {
				targetAngle = mSteeringWheelAngle - mSteeringRate * dt;
				mSteeringWheelAngle = targetAngle < -mMaxAbsSteeringWheelAngle ? -mMaxAbsSteeringWheelAngle : targetAngle;
			}
		}
		else {
			double steeringFraction = abs(mSteeringWheelAngle) / mMaxAbsSteeringWheelAngle;

			if (mSteeringWheelAngle < 0.0)
				mSteeringWheelAngle += mSteeringRate * steeringFraction * dt;
			else
				mSteeringWheelAngle -= mSteeringRate * steeringFraction * dt;
		}
	}

	void ControlSystem::handleSpeedInput()
		/* Called by ControlSystem::handleInput
		 * Responsible for handling the input for changing the speed of the Car
		*/
	{
		//Neutralise everything before input is checked
		mTorqueGenerator->setThrottle(0.0);

		for (Brake* b : mBrakes)
			b->setTravelPercentage(0.0);

		mBrakesOn = false;

		//Checking switch to reverse mode
		if (Framework::Input::isKeyReleased(GLFW_KEY_END))
			mTorqueGenerator->toggleReverse();

		//Accelerator pedal input
		if (Framework::Input::isKeyPressed(GLFW_KEY_UP))
			mTorqueGenerator->setThrottle(1.0);

		//Brake input (All brakes are activated)
		if (Framework::Input::isKeyPressed(GLFW_KEY_DOWN)) {
			mBrakesOn = true;
			for (Brake* b : mBrakes)
				b->setTravelPercentage(1.0);
		}
	}

}