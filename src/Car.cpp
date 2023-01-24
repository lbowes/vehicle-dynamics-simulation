#include "Car.h"

namespace Internal {

	Car::Car() :
		/* Called during VehicleSimulation::VehicleSimulation
		 * Fully sets up this object ready for the start of the simulation
		*/
		RigidBody(Framework::Physics::RigidBody::IntegrationMethod::EULER)
	{ 
		assemble();
		resetToTrackPosition();
	} 
	
	void Car::update(double t, double dt) 
		/* Called by VehicleSimulation::onUpdate
		 * The core update function for the Car's simulation
		 * Updates member objects and then updates own internal physical state  
		*/
	{
		//Member objects updated
		mControlSystem.update(mWheelSystem.getWheelBase(), mWheelSystem.getAxle(WheelSystem::AxlePos::FRONT).getLength());
		mTorqueGenerator->update();
		mWheelSystem.update(mState, mAcceleration, dt);
		
		//State advanced by dt seconds
		integrate(t, dt);
		
		positionConstraints();
	}

	void Car::checkInput(double dt) 
		/* Called by VehicleSimulation::onInputCheck
		 * Passes main input handling responsibility to mControlSystem
		*/
	{
		using namespace Framework;

		mControlSystem.handleInput(dt);

		if (Input::isKeyPressed(GLFW_KEY_R)) {
			resetToTrackPosition();
			mWheelSystem.reset();
		}
	}

	void Car::resetToTrackPosition() 
		/* Called by
		 * - Car::Car
		 * - Car::checkInput
		 * - UILayer::mainControlPanel
		*/
	{
		mState.reset();
		mState.setPosition_world(glm::dvec3(10.0, -1.0, 0.0));
	}

	void Car::updateTotalForce_world() 
		/* Called by Car::getForce_world
		 * Responsible for summating all forces affecting the Car 
		*/
	{
		mTotalForce_world = glm::dvec3(0.0);
		mAerodynamicDrag_world = glm::dvec3(0.0);

		glm::dvec3 velocity_world = mState.getVelocity_world();

		if(glm::length(velocity_world))
			mAerodynamicDrag_world = -0.5 * pow(glm::length(velocity_world), 2.0) * mDragCoefficient * mFrontalArea * External::Environment::mAirDensity * glm::normalize(velocity_world);

		mTotalForce_world += glm::dvec3(0.0, mState.getMass().getValue() * -External::Environment::mGravityAccel, 0.0); //Gravity
		mTotalForce_world += mWheelSystem.getTotalForce_world();                                                        //Wheel interfaces
		mTotalForce_world += mAerodynamicDrag_world;                                                                    //Aerodynamic drag
	}

	glm::dvec3 Car::getForce_world(Framework::Physics::State& state, double t) 
		/* Called by RigidBody::integrate
		 * Pure virtual function, inherited from RigidBody
		*/
	{
		updateTotalForce_world();
		return mTotalForce_world;
	}

	void Car::updateTotalTorque_world() 
		/* Called by Car::getTorque_world
		 * Responsible for summating all torques affecting the Car
		 */
	{
		//Since all other forces on the Car are modelling as acting through the centre of mass, the wheels are the only
		//objects that generate a torque
		mTotalTorque_world = mWheelSystem.getTotalTorque_world(); //Wheels
	}

	glm::dvec3 Car::getTorque_world(Framework::Physics::State& state, double t) 
		/* Called by RigidBody::integrate
		* Pure virtual function, inherited from RigidBody
		*/
	{
		updateTotalTorque_world();
		return mTotalTorque_world;
	}

	void Car::positionConstraints() 
		/* Called by Car::update
		 * Point collision with the terrain's surface, and a boundary constraint at edges of the terrain
		 * Corrective action after physics state update
		*/
	{
		using namespace External;
		using namespace glm;

		dvec3
			positionInitial = mState.getPosition_world(),
			velocityInitial = mState.getVelocity_world(),
			newPosition = positionInitial,
			newVelocity = velocityInitial;

		const double planeHeight = Environment::mTerrain.getHeight(dvec2(positionInitial.x, positionInitial.z));

		//Terrain collision
		if (positionInitial.y < planeHeight) {
			double beneathTerrain = planeHeight - positionInitial.y;
			newPosition.y += beneathTerrain;
			newVelocity.y *= -0.3;
		}

		//Playable region limits
		double halfTerrainSize = floor(Environment::mTerrain.getSize() * 0.5);
		if (positionInitial.x < -halfTerrainSize) {
			newPosition.x = -halfTerrainSize;
			
			//This causes the Car to bounce of the invisible walls at terrain edges
			newVelocity.x *= -0.5;
		}
		else if (positionInitial.x > halfTerrainSize) {
			newPosition.x = halfTerrainSize;
			newVelocity.x *= -0.5;
		}

		if (positionInitial.z < -halfTerrainSize) {
			newPosition.z = -halfTerrainSize;
			newVelocity.z *= -0.5;
		}
		else if (positionInitial.z > halfTerrainSize) {
			newPosition.z = halfTerrainSize;
			newVelocity.z *= 0.5;
		}

		mState.setPosition_world(newPosition);
		mState.setVelocity_world(newVelocity);
	}

	void Car::assemble() 
		/* Called by Car::Car
		 * Crucial that this is called before the simulation begins
		 * 'Wires together' sub components of the Car
		*/
	{
		using namespace Internal;

		const double
			minTurningRadius = 3.0,
			steeringRatio = 13.0,
			wheelBase = mWheelSystem.getWheelBase(),
			maxAbsoluteAngle = glm::degrees(asin(wheelBase / sqrt(pow(minTurningRadius, 2) + pow(wheelBase, 2))));

		//Add rear engine
		mTorqueGenerator = std::make_unique<TorqueGenerator>(5500.0, 4000.0);

		//...and attach it to the rear axle
		Axle* tempAxle = &mWheelSystem.getAxle(WheelSystem::REAR);
		tempAxle->attachTorqueGenerator(mTorqueGenerator.get());

		//Set geometrical properties of car
		mWheelSystem.setMinimumTurnRadius(minTurningRadius);

		mControlSystem.setSteeringRatio(steeringRatio);
		mControlSystem.setMaxAbsWheelAngle(maxAbsoluteAngle);
		mControlSystem.attachTorqueGenerator(mTorqueGenerator.get());

		mWheelSystem.bindControlSystem(mControlSystem);

		mState.setMassValue_local(1961.0);
		mState.setInertiaTensor_local(glm::dmat3(mState.getMass().getValue()));
	}

}