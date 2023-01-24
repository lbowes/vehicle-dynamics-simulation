#include "WheelSystem.h"
#include "Environment.h"
#include "ControlSystem.h"

namespace Internal {

	WheelSystem::WheelSystem() 
		/* Called during Car::Car
		*/
	{
		for (unsigned char i = 0; i < 4; i++)
			mWheelInterfaces.push_back(WheelInterface(getAxle(calcAxleFromIndex(i))));

		mFrontAxle.setLongDisplacement_car(-mWheelBase * 0.5);
		mFrontAxle.setLength(mTrack_front);
		mRearAxle.setLongDisplacement_car(mWheelBase * 0.5);
		mRearAxle.setLength(mTrack_rear);

		positionWheelInterfaces();
	}

	void WheelSystem::update(Framework::Physics::State& carState, glm::dvec3 carAcceleration_world, double dt) 
		/* Called by Car::update 
		 * Passes Car physical-state information down to the WheelInterfaces and updates them
		 * Calculates the final force and torque vectors
		*/
	{
		glm::dvec3 carAcceleration_car = glm::dvec3(carState.getWorldToLocal_direction() * glm::dvec4(carAcceleration_world, 1.0));
		
		updateAxles();
		updateAllWheelInterfaces(carState, carAcceleration_car, dt);
		updateTotalForce_world();
		updateTotalTorque_world(carState.getPosition_world(), carState.getLocalToWorld_position());
	}

	void WheelSystem::bindControlSystem(ControlSystem& controlSystem) 
		/* Called by Car::assemble 
		*/
	{
		//Gives the control system access to the front wheels so they can be steered with this class
		controlSystem.attachWheels(&getWheelInterface(FRONT, LEFT).getWheel(), &getWheelInterface(FRONT, RIGHT).getWheel());

		//Gives the control system access to all the brakes
		std::vector<Brake*> brakes = {
			&mWheelInterfaces[0].getBrake(),
			&mWheelInterfaces[1].getBrake(),
			&mWheelInterfaces[2].getBrake(),
			&mWheelInterfaces[3].getBrake(),
		};
		controlSystem.attachBrakes(brakes);
	}

	void WheelSystem::reset() 
		/* Called by Car::checkInput
		*/
	{
		for (WheelInterface& w : mWheelInterfaces)
			w.reset();
	}

	void WheelSystem::positionWheelInterfaces() 
		/* Called by 
		 * Responsible for calculating the position of the wheel interfaces based on parameters
		 * Must be called whenever these parameters change (or are set for the first time)
		*/
	{
		glm::dvec3 newPosition;

		Axle* currentAxle = nullptr;

		for (unsigned char i = 0; i < 4; i++) {
			currentAxle = &getAxle(calcAxleFromIndex(i));
			
			newPosition.x = (calcSideFromIndex(i) == LEFT ? -1.0 : 1.0) * currentAxle->getLength() * 0.5;
			newPosition.y = mWheelInterfaces[i].getPosition_car().y + mWheelHeight;
			newPosition.z = currentAxle->getLongDisplacement_car();

			mWheelInterfaces[i].setPosition_car(newPosition);
		}
	}

	void WheelSystem::updateAxles() 
		/* Called by WheelSystem::update
		 * Summates counter torques for both the front and rear axles, and updates them
		*/
	{
		WheelInterface
			*frontLeft = &getWheelInterface(FRONT, LEFT),
			*frontRight = &getWheelInterface(FRONT, RIGHT),
			*rearLeft = &getWheelInterface(REAR, LEFT),
			*rearRight = &getWheelInterface(REAR, RIGHT);
		
		double
			frontAxleCounterTorque = 0.0,
			rearAxleCounterTorque = 0.0;

		//Front axle
		{
			//Brake torque
			frontAxleCounterTorque += frontLeft->getBrake().getTorqueMagnitude() * -frontLeft->getWheel().getRotationDirection();   //Left wheel
			frontAxleCounterTorque += frontRight->getBrake().getTorqueMagnitude() * -frontRight->getWheel().getRotationDirection(); //Right

			//Traction torque
			frontAxleCounterTorque += frontRight->getWheel().getTyre().getTotalForce_wheel().y * frontRight->getWheel().getTotalRadius();
			frontAxleCounterTorque += frontLeft->getWheel().getTyre().getTotalForce_wheel().y * frontRight->getWheel().getTotalRadius();

			//Send this total resistive torque to the front axle itself
			mFrontAxle.update(std::max(frontLeft->getWheel().getRPM(), frontRight->getWheel().getRPM()) * 60.0, frontAxleCounterTorque);
		}

		//Rear axle
		{
			//Add the brake torques from both wheels on the rear axle
			rearAxleCounterTorque += rearLeft->getBrake().getTorqueMagnitude() * -rearLeft->getWheel().getRotationDirection();   //Left wheel
			rearAxleCounterTorque += rearRight->getBrake().getTorqueMagnitude() * -rearRight->getWheel().getRotationDirection(); //Right
																																  
			//Add the traction torques from both wheels on the rear axle
			rearAxleCounterTorque += rearRight->getWheel().getTyre().getTotalForce_wheel().y * rearRight->getWheel().getTotalRadius();
			rearAxleCounterTorque += rearLeft->getWheel().getTyre().getTotalForce_wheel().y * rearLeft->getWheel().getTotalRadius();

			//Send this total resistive torque to the front axle itself
			mRearAxle.update(std::max(rearLeft->getWheel().getRPM(), rearRight->getWheel().getRPM()) * 60.0, rearAxleCounterTorque);
		}
	}

	void WheelSystem::updateAllWheelInterfaces(Framework::Physics::State& carState, glm::dvec3 carAcceleration_car, double dt) 
		/* Called by WheelSystem::update
		*/
	{
		double carCMHeightAboveGround = recalcCarCmHeightAboveGround(carState);

		for (unsigned char i = 0; i < mWheelInterfaces.size(); i++)
			mWheelInterfaces[i].update(carState, recalcLoad(calcAxleFromIndex(i), calcSideFromIndex(i), carState.getMass(), carAcceleration_car, carCMHeightAboveGround), dt);
	}

	double WheelSystem::recalcLoad(AxlePos position, Side side, Framework::Physics::Mass& carMass_car, glm::dvec3 carAcceleration_car, double carCMHeightAboveGround) 
		/* Called by WheelSystem::updateAllWheelInterfaces 
		*/
	{
		Axle& currentAxle = position == FRONT ? mFrontAxle : mRearAxle;
		
		double
			carMassValue = carMass_car.getValue(),
			
			//The load on the axle involved, when the car is at rest.
			restAxleLoad = abs(currentAxle.getLongDisplacement_car() - carMass_car.getCentre().z) / mWheelBase * (carMassValue * External::Environment::mGravityAccel),
			
			//The current load on the axle involved, considering load transfer due to longitudinal acceleration.
			currentAxleLoad = restAxleLoad + (carCMHeightAboveGround / mWheelBase) * carMassValue * carAcceleration_car.z * (signbit(currentAxle.getLongDisplacement_car()) ? -1.0 : 1.0),
			
			//The load on the wheel involved, when the car is at rest.
			restIndividualWheelLoad = abs(getWheelInterface(position, side).getWheel().getPosition_car().x - carMass_car.getCentre().x) / currentAxle.getLength() * currentAxleLoad,
			
			//The current load on the wheel involved, considering load transfer due to lateral acceleration.
			currentWheelLoad = restIndividualWheelLoad + (carCMHeightAboveGround / currentAxle.getLength()) * carMassValue * carAcceleration_car.x * (signbit(getWheelInterface(position, side).getWheel().getPosition_car().x) ? 1.0 : -1.0);
	
		return currentWheelLoad;
	}

	double WheelSystem::recalcCarCmHeightAboveGround(Framework::Physics::State& carState) 
		/* Called by WheelSystem::updateAllWheelInterfaces
		*/
	{
		/* This function needs to return a single value representing the height of the car's centre of mass above the ground.
		 * The 'ground' in this case is not strictly speaking the actual height of the terrain below the car's centre of mass
		 * (what might be presumed). Instead, the ground height is taken to be the average height of the contact patches of
		 * the tyres.
		 *
		 * Using the terrain height sampling method, if the car drove *over* a huge dip in the terrain (not *into* the dip, as
		 * the wheels would pass around it), then the centre of mass height above the ground would suddenly increase and this
		 * would have a sudden effect on the load. But this is completely wrong, as the wheels have continued moving along the
		 * flat terrain in front of them. There should not be any increase in load in this situation at all.
		 *
		 * It can thus be said that the height used must be connected to the height of the wheels, specifically, the average
		 * height of the tyre contact patches.
		*/

		glm::dvec3 avgWheelOriginPos_world;

		double
			avgWheelRadius = 0.0,
			avgTyreContactPatchHeight_world = 0.0;

		Wheel* currentWheel = nullptr;

		for (WheelInterface& w : mWheelInterfaces) {
			currentWheel = &w.getWheel();
			avgWheelOriginPos_world += glm::dvec3(carState.getLocalToWorld_position() * glm::dvec4(currentWheel->getPosition_car(), 1.0));
			avgWheelRadius += currentWheel->getTotalRadius();
		}

		avgWheelOriginPos_world /= mWheelInterfaces.size();
		avgWheelRadius /= mWheelInterfaces.size();

		avgTyreContactPatchHeight_world = avgWheelOriginPos_world.y - avgWheelRadius;

		return carState.getPosition_world().y - avgTyreContactPatchHeight_world;
	}

	void WheelSystem::updateTotalForce_world() 
		/* Called by WheelSystem::update
		*/
	{
		mTotalForce_world = glm::dvec3(0.0);
		
		for (WheelInterface& w : mWheelInterfaces) {
			mTotalForce_world += w.getSuspension().getForce_world();
			mTotalForce_world += w.getWheel().getTyreForce_world();
		}
	}

	void WheelSystem::updateTotalTorque_world(glm::dvec3 carPosition_world, glm::dmat4 carToWorldTransform_car) 
		/* Called by WheelSystem::update
		*/
	{
		mTotalTorque_world = glm::dvec3(0.0);

		for (WheelInterface& w : mWheelInterfaces) {
			mTotalTorque_world += glm::cross(w.getPosition_world() - carPosition_world, w.getSuspension().getForce_world());
			mTotalTorque_world += glm::cross(glm::dvec3(carToWorldTransform_car * glm::dvec4(w.getWheel().getContactPatchPosition_car(), 1.0)) - carPosition_world, w.getWheel().getTyreForce_world());
		}
	}

}