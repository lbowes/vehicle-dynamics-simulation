/* CLASS OVERVIEW
 * Root class of the visual side of the application
 * Manages all graphical models, the UI layer and the camera system, as well as the objects required to render the models (a ResourceSet, Renderers)
*/

#ifndef VISUALSHELL_H
#define VISUALSHELL_H
#pragma once

#include <Framework/Core/Window.h>
#include <Framework/Core/ResourceSet.h>
#include <Framework/Input/Input.h>
#include <Framework/Imgui/imgui.h>

#include "GameCarModel.h"
#include "DebugCarModel.h"
#include "EnvironmentModel.h"
#include "CameraSystem.h"
#include "UILayer.h"

//temp
#include <Framework/Camera/OrthographicCamera.h>
//

namespace Visual {
	class VisualShell {
	private:
		Framework::Window& mWindow;
		Framework::ResourceSet mResourceHolder;

		Framework::Graphics::Renderer
			mBaseRenderer,
			mDebugLayerRenderer;

		Internal::Car& mDataSource;

		//temp
		std::unique_ptr<Framework::OrthographicCamera> orthoCam;
		//

		CameraSystem mCameraSystem;
		std::unique_ptr<UILayer> mUILayer;

		std::unique_ptr<GameCarModel> mGameCarModel;
		std::unique_ptr<DebugCarModel> mDebugCarModel;
		std::unique_ptr<EnvironmentModel> mEnvironmentModel;

		bool mDebugMode = false;

		float& mSimulationSpeedHandle;

	public:
		VisualShell(Internal::Car& dataSource, Framework::Window& window, float& simSpeedHandle);
		~VisualShell() = default;

		void update(float dt);
		void renderAll();
		void checkInput(float dt);

		inline bool getDebugMode() const { return mDebugMode; }
		inline void setDebugMode(bool debugOn) { mDebugMode = debugOn; }

	private:
		void load();

	};
}

#endif
