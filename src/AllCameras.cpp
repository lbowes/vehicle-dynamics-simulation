#include "AllCameras.h"
#include "Environment.h"

namespace Visual {

	SimulationCamera::SimulationCamera(glm::vec3 position_OGL, glm::vec3 direction_OGL, float near, float far, float aspect, float FOV) :
		/* Called by
		 * - FPVCamera::FPVCamera
		 * - FrontWheelCamera::FrontWheelCamera
		 * - DriverCamera::DriverCamera
		*/
		mPerspectiveCamera(position_OGL, direction_OGL, glm::vec3(0.0f, 1.0f, 0.0f), near, far, aspect, FOV)
	{ }

	FPVCamera::FPVCamera(glm::vec3 position_OGL, glm::vec3 direction_OGL, float near, float far, float aspect, float FOV) :
		/* Called by CameraSystem::addAllCameras
		 * Initialises camera's state ready to view the simulation from the correct position/direction
		*/
		SimulationCamera(position_OGL, direction_OGL, near, far, aspect, FOV),
		mPosition_OGL(position_OGL),
		mDirection_OGL(direction_OGL)
	{
		using namespace glm;

		//Calculating intial yaw and pitch values from direction vector
		vec3 horizontalDirection = normalize(vec3(direction_OGL.x, 0.0f, direction_OGL.z));
		mYaw = degrees(orientedAngle(horizontalDirection, vec3(1.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f)));
		mPitch = degrees(asin(normalize(direction_OGL).y));
	}

	void FPVCamera::update(float windowAspect, float dt) 
		/* Called by CameraSystem::update
		*/
	{
		//If the window is resized between updates, then the camera's aspect ratio must be changed
		mPerspectiveCamera.setAspect(windowAspect);

		//Recalculates position 
		mPosition_OGL += mVelocity_OGL * dt;
		
		//Ensures that the new position is not below the terrain, or outside its borders
		mPosition_OGL = afterPositionConstraints(mPosition_OGL);

		//Updates the internal position of the camera with this new position
		mPerspectiveCamera.setPosition(mPosition_OGL);

		//Applies friction to the camera movement
		mVelocity_OGL *= 1.0f / (1.0f + (dt * mMovementFriction));
	}

	void FPVCamera::handleInput(float dt) 
		/* Called by CameraSystem::checkInput
		*/
	{
		handleDirectionInput(mPerspectiveCamera, mYaw, mPitch, mDirection_OGL, mLookAroundSensitivity);
		handleMovementInput(dt);
		handleZoomInput(mPerspectiveCamera, mZoomSensitivity);
	}

	void FPVCamera::handleMovementInput(float dt) 
		/* Called by FPVCamera::handleInput
		 * Deals with all user input that affects camera translation
		*/
	{
		using namespace Framework;
		using namespace glm;

		if (Input::isKeyPressed(GLFW_KEY_W))          mVelocity_OGL += normalize(vec3(mDirection_OGL.x, 0.0f, mDirection_OGL.z)) * mMovementSpeed * dt;
		if (Input::isKeyPressed(GLFW_KEY_S))          mVelocity_OGL -= normalize(vec3(mDirection_OGL.x, 0.0f, mDirection_OGL.z)) * mMovementSpeed * dt;
		if (Input::isKeyPressed(GLFW_KEY_A))          mVelocity_OGL -= normalize(cross(mDirection_OGL, vec3(0.0f, 1.0f, 0.0f))) *  mMovementSpeed * dt;
		if (Input::isKeyPressed(GLFW_KEY_D))          mVelocity_OGL += normalize(cross(mDirection_OGL, vec3(0.0f, 1.0f, 0.0f))) *  mMovementSpeed * dt;
		if (Input::isKeyPressed(GLFW_KEY_SPACE))      mVelocity_OGL.y += mMovementSpeed * dt;
		if (Input::isKeyPressed(GLFW_KEY_LEFT_SHIFT)) mVelocity_OGL.y -= mMovementSpeed * dt;
	}

	glm::dvec3 FPVCamera::afterPositionConstraints(glm::dvec3 input) 
		/* Called by FPVCamera::update
		*/
	{
		glm::dvec3 output = input;
		
		//Terrain surface constraint
		double terrainHeight = External::Environment::mTerrain.getHeight(glm::dvec2(input.x, input.z));
		if (input.y < terrainHeight + 0.1f)
			output.y = terrainHeight + 0.1f;

		//Terrain border constraints
		double halfTerrainSize = floor(External::Environment::mTerrain.getSize() * 0.5);
		if (input.x < -halfTerrainSize) output.x = -halfTerrainSize;
		else if (input.x > halfTerrainSize) output.x = halfTerrainSize;
		if (input.z < -halfTerrainSize) output.z = -halfTerrainSize;
		else if (input.z > halfTerrainSize) output.z = halfTerrainSize;

		return output;
	}

	FrontWheelCamera::FrontWheelCamera(glm::vec3 position_car, glm::vec3 direction_car, float near, float far, float aspect, float FOV) :
		/* Called by CameraSystem::addAllCameras
		* Initialises camera's state ready to view the simulation from the correct position/direction
		*/
		SimulationCamera(position_car, direction_car, near, far, aspect, FOV)
	{ }

	void FrontWheelCamera::update(float windowAspect, glm::mat4 localToWorld_car, glm::quat localToWorldRotation_car) 
		/* Called by CameraSystem::update
		*/
	{
		mPerspectiveCamera.setAspect(windowAspect);

		//The position and orientation of this camera are tied to the state of the Car, rather than user input
		mPerspectiveCamera.transformPosition(localToWorld_car);
		mPerspectiveCamera.rotate(localToWorldRotation_car);
	}
	
	DriverCamera::DriverCamera(glm::vec3 position_car, glm::vec3 direction_car, float near, float far, float aspect, float FOV) :
		/* Called by CameraSystem::addAllCameras
		* Initialises camera's state ready to view the simulation from the correct position/direction
		*/
		SimulationCamera(position_car, direction_car, near, far, aspect, FOV),
		mDirection_OGL(direction_car)
	{
		using namespace glm;
												  
		//Calculating intial yaw and pitch values from direction vector
		vec3 horizontalDirection = normalize(vec3(direction_car.x, 0.0f, direction_car.z));
		mYaw = degrees(orientedAngle(horizontalDirection, vec3(1.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f)));
		mPitch = degrees(asin(normalize(direction_car).y));
	}

	void DriverCamera::update(float windowAspect, glm::mat4 localToWorld_car, glm::quat localToWorldRotation_car) 
		/* Called by CameraSystem::update
		*/
	{
		mPerspectiveCamera.setAspect(windowAspect);
		
		//The position and up vectors of this camera are tied to the state of the Car...
		mPerspectiveCamera.transformPosition(localToWorld_car);
		mPerspectiveCamera.rotateUp(localToWorldRotation_car);
		
		//...but the user can still look around inside the vehicle																					
		mPerspectiveCamera.setFront(glm::vec3(localToWorldRotation_car * glm::vec4(mDirection_OGL, 1.0f)));
	}

	void DriverCamera::handleInput(float dt) 
		/* Called by CameraSystem::checkInput
		*/
	{
		handleDirectionInput(mPerspectiveCamera, mYaw, mPitch, mDirection_OGL, mLookAroundSensitivity);
		handleZoomInput(mPerspectiveCamera, mZoomSensitivity);
	}

	void handleDirectionInput(Framework::PerspectiveCamera& camHandle, float& yawHandle, float& pitchHandle, glm::vec3& directionHandle, float sensitivity) 
		/* Called by
		 * - FPVCamera::handleInput
		 * - DriverCamera::handleInput
		 * Used by multiple camera classes
		*/
	{
		glm::vec2 mouseDelta = Framework::Input::getMouseDelta();

		yawHandle += mouseDelta.x * sensitivity;
		pitchHandle -= mouseDelta.y * sensitivity;

		if (pitchHandle > 89.0f) pitchHandle = 89.0f;
		if (pitchHandle < -89.0f) pitchHandle = -89.0f;

		directionHandle.x = cos(glm::radians(pitchHandle)) * cos(glm::radians(yawHandle));
		directionHandle.y = sin(glm::radians(pitchHandle));
		directionHandle.z = cos(glm::radians(pitchHandle)) * sin(glm::radians(yawHandle));
		directionHandle = normalize(directionHandle);

		camHandle.setFront(directionHandle);
	}

	void handleZoomInput(Framework::PerspectiveCamera& camHandle, float sensitivity) 
		/* Called by
		 * - FPVCamera::handleInput
		 * - DriverCamera::handleInput
		 * - Used by multiple camera classes
		*/
	{
		float
			mouseScroll = Framework::Input::getMouseScroll(),
			currentFOV = camHandle.getFOVY(),
			newTargetFOV = currentFOV -= mouseScroll * sensitivity;

		if (newTargetFOV >= 44.0f && newTargetFOV <= 46.0f)
			camHandle.setFOVY(currentFOV -= mouseScroll * sensitivity);

		if (Framework::Input::isMouseButtonReleased(GLFW_MOUSE_BUTTON_5) || Framework::Input::isMouseButtonReleased(GLFW_MOUSE_BUTTON_MIDDLE))
			camHandle.setFOVY(45.0f);
	}

}