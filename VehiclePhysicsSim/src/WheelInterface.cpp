#include "WheelInterface.h"
#include "Environment.h"

namespace Internal {

	WheelInterface::WheelInterface(Axle& connectedAxle) :
		/* Called by WheelSystem::WheelSystem
		*/
		mConnectedAxle(connectedAxle)
	{ }

	void WheelInterface::update(Framework::Physics::State& carState, double load, double dt) 
		/* Called by WheelSystem::updateAllWheelInterfaces
		 * Updates all per-wheel components
		 * Transforms car physical-state data before passing it to components
		*/
	{
		using namespace glm;
		using namespace External;

		mPosition_world = dvec3(carState.getLocalToWorld_position() * dvec4(mPosition_car, 1.0));
		mVelocity_world = carState.getVelocity_world() + cross(carState.getAngularVelocity_world(), mPosition_world - dvec3(carState.getLocalToWorld_position() * dvec4(carState.getMass().getCentre(), 1.0)));

		dvec3 terrainNormal = Environment::mTerrain.getNormal_world(dvec2(mPosition_world.x, mPosition_world.z));

		double
			terrainHeight = Environment::mTerrain.getHeight(dvec2(mPosition_world.x, mPosition_world.z)),
			terrainOverlap = std::max(0.0, mWheel.getTotalRadius() - (mPosition_world.y - terrainHeight));

		mCollisionRegistered = terrainOverlap ? true : false;
		mLoad = mCollisionRegistered ? load : 0.0;

		//Member object updates
		mBrake.update();
		mSuspension.update(mVelocity_world.y, terrainOverlap, terrainNormal);
		updateWheel(carState.getLocalToWorld_direction(), terrainNormal, terrainOverlap, dt);
	}

	void WheelInterface::setPosition_car(glm::dvec3 newPosition_car) 
		/* Called by WheelSystem::positionWheelInterfaces
		 * Changing Wheel interface position, must cause the Wheel to change position
		*/
	{
		//Reposition the wheels
		mPosition_car = newPosition_car;
		mWheel.setBasePosition_car(newPosition_car);
		mWheel.resetToBasePosition();

		//Neutralise the suspension
		mSuspension.neutralise();
	}

	void WheelInterface::reset() 
		/* Called by WheelSystem::reset
		*/
	{
		mWheel.reset();
		mSuspension.neutralise();
		mLoad = 0.0;
	}

	void WheelInterface::updateWheel(glm::dmat4 carToWorldRotation_car, glm::dvec3 terrainNormalUnderWheel, double terrainOverlap, double dt) 
		/* Called by WheelInterface::update
		*/
	{
		using namespace glm;

		//Use carToWorldRotation_car to calculate the velocity of the Wheel, relative to the Car
		dvec3 wheelVelocity_car = dvec3(inverse(carToWorldRotation_car) * dvec4(mVelocity_world, 1.0));
		mWheel.update(carToWorldRotation_car, terrainNormalUnderWheel, dvec2(wheelVelocity_car.x, wheelVelocity_car.z), mLoad, mConnectedAxle.getTransferredTorque(), dt);

		mWheel.resetToBasePosition();
		mWheel.setPosition_car(mWheel.getPosition_car() + dvec3(0.0, terrainOverlap, 0.0));
	}

}
