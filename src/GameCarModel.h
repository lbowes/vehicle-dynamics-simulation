/* CLASS OVERVIEW
 * - A visual representation of the physical state of a Car object
 * - One of two classes that inherits from ICarModel
*/

#ifndef GAMECARMODEL_H
#define GAMECARMODEL_H
#pragma once

#include <glm/glm/vec3.hpp>

#include "ICarModel.h"

namespace Visual {
	class GameCarModel : public ICarModel {
	public:
		GameCarModel(Internal::Car& carData, Framework::ResourceSet& resourceBucket);
		~GameCarModel() = default;

		void loadResources();
		void setShaderUniforms(float fogDensity, float fogGradient, glm::vec3 skyColour, glm::vec3 sunDirection);
		void update();

	};
}

#endif
