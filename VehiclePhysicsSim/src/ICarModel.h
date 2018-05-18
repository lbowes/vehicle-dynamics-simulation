/* CLASS OVERVIEW
 * A base class for GameCarModel and DebugCarModel
 * Stores a reference to a Car object, that is used as the source of all model transformation data
*/

#ifndef CARMODEL_H
#define CARMODEL_H
#pragma once

#include <Framework/Graphics/Renderer/Renderer.h>
#include <Framework/Core/ResourceSet.h>
#include <Framework/Objects/Model3D.h>
#include <Framework/Camera/PerspectiveCamera.h>

namespace Internal {
	class Car;
}

namespace Visual {
	class ICarModel {
	protected:
		Framework::ResourceSet& mResourceBucket;
		Framework::Model3D mModel;
		Internal::Car& mCarData;

	public:
		ICarModel(Internal::Car& carData, Framework::ResourceSet& resourceBucket);
		~ICarModel() = default;

		virtual void render(Framework::Graphics::Renderer& renderer);

	private:
		virtual void loadResources() = 0;
		virtual void update() = 0;
		

	};
}

#endif
