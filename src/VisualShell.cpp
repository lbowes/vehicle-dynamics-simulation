#include "VisualShell.h"
#include "Car.h"

namespace Visual {

	VisualShell::VisualShell(Internal::Car& dataSource, Framework::Window& window, float& simSpeedHandle) :
		/* Called by VehicleSimulation::onLoad
		*/
		mWindow(window),
		mDataSource(dataSource),
		mCameraSystem(window.getAspect()),
		mSimulationSpeedHandle(simSpeedHandle)
	{
		load();
	}

	void VisualShell::update(float dt)
		/* Called by VehicleSimulation::onRender
		*/
	{
		mCameraSystem.update(mWindow.getAspect(), dt, mDataSource);
	}

	void VisualShell::renderAll()
		/* Called by VehicleSimulation::onRender
		*/
	{
		//temp
		//glm::dvec3 carPos = mDataSource.getWheelSystem().getWheelInterface(Internal::WheelSystem::AxlePos::REAR, Internal::WheelSystem::Side::LEFT).getPosition_world();
		//glm::dvec3
		//	wheelPos_world = mDataSource.getWheelSystem().getWheelInterface(Internal::WheelSystem::AxlePos::REAR, Internal::WheelSystem::Side::LEFT).getPosition_world(),
		//	targetPos_world = glm::dvec3(0.0, External::Environment::mTerrain.getHeight({ wheelPos_world.x, wheelPos_world.z }), wheelPos_world.z);
		//glm::dvec3 carPos = mDataSource.getState().getPosition_world();

		//orthoCam->setPosition({0.0, carPos.y, carPos.z});
		//mBaseRenderer.setCamera(*orthoCam);
		//mDebugLayerRenderer.setCamera(*orthoCam);
		//

		//Always rendered
		mUILayer->render();
		mGameCarModel->render(mBaseRenderer);
		mEnvironmentModel->render(mBaseRenderer);
		mBaseRenderer.flush();

		//Only rendered in debug mode
		if (mDebugMode) {
			mDebugCarModel->render(mDebugLayerRenderer);
			glLineWidth(3.0f);
			glClear(GL_DEPTH_BUFFER_BIT);
			mDebugLayerRenderer.flush();
			glLineWidth(1.0f);
		}
	}

	void VisualShell::checkInput(float dt)
		/* Called by VehicleSimulation::onInputCheck
		 * Any user input that the visual side of the application requires is processed here
		*/
	{
		using namespace Framework;

		//Toggle wireframe mode
		static int mode = GL_FILL;
		if (Input::isKeyReleased(GLFW_KEY_0)) {
			mode = mode == GL_LINE ? GL_FILL : GL_LINE;
			glPolygonMode(GL_FRONT_AND_BACK, mode);
		}

		//Hide/show cursor for menu interaction/camera focus respectively
		if (Input::isMouseButtonReleased(GLFW_MOUSE_BUTTON_RIGHT) && mCameraSystem.hasFocus()) {
			Input::showCursor();
			Input::setMousePosition(glm::vec2(mWindow.getWidth() / 2.0f, mWindow.getHeight() / 2.0f));
			mCameraSystem.shouldHaveFocus(false);
		}
		if (Input::isMouseButtonReleased(GLFW_MOUSE_BUTTON_LEFT) && !ImGui::GetIO().WantCaptureMouse) {
			Input::hideCursor();
			mCameraSystem.shouldHaveFocus(true);
		}

		SimulationCamera& currentCamera = mCameraSystem.getCurrentSimCamera();
		mBaseRenderer.setCamera(currentCamera.getInternalCamera());
		mDebugLayerRenderer.setCamera(currentCamera.getInternalCamera());

		mCameraSystem.checkInput(dt);
	}

	void VisualShell::load()
		/* Called by VisualShell::VisualShell
		 * Called once at load time
		*/
	{
		using namespace glm;

		//temp
		//For viewing car
		orthoCam = std::make_unique<Framework::OrthographicCamera>(vec3(0.0f, 0.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f), 0.1f, 100.0f, mWindow.getAspect(), 1.5f/*2.5f*/);
		//For viewing terrain from above
		//orthoCam = std::make_unique<Framework::OrthographicCamera>(vec3(0.0f, 200.0f, 0.0f), vec3(0.0, -1.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f), 100.0f, 300.0f, mWindow.getAspect(), 150.0f);
		//

		mWindow.setClearColour(vec4(0.5, 0.5, 0.5, 1.0));

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		mEnvironmentModel = std::make_unique<EnvironmentModel>(mResourceHolder);
		mGameCarModel = std::make_unique<GameCarModel>(mDataSource, mResourceHolder);
		mGameCarModel->setShaderUniforms(
			mEnvironmentModel->getFogDensity(),
			mEnvironmentModel->getFogGradient(),
			mEnvironmentModel->getSkyColour(),
			mEnvironmentModel->getSunDirection()
		);

		mDebugCarModel = std::make_unique<DebugCarModel>(mDataSource, mResourceHolder);

		mUILayer = std::make_unique<UILayer>(mDataSource, *mResourceHolder.getResource<Framework::Graphics::Shader>("bodyShader"), mCameraSystem, mSimulationSpeedHandle, mDebugMode);

		Framework::Camera& currentCamera = mCameraSystem.getCurrentSimCamera().getInternalCamera();
		mBaseRenderer.setCamera(currentCamera);
		mDebugLayerRenderer.setCamera(currentCamera);
	}

}