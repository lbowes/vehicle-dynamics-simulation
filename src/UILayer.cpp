#include <Framework/Physics/State.hpp>

#include "UILayer.h"
#include "Car.h"
#include "Environment.h"

namespace Visual {

	UILayer::UILayer(Internal::Car& simDataSource, Framework::Graphics::Shader& carModelShader, CameraSystem& cameraSystem, float& simulationSpeedHandle, bool& debugModeHandle) :
		/* Called by VisualShell::load
		*/
		mDataSource(simDataSource),
		mCarModelShader(carModelShader),
		mCameraSystem(cameraSystem),
		mSimulationSpeedHandle(simulationSpeedHandle),
		mShowDebugMode_handle(debugModeHandle)
	{
		load();
	}

	void UILayer::render()
		/* Called by VisualShell::renderAll
		 * Renders all active ImGui windows
		*/
	{
		mainControlPanel();

		if (mShowCarCustomise) carCustomisation();
		if (mShowDriverInfo) driverInfo();
		if (mShowDebugMode_handle) carDebugInfo();
		if (mShowHelpInfo) helpInfo();
		if (mShowTyreParams) tyreParameters();

		upsideDownWarning();
	}

	void UILayer::load() const
		/* Called by UILayer::UILayer
		*/
	{
		//Load the ImGui style
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_ChildWindowRounding, 4.0f);
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.17647058823f, 0.17647058823f, 0.18823529411f, 0.4f));
		ImGui::PushStyleColor(ImGuiCol_ChildWindowBg, ImVec4(0.11764705882f, 0.11764705882f, 0.11764705882f, 0.4f));
		ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.64705882352, 0.64705882352, 0.64705882352, 0.4f));
	}

	void UILayer::mainControlPanel()
		/* Called by UILayer::render
		 * Defines the structure of the main control panel window
		 * The main control panel is always available (unlike other windows that can be hidden)
		*/
	{
		using namespace ImGui;

		SetNextWindowPos(ImVec2(0.0f, 0.0f));
		SetNextWindowSize(ImVec2(235.0f, 425.0f));
		Begin("Control panel", NULL, ImGuiWindowFlags_NoResize);
		{
			float childWidth = GetContentRegionAvailWidth();

			Text("Simulation settings");
			BeginChild("Simulation settings", ImVec2(childWidth, 80.0f), true);
			{
				Text("Time");
				SliderFloat("Speed", &mSimulationSpeedHandle, 0.0f, 10.0f);
				if (Button("Resume")) mSimulationSpeedHandle = 1.0f; SameLine();
				if (Button("Pause")) mSimulationSpeedHandle = 0.0f; SameLine();
				if (Button("0.5%")) mSimulationSpeedHandle = 0.005f; SameLine();
				if (Button("50%")) mSimulationSpeedHandle = 0.5f;
				EndChild();
			}

			Text("Vehicle state");
			BeginChild("Vehicle state", ImVec2(childWidth, 65.0f), true);
			{
				Framework::Physics::State& carState = mDataSource.getState();
				static glm::vec3 position = carState.getPosition_world();

				if (Button("Vehicle to track"))
					mDataSource.resetToTrackPosition();

				if (Button("Suspension demo")) {
					glm::dvec3 currentPosition = carState.getPosition_world();
					carState.reset();
					carState.setPosition_world(currentPosition + glm::dvec3(0.0, 5.0, 0.0));

					carState.setOrientation_world(
						rotate(
							carState.getOrientation_world(),
							glm::radians(30.0),
							glm::dvec3((double)rand() / RAND_MAX * 2.0 - 0.5, 0.0, (double)rand() / RAND_MAX * 2.0 - 0.5)
						)
					);
				}
				EndChild();
			}

			Text("View");
			BeginChild("View", ImVec2(childWidth, 185.0f), true);
			{
				Text("Windows");
				Checkbox("Driver info panel", &mShowDriverInfo);
				Checkbox("Debug panel", &mShowDebugMode_handle);
				Checkbox("Car customisation panel", &mShowCarCustomise);
				Checkbox("Help", &mShowHelpInfo);
				Checkbox("Tyre parameters", &mShowTyreParams);

				Separator();

				Text("Cameras");
				if (Button(" <- ")) mCameraSystem.cycleCameras(true);
				SameLine();
				switch (mCameraSystem.getCurrentCameraName()) {
				case CameraSystem::DRIVER:   Text("Driver           "); break;
				case CameraSystem::FPV:      Text("Free camera      "); break;
				case CameraSystem::FR_WHEEL: Text("Front-right wheel"); break;
				default:                     Text("ERROR"); break;
				}
				SameLine();
				if (Button(" -> ")) mCameraSystem.cycleCameras(false);

				EndChild();
			}
		}

		End();
	}

	void UILayer::carCustomisation() const
		/* Called by UILayer::render
		 * Defines the structure of the car customisation window
		*/
	{
		using namespace ImGui;
		using namespace Internal;

		Begin("Vehicle customisation");
		{
			float childWidth = GetContentRegionAvailWidth();

			//Car's mass
			static float mass = 1961.0f;
			Text("Physics state");
			SliderFloat("mass", &mass, 100.0f, 2000.0f);
			if (Button("Reset")) mass = 1961.0f;
			mDataSource.getState().setMassValue_local(mass);

			//Suspension parameters
			Text("Suspension");
			BeginChild("Suspension", ImVec2(childWidth, 100.0f), true);
			{
				static float
					springConstant = 49000.0f,
					dampingCoefficient = 3000.0f;

				SliderFloat("Spring constant", &springConstant, 10000.0f, 50000.0f);
				if (Button("Reset")) springConstant = 49000.0f;
				SliderFloat("Damping coeffient", &dampingCoefficient, 0.0f, 8000.0f);
				if (Button("Reset damping")) dampingCoefficient = 3000.0f;

				Framework::Physics::Spring* currentSpring = nullptr;
				for (Internal::WheelInterface& s : mDataSource.getWheelSystem().getAllWheelInterfaces()) {
					currentSpring = &s.getSuspension().getSpring();
					currentSpring->setSpringConstant(springConstant);
					currentSpring->setDamping(dampingCoefficient);
				}

				EndChild();
			}

			Text("Appearance");
			static float colours[3] = { 0.1f, 0.2f, 0.5f };
			ColorPicker3("body colour", colours);
			mCarModelShader.bind();
			mCarModelShader.setUniform(7, glm::vec3(colours[0], colours[1], colours[2]));
		}

		End();
	}

	void UILayer::driverInfo() const
		/* Called by UILayer::render
		 * Displays at-a-glance information about the car
		 * Defines the structure of the driver information window
		*/
	{
		using namespace ImGui;

		ImVec4
			red = ImVec4(1.0f, 0.0f, 0.0f, 1.0f),
			green = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);

		Begin("Vehicle overview");

		//Speed info
		{
			double speedMetPerSec = glm::length(mDataSource.getState().getVelocity_world());
			Text("Speed (mph): %.1f", speedMetPerSec * 2.23694);
			Text("Speed (kph): %.1f", speedMetPerSec * 3.6);
		}

		//Drive mode info
		{
			Text("Engine mode: "); SameLine();
			bool reverseMode = mDataSource.getTorqueGenerator().reverseModeOn();
			PushStyleColor(ImGuiCol_Text, reverseMode ? red : green);
			Text("%s", reverseMode ? "REVERSE" : "FORWARD");
			PopStyleColor();
		}

		Internal::ControlSystem& controls = mDataSource.getControlSystem();

		//Brake info
		{
			Text("Brakes: "); SameLine();
			bool brakesOn = controls.brakesOn();
			PushStyleColor(ImGuiCol_Text, brakesOn ? green : red);
			Text("%s", brakesOn ? "ON" : "OFF");
			PopStyleColor();
		}

		//Steering info
		Text("Steering wheel angle: %.3f", controls.getSteeringWheelAngle());

		End();
	}

	void UILayer::helpInfo() const
		/* Called by UILayer::render
		 * Available by default when the application starts
		*/
	{
		using namespace ImGui;

		Begin("Help");
		{
			float childWidth = GetContentRegionAvailWidth();

			Text("Keyboard controls");
			BeginChild("Keyboard controls", ImVec2(childWidth, 180.0f), true);
			{
				std::string info =
					"ESC         = quit\n"
					"UP ARROW    = accelerate\n"
					"DOWN ARROW  = brake\n"
					"LEFT ARROW  = steering wheel left\n"
					"RIGHT ARROW = steering wheel right\n"
					"C           = cycle cameras\n"
					"R           = reset vehicle\n"
					"END         = toggle engine mode\n"
					"W           = free camera forwards\n"
					"A           = free camera left\n"
					"S           = free camera backwards\n"
					"D           = free camera right\n"
					"SPACE       = free camera up\n"
					"SHIFT       = free camera down\n"
					"0           = toggle wireframe\n";

				Text("%s", info.c_str());
				EndChild();
			}

			Text("Application controls");
			BeginChild("Application controls", ImVec2(childWidth, 270.0f), true);
			{
				TextWrapped("Windows can be shown/hidden by double clicking their title bars");
				ImGui::Spacing();
				TextWrapped("All windows apart from the control panel can be moved with click + drag.");
				ImGui::Spacing();
				TextWrapped("All windows apart from control panel can be resized by dragging the bottom right corner.");
				ImGui::Spacing();
				TextWrapped("Left click anywhere outside the menu windows to give mouse focus to the simulation cameras and hide the cursor.");
				ImGui::Spacing();
				TextWrapped("Right click anywhere to show the cursor and end camera mouse focus.");
				ImGui::Spacing();
				TextWrapped("The scroll wheel changes the field of view on the free camera and driver camera, if they have mouse focus");
				ImGui::Spacing();
				TextWrapped("The middle mouse button is used to reset the zoom on the free camera and driver camera.");

				EndChild();
			}
		}
		End();
	}

	void UILayer::carDebugInfo() const
		/* Called by UILayer::render
		 * Available when the DebugCarModel's is rendered
		*/
	{
		ImGui::Begin("Debug info");
		{
			debug_physicsTelemetry();
			debug_allWheelTelemetry();
		}
		ImGui::End();
	}

	void UILayer::debug_physicsTelemetry() const
		/* Called by UILayer::carDebugInfo
		 * Displays a child region within the debug window, that shows the core physical state of the Car
		*/
	{
		using namespace ImGui;

		Framework::Physics::State carState = mDataSource.getState();
		glm::dvec3 temp;

		Text("Physics state");
		BeginChild("State", ImVec2(0.0f, 250.0f), true);
		{
			temp = carState.getPosition_world();
			Text("Position\nx: %.3f y: %.3f z: %.3f", temp.x, temp.y, temp.z); Separator();

			temp = carState.getVelocity_world();
			Text("Linear Velocity\nx: %.3f y: %.3f z: %.3f", temp.x, temp.y, temp.z); Separator();

			float speed = glm::length(carState.getVelocity_world());
			Text("Speed\nm/h: %.3f k/h: %.3f m/s: %.3f", speed * 2.23694, speed * 3.6, speed); Separator();

			temp = mDataSource.getAcceleration_world();
			Text("Linear Acceleration\nx: %.3f y: %.3f z: %.3f", temp.x, temp.y, temp.z); Separator();

			temp = carState.getMomentum_world();
			Text("Momentum\nx: %.3f y: %.3f z: %.3f", temp.x, temp.y, temp.z); Separator();

			temp = carState.getAngularVelocity_world();
			Text("Angular Velocity\nx: %.3f y: %.3f z: %.3f", temp.x, temp.y, temp.z); Separator();

			temp = carState.getAngularMomentum_world();
			Text("Angular Momentum\nx: %.3f y: %.3f z: %.3f", temp.x, temp.y, temp.z);
		}
		EndChild();
	}

	void UILayer::debug_wheelTelemetry(unsigned char axlePos, unsigned char side, ImVec4 colour) const
		/* Called by UILayer::debug_allWheelTelemetry
		 * Adds ImGui::Text to a window detailing some properties of one Wheel
		*/
	{
		Internal::WheelInterface* temp = &mDataSource.getWheelSystem().getWheelInterface(Internal::WheelSystem::AxlePos(axlePos), Internal::WheelSystem::Side(side));

		ImGui::PushStyleColor(ImGuiCol_Text, colour);
		glm::dvec3 wheelPos_car = temp->getWheel().getPosition_car();
		ImGui::Text("Position_car:      (%.3f, %.3f, %.3f)", wheelPos_car.x, wheelPos_car.y, wheelPos_car.z);
		ImGui::Text("Long force:         %.3f", temp->getWheel().getTyre().getTotalForce_wheel().y);
		ImGui::Text("Lateral force:      %.3f", temp->getWheel().getTyre().getTotalForce_wheel().x);
		ImGui::Text("Load:               %.3f", temp->getLoad());
		ImGui::Text("Longitudinal slip:  %.3f", temp->getWheel().getTyre().getSlip().getLongitudinal());
		ImGui::Text("Lateral slip angle: %.3f", temp->getWheel().getTyre().getSlip().getAngle_degs());
		ImGui::PopStyleColor();
	}

	void UILayer::debug_allWheelTelemetry() const
		/* Called by UILayer::carDebugInfo
		 * Combines the text provided by 4 calls to debug_wheelTelemetry, into a 2 x 2 grid and displays this in a child region
		*/
	{
		using namespace Internal;
		using namespace ImGui;

		Text("Wheel info");
		BeginChild("Wheel info", ImVec2(0.0f, 265.0f), true);
		{
			Columns(2);

			Text("Front left");
			debug_wheelTelemetry(WheelSystem::AxlePos::FRONT, WheelSystem::Side::LEFT, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));

			NextColumn();
			Text("Front right");
			Separator();
			debug_wheelTelemetry(WheelSystem::AxlePos::FRONT, WheelSystem::Side::RIGHT, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));

			Separator();

			NextColumn();
			Text("Rear left");
			debug_wheelTelemetry(WheelSystem::AxlePos::REAR, WheelSystem::Side::LEFT, ImVec4(0.0f, 1.0f, 1.0f, 1.0f));

			NextColumn();
			Text("Rear right");
			Separator();
			debug_wheelTelemetry(WheelSystem::AxlePos::REAR, WheelSystem::Side::RIGHT, ImVec4(1.0f, 0.0f, 1.0f, 1.0f));
		}
		EndChild();
	}

	void UILayer::tyreParameters() const
		/* Called by UILayer::render
		 * Enables individual tyre parameter alteration
		*/
	{
		using namespace ImGui;
		using namespace Internal;

		Begin("Pacejka magic formula parameters");

		if (Button("Drifting tyres"))
			PacejkaMagicFormula::setToDriftingTyreParams();
		if (Button("Standard tyres"))
			PacejkaMagicFormula::setToRoadTyreParams();

		//Longitudinal
		Text("Longitidinal parameters");
		BeginChild("longitudinal params", ImVec2(0, 335.0f), true);
		{
			SliderFloat("b0 Shape factor", &PacejkaMagicFormula::b0, 1.4f, 1.8f);
			SliderFloat("b1 Load influence on longitudinal friction coefficient", &PacejkaMagicFormula::b1, -80.0f, 80.0f);
			SliderFloat("b2 Longitudinal friction coefficient", &PacejkaMagicFormula::b2, 900.0f, 1700.0f);
			SliderFloat("b3 Curvature factor of stiffness/load", &PacejkaMagicFormula::b3, -20.0f, 20.0f);
			SliderFloat("b4 Change of stiffness with slip", &PacejkaMagicFormula::b4, 100.0f, 500.0f);
			SliderFloat("b5 Change of progressivity of stiffness/load", &PacejkaMagicFormula::b5, -1.0f, 1.0f);
			SliderFloat("b6 Curvature change with load^2", &PacejkaMagicFormula::b6, -0.1f, 0.1f);
			SliderFloat("b7 Curvature change with load", &PacejkaMagicFormula::b7, -1.0f, 1.0f);
			SliderFloat("b8 Curvature factor", &PacejkaMagicFormula::b8, -20.0f, 1.0f);
			SliderFloat("b9 Load influence on horizontal shift", &PacejkaMagicFormula::b9, -1.0f, 1.0f);
			SliderFloat("b10 Horizontal shift", &PacejkaMagicFormula::b10, -5.0f, 5.0f);
			SliderFloat("b11 Vertical shift", &PacejkaMagicFormula::b11, -100.0f, 100.0f);
			SliderFloat("b12 Vertical shift at load = 0", &PacejkaMagicFormula::b12, -10.0f, 10.0f);
			SliderFloat("b13 Curvature shift", &PacejkaMagicFormula::b13, -1.0f, 1.0f);
		}
		EndChild();

		//Lateral
		Text("Lateral parameters");
		BeginChild("lateral params", ImVec2(0, 425.0f), true);
		{
			SliderFloat("a0 Shape factor", &PacejkaMagicFormula::a0, 1.2f, 1.8f);
			SliderFloat("a1 Load influence on lateral friction coefficient", &PacejkaMagicFormula::a1, -80.0f, 80.0f);
			SliderFloat("a2 Lateral friction coefficient", &PacejkaMagicFormula::a2, 900.0f, 1700.0f);
			SliderFloat("a3 Change of stiffness with slip", &PacejkaMagicFormula::a3, 500.0f, 2000.0f);
			SliderFloat("a4 Change of progressivity of stiffness / load", &PacejkaMagicFormula::a4, 0.0f, 50.0f);
			SliderFloat("a5 Camber influence on stiffness", &PacejkaMagicFormula::a5, -0.1f, 1.0f);
			SliderFloat("a6 Curvature change with load", &PacejkaMagicFormula::a6, -2.0f, 2.0f);
			SliderFloat("a7 Curvature factor", &PacejkaMagicFormula::a7, -20.0f, 1.0f);
			SliderFloat("a8 Load influence on horizontal shift", &PacejkaMagicFormula::a8, -1.0f, 1.0f);
			SliderFloat("a9 Horizontal shift at load = 0 and camber = 0", &PacejkaMagicFormula::a9, -1.0f, 1.0f);
			SliderFloat("a10 Camber influence on horizontal shift", &PacejkaMagicFormula::a10, -0.1f, 0.1f);
			SliderFloat("a11 Vertical shift", &PacejkaMagicFormula::a11, -200.0f, 200.0f);
			SliderFloat("a12 Vertical shift at load = 0", &PacejkaMagicFormula::a12, -10.0f, 10.0f);
			SliderFloat("a13 Camber influence on vertical shift, load dependent	", &PacejkaMagicFormula::a13, -10.0f, 10.0f);
			SliderFloat("a14 Camber influence on vertical shift", &PacejkaMagicFormula::a14, -15.0f, 15.0f);
			SliderFloat("a15 Camber influence on lateral friction coefficient", &PacejkaMagicFormula::a15, -0.01f, 0.01f);
			SliderFloat("a16 Curvature change with camber", &PacejkaMagicFormula::a16, -0.1f, 0.1f);
			SliderFloat("a17 Curvature shift", &PacejkaMagicFormula::a17, -1.0f, 1.0f);
		}
		EndChild();
		End();
	}

	void UILayer::upsideDownWarning() const
		/* Called by UILayer::render
		 * If the user flips the Car over, this warning prompts them to reset it
		*/
	{
		glm::dvec3 carPosition_world = mDataSource.getState().getPosition_world();

		double terrainHeight = External::Environment::mTerrain.getHeight(glm::dvec2(carPosition_world.x, carPosition_world.y));

		bool displayWarning =
			glm::dvec3(mDataSource.getState().getLocalToWorld_direction() * glm::dvec4(0.0, 1.0, 0.0, 1.0)).y < 0.0
			&& (abs(carPosition_world.y - terrainHeight)) < 2.0;

		if (displayWarning)
			ImGui::OpenPopup("Reset vehicle");

		if (ImGui::BeginPopupModal("Reset vehicle", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove))
		{
			ImGui::Text("Your vehicle is upside down.\nPress R to reset.\n");
			if (!displayWarning)
				ImGui::CloseCurrentPopup();
			ImGui::EndPopup();
		}
	}

}