#include "CameraSystem.h"
#include "Car.h"

namespace Visual {

	CameraSystem::CameraSystem(float windowAspect) 
		/* Called by VisualShell::VisualShell
		*/
	{
		addAllCameras(windowAspect);
	}

	void CameraSystem::update(float windowAspect, float dt, Internal::Car& cameraTarget) 
		/* Called by VisualShell::update
		*/
	{
		FPV_CAM->update(windowAspect, dt);
		
		//The front wheel camera and the driver camera are each bound to the car in some way
		glm::mat4 carToWorldTransform = cameraTarget.getState().getLocalToWorld_position();
		glm::quat carToWorldRotation = cameraTarget.getState().getOrientation_world();

		FRONT_WHEEL_CAM->update(windowAspect, carToWorldTransform, carToWorldRotation);
		DRIVER_CAM->update(windowAspect, carToWorldTransform, carToWorldRotation);
	}

	void CameraSystem::checkInput(float dt) 
		/* Called by VisualShell::checkInput
		*/
	{
		if (Framework::Input::isKeyReleased(GLFW_KEY_C))
			cycleCameras(true);

		if (!mHasFocus) return;

		if (mCurrentCameraName == FPV)
			FPV_CAM->handleInput(dt);
		else if (mCurrentCameraName == DRIVER)
			DRIVER_CAM->handleInput(dt);
	}

	void CameraSystem::cycleCameras(bool left) 
		/* Called by CameraSystem::checkInput
		 * - CameraSystem::checkInput
		 * - UILayer::mainControlPanel
		*/
	{
		if (left)
			mCurrentCameraName = mCurrentCameraName + 1 > DRIVER ? FPV : mCurrentCameraName + 1;
		else
			mCurrentCameraName = mCurrentCameraName - 1 < FPV ? DRIVER : mCurrentCameraName - 1;
	}

	void CameraSystem::addAllCameras(float windowAspect) 
		/* Called by CameraSystem::CameraSystem
		 * Defines the initial states of the SimulationCameras
		*/
	{
		mCameras.push_back(std::make_unique<FPVCamera>(
			glm::vec3(13.513960f, -1.0f, -3.806919f),
			glm::vec3(-0.711257f, 0.125334f, 0.691669f),
			0.01f,
			400.0f,
			windowAspect,
			45.0f
			));

		mCameras.push_back(std::make_unique<FrontWheelCamera>(
			glm::vec3(1.417804f, 0.665811f, -0.026093f),
			glm::vec3(-0.314092f, -0.224951f, -0.922357f),
			0.01f,
			1000.0f,
			windowAspect,
			45.0f
			));

		mCameras.push_back(std::make_unique<DriverCamera>(
			glm::vec3(0.5f, 1.0f, 0.0f),
			glm::vec3(0.0f, 0.0f, -1.0f),
			0.01f,
			1000.0f,
			windowAspect,
			45.3f
			));
	}

}