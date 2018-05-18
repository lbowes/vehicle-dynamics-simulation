/* CLASS OVERVIEW
 * Provides a debugging *overlay* model for the Car object
 * Used to visualise physical quantities
*/

#ifndef DEBUGCARMODEL_H
#define DEBUGCARMODEL_H
#pragma once

#include <Framework/ImGui/imgui.h>

#include "ICarModel.h"
#include "DebugVectorGroup.h"

namespace Visual {
	class DebugCarModel : public ICarModel {
	public:
		DebugCarModel(Internal::Car& carData, Framework::ResourceSet& resourceBucket);
		~DebugCarModel() = default;

		virtual void render(Framework::Graphics::Renderer& renderer) override;

	private:
		glm::vec4
			mWheelNeutralColour = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),
			mWheelCollidingColour = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);

		const unsigned int
			mWheelNumSides = 16,
			mNumVectorsPerWheelInterface = 4;

		std::vector<float>
			mWheelNeutralColourData,
			mWheelCollidingColourData;

		DebugVectorGroup mVectorGroup;
		
	private:
		void addVectorLines();
		void loadResources();
		void populateWheelColourBufferData();
		void carBaseResources();
		void carWheelsResources();
		void loadVectorLines();
		void update();
		void updateVectorLines();

	};
}

#endif
