/* CLASS OVERVIEW
 * - Manages instances of all 3 SimulationCameras, FPVCamera, DriverCamera and FrontWheelCamera
 * - Controls which of the 3 is currently active and updates all 3 accordingly
*/

#ifndef CAMERASYSTEM_H
#define CAMERASYSTEM_H

#include <vector>
#include <memory>
#include <Framework/Input/Input.h>

#include "AllCameras.h"

#define FPV_CAM static_cast<FPVCamera*>(mCameras[0].get())
#define FRONT_WHEEL_CAM static_cast<FrontWheelCamera*>(mCameras[1].get())
#define DRIVER_CAM static_cast<DriverCamera*>(mCameras[2].get())

namespace Internal {
	class Car;
}

namespace Visual {
	class CameraSystem {
	public:
		enum CameraName { FPV, FR_WHEEL, DRIVER };
	
	private:
		std::vector<std::unique_ptr<SimulationCamera>> mCameras;

		int mCurrentCameraName = DRIVER;

		bool mHasFocus = false;

	public:
		CameraSystem(float windowAspect);
		~CameraSystem() = default;

		void update(float windowAspect, float dt, Internal::Car& cameraTarget);
		void checkInput(float dt);
		void cycleCameras(bool left);

		inline bool hasFocus() const { return mHasFocus; }
		inline SimulationCamera& getCurrentSimCamera() const { return *mCameras[mCurrentCameraName].get(); }
		inline int getCurrentCameraName() const { return mCurrentCameraName; }

		inline void shouldHaveFocus(bool shouldHaveFocus) { mHasFocus = shouldHaveFocus; }

	private:
		void addAllCameras(float windowAspect);

	};
}

#endif
