#include "VehicleSimulation.h"

VehicleSimulation::VehicleSimulation() :
	/* Called by main
	*/
	Application("NEA - Vehicle Simulation", "res/images/windowIcon.png", false)
{
	onLoad();
}

void VehicleSimulation::onLoad()
	/* Called by VehicleSimulation::VehicleSimulation
	 * Called once
	*/
{
	mVisuals = std::make_unique<Visual::VisualShell>(mCar, mWindow, mSimulationSpeed);
}

void VehicleSimulation::onInputCheck()
	/* Called by VehicleSimulation::run
	 * Called once per frame
	*/
{
	using namespace Framework;

	if (Input::isKeyReleased(GLFW_KEY_ESCAPE)) mRunning = false;

	mCar.checkInput(mFrameTime * mSimulationSpeed);
	mVisuals->checkInput(mFrameTime);
}

void VehicleSimulation::onUpdate()
	/* Called by VehicleSimulation::run
	 * Called multiple times per frame
	*/
{
	mCar.update(mCurrentTime, mUpdateDelta * mSimulationSpeed);
}

void VehicleSimulation::onRender()
	/* Called by Application::render
	 * Called once per frame
	*/
{
	mVisuals->update(mFrameTime);
	mVisuals->renderAll();
}