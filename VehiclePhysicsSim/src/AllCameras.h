/* CLASS(ES) OVERVIEW
 * - SimulationCamera is just a wrapper around a Framework::PerspectiveCamera
 * - FPVCamera, FrontWheelCamera, DriverCamera all inherit from SimulationCamera
 * - Each have unique ways of updating themselves/controllable features (like movement, gaze direction etc)
*/

#ifndef ALLCAMERAS_H
#define ALLCAMERAS_H
#pragma once

#include <glm/glm/gtx/vector_angle.hpp>
#include <Framework/Camera/PerspectiveCamera.h>
#include <Framework/Input/Input.h>

namespace Visual {
	class SimulationCamera {
	protected:
		Framework::PerspectiveCamera mPerspectiveCamera;

	public:
		SimulationCamera(glm::vec3 position_OGL, glm::vec3 direction_OGL, float near, float far, float aspect, float FOV);
		~SimulationCamera() = default;

		inline Framework::Camera& getInternalCamera() { return mPerspectiveCamera; }

	};

	class FPVCamera : public SimulationCamera {
	private:
		const float
			mMovementSpeed = 275.0f,       //400.0f  
			mMovementFriction = 7.0f,       //7.0f
			mZoomSensitivity = 0.1f,        //0.1f
			mLookAroundSensitivity = 0.05f; //0.05f

		float
			mPitch = 0.0f,
			mYaw = 0.0f;

		glm::vec3
			mPosition_OGL,
			mVelocity_OGL,
			mDirection_OGL;

	public:
		FPVCamera(glm::vec3 position_OGL, glm::vec3 direction_OGL, float near, float far, float aspect, float FOV);
		~FPVCamera() = default;

		void update(float windowAspect, float dt);
		void handleInput(float dt);

	private:
		void handleMovementInput(float dt);
		glm::dvec3 afterPositionConstraints(glm::dvec3 input);

	};

	class FrontWheelCamera : public SimulationCamera {
	private:
		const glm::dvec3
			mPositionOnStage_stage = glm::vec3(-0.401277f, 44.73737f, -1.850528f),
			mFront_stage = glm::vec3(0.0f, -1.0f, 0.0f),
			mUp_stage = glm::vec3(-0.214532f, 0.0f, -0.976717f);

	public:
		FrontWheelCamera(glm::vec3 position_car, glm::vec3 direction_car, float near, float far, float aspect, float FOV);
		~FrontWheelCamera() = default;

		void update(float windowAspect, glm::mat4 localToWorld_car, glm::quat localToWorldRotation_car);

	};

	class DriverCamera : public SimulationCamera {
	private:
		const glm::dvec3 mPosition_car = glm::vec3(-0.401277f, 44.73737f, -1.850528f);

		glm::vec3 
			mPositionInCar_OGL,
			mVelocityInCar_OGL,
			mDirection_OGL;

		const float
			mZoomSensitivity = 0.1f,        //0.1f
			mLookAroundSensitivity = 0.05f; //0.05f

		float
			mPitch = 0.0f,
			mYaw = 0.0f;

	public:
		DriverCamera(glm::vec3 position_car, glm::vec3 direction_car, float near, float far, float aspect, float FOV);
		~DriverCamera() = default;

		void update(float windowAspect, glm::mat4 localToWorld_car, glm::quat localToWorldRotation_car);
		void handleInput(float dt);

	};

	void handleDirectionInput(Framework::PerspectiveCamera& camHandle, float& yawHandle, float& pitchHandle, glm::vec3& directionHandle, float sensitivity);
	void handleZoomInput(Framework::PerspectiveCamera& camHandle, float sensitivity);

}

#endif