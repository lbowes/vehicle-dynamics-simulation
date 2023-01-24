/* CLASS OVERVIEW
 * The root class for the application
 * Owns the physics simulation (in the form of the mCar object) and its visual representation (mVisuals)
*/

#ifndef VEHICLESIMULATION_H
#define VEHICLESIMULATION_H
#pragma once

#include <Framework/Framework.h>

#include "Car.h"
#include "VisualShell.h"

class VehicleSimulation : public Framework::Application {
private:
	std::unique_ptr<Visual::VisualShell> mVisuals;

	Internal::Car mCar;

	float mSimulationSpeed = 1.0;

public:
	VehicleSimulation();
	~VehicleSimulation() = default;

private:
	void onLoad();
	void onInputCheck();
	void onUpdate();
	void onRender();

};

#endif
