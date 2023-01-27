#include "Wheel.h"

namespace Internal {

	Wheel::Wheel() :
		/* Called during WheelInterface::WheelInterface
		*/
		mTyre(mRimRadius)
	{ }

	void Wheel::update(glm::dmat4 carToWorldRotation_car, glm::dvec3 terrainNormalUnderWheel, glm::dvec2 wheelVel_car, double load, double totalInputTorque, double dt)
		/* Called by WheelInterface::updateWheel
		*/
	{
		mInertiaAboutAxle = mTyre.getAxialInertia();

		updateAngularMotion(totalInputTorque, dt);

		mRotationDirection = mAngularVelocity < 0.0 ? -1 : mAngularVelocity > 0.0 ? 1 : 0;

		glm::dvec2 wheelVel_wheel = glm::rotate(wheelVel_car, glm::radians(mSteeringAngle));
		mTyre.update(wheelVel_wheel, load, 0.0, mRimRadius, mAngularVelocity);

		updateTyreForce_world(carToWorldRotation_car, terrainNormalUnderWheel);
	}

	void Wheel::reset()
		/* Called by WheelInterface::reset
		*/
	{
		mAngularAcceleration = 0.0;
		mAngularVelocity = 0.0;
		mAngularPosition = 0.0;
		mRotationDirection = 0;
		mSteeringAngle = 0.0;

		resetToBasePosition();
	}

	void Wheel::updateAngularMotion(double totalTorque, double dt)
		/* Called by Wheel::update
		 * Handles the updating of state that is only linked to angular motion
		*/
	{
		if (mInertiaAboutAxle)
			mAngularAcceleration = totalTorque / mInertiaAboutAxle;

		mAngularVelocity += mAngularAcceleration * dt;
		mAngularPosition += mAngularVelocity * dt;

		if (mAngularPosition >= glm::two_pi<double>())
			mAngularPosition -= glm::two_pi<double>();

		if (mAngularPosition <= -glm::two_pi<double>())
			mAngularPosition += glm::two_pi<double>();
	}

	void Wheel::updateTyreForce_world(glm::dmat4 carToWorldRotation_car, glm::dvec3 terrainNormalUnderWheel)
		/* Called by Wheel::update
		 * Transforms individual tyre force components, into a 3D force vector in world space that lies tangent to the terrain surface
		*/
	{
		/* 1. Create normalised vector pointing along forward axis in wheel space (0, 0, -1).
		 * 2. Rotate this vector about Y axis, by the steering angle, taking it into car space.
		 * 3. Then rotate the result into world space using carToWorldRotation_car.
		 * 4. Flatten this vector down into the XZ plane and re-normalize it, to remove the Y component.
		 * 5. Rotate it 90 degrees around the Y axis (0, 1, 0)
		 * 6. Take the cross product of the result and the terrain normal to get a vector at a tangent to the terrain's surface.
		 *
		 * This resulting normalised vector will be the direction in which the longitudinal tyre force will act,
		 * in world space. It must then be scaled according to the value produced by the force generator in the Tyre.
		*/

		using namespace glm;

		//This is a check performed to make sure that, if the car is upside down, the tyres do not generate any force
		if (dvec3(carToWorldRotation_car * dvec4(0.0, 1.0, 0.0, 1.0)).y < 0.0) {
			mTyreForce_world = dvec3();
			return;
		}

		dvec3
			normalisedLong_world = normalize(dvec3(carToWorldRotation_car * dvec4(rotate(dvec3(0.0, 0.0, -1.0), radians(mSteeringAngle), dvec3(0.0, 1.0, 0.0)), 1.0))),
			flattenedNormLong_world = normalize(dvec3(normalisedLong_world.x, 0.0, normalisedLong_world.z)),
			rotatedFlatNormLong_world = rotate(flattenedNormLong_world, radians(90.0), dvec3(0.0, 1.0, 0.0)),
			normTangentResult_world = normalize(cross(rotatedFlatNormLong_world, terrainNormalUnderWheel)),
			normalisedLat_world = normalize(cross(terrainNormalUnderWheel, normTangentResult_world));

		mTyreForce_world = normTangentResult_world * mTyre.getTotalForce_wheel().y + normalisedLat_world * mTyre.getTotalForce_wheel().x;
	}

}