/* CLASS OVERVIEW
 * - Responsible for generating, storing, and providing acces to 3 buffers of data (mHeights, mNormals, mSurfaceTypes)
 * - Generation of these buffers uses multiple TerrainGenLayer objects
 * - The majority of the code in this class is executed at load-time
*/

#ifndef TERRAIN_H
#define TERRAIN_H
#pragma once

#include <vector>
#include <memory>
#include <glm/glm/vec3.hpp>
#include <glm/glm/vec2.hpp>
#include <glm/glm/geometric.hpp>
#include <glm/glm/common.hpp>
#include <Framework/Maths/Maths.hpp>

#include "TerrainGenLayers.hpp"
#include "Track.h"

namespace Visual {
	class TerrainModel;
}

namespace External {
	class Terrain {
		friend class Visual::TerrainModel;
	private:
		//Width of the terrain square in height sample points e.g. 7 for 6m x 6m terrain.
		const unsigned short mSize = 291;

		std::vector<double> mHeights;
		std::vector<glm::dvec3> mNormals;
		std::vector<unsigned char> mSurfaceTypes;
		std::vector<std::unique_ptr<TerrainGenLayer>> mGenerationLayers;

	public:
		Terrain();
		~Terrain() = default;

		void generate();
		double getHeight(glm::dvec2 horizontalSamplePoint);
		glm::dvec3 getNormal_world(glm::dvec2 horizontalSamplePoint);

		inline const unsigned short getSize() const { return mSize; }

	private:
		void generateHeightData();
		void generateNormalData();
		void generateSurfaceTypeData();
		unsigned int calc_PerTriAttribute_Index(glm::dvec2 horizontalSamplePoint);

	};
}

#endif
