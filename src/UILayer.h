/* CLASS OVERVIEW
 * Encapsulates all user interface code
 * Rendered on top of the simulation
*/

#ifndef UILAYER_H
#define UILAYER_H
#pragma once

#include <Framework/Imgui/imgui.h>
#include <Framework/Core/ResourceSet.h>
#include <glm/glm/vec3.hpp>

#include "CameraSystem.h"

namespace Internal {
	class Car;
}

namespace Visual {
	class UILayer {
	private:
		Internal::Car& mDataSource;

		Framework::Graphics::Shader& mCarModelShader;

		CameraSystem& mCameraSystem;

		float &mSimulationSpeedHandle;

		bool &mShowDebugMode_handle;

		bool
			mShowDriverInfo = true,
			mShowCarCustomise = false,
			mShowTyreParams = false,
			mShowHelpInfo = true;

	public:
		UILayer(Internal::Car& simDataSource, Framework::Graphics::Shader& carModelShader, CameraSystem& cameraSystem, float& simulationSpeedHandle, bool& debugModeHandle);
		~UILayer() = default;

		void render();

	private:
		void load() const;

		void mainControlPanel();
		void carCustomisation() const;
		void driverInfo() const;
		void helpInfo() const;
		void carDebugInfo() const;
		void debug_physicsTelemetry() const;
		void debug_wheelTelemetry(unsigned char axlePos, unsigned char side, ImVec4 colour) const;
		void debug_allWheelTelemetry() const;
		void tyreParameters() const;
		void upsideDownWarning() const;

	};
}

#endif
