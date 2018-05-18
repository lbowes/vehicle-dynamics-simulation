#include "ICarModel.h"
#include "Car.h"

namespace Visual {

	ICarModel::ICarModel(Internal::Car& carData, Framework::ResourceSet& resourceBucket) :
		/* Called by
		 * - GameCarModel::GameCarModel
		 * - DebugCarModel::DebugCarModel
		*/
		mResourceBucket(resourceBucket),
		mCarData(carData)
	{ }

	void ICarModel::render(Framework::Graphics::Renderer& renderer) 
		/* Called by VisualShell::renderAll
		*/
	{
		update();
		mModel.sendRenderCommands(renderer);
	}

}