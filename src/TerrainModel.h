/* CLASS OVERVIEW
 * Generates a graphical model of the Terrain, to be rendered within the Environment
*/

#ifndef TERRAINMODEL_H
#define TERRAINMODEL_H
#pragma once

#include <vector>
#include <Framework/Graphics/Renderer/Renderer.h>
#include <Framework/Core/ResourceSet.h>
#include <Framework/Objects/Model3D.h>

#include "Environment.h"

namespace Visual {
	class TerrainModel {
	private:
		Framework::ResourceSet& mResourceBucket;
		Framework::Model3D mModel;
		Framework::Graphics::Shader* mShader = nullptr;

	public:
		TerrainModel(Framework::ResourceSet& resourceBucket, Framework::Graphics::Shader* terrainShader);
		~TerrainModel() = default;

		void render(Framework::Graphics::Renderer& renderer);

	private:
		void loadModel();
		void fillWithPositionData(std::vector<float>& toFill);
		void fillWithNormalData(std::vector<float>& toFill);
		void fillWithColourData(std::vector<float>& toFill);
		void fillWithIndexData(std::vector<unsigned int>& toFill);

	};
}

#endif
