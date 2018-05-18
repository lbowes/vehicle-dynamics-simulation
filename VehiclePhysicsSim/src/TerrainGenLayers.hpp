/* CLASS(ES) OVERVIEW
* - A TerrainGenLayer adds one layer of modification to data buffers passed to it (heights and surface types).
* - RoughSand derives from TerrainGenLayer and modifies the heights buffer using Perlin noise
* - All code in this class is executed at load-time
*/

#ifndef TERRAINGENLAYERS_H
#define TERRAINGENLAYERS_H
#pragma once

#include <cmath>
#include <vector>
#include <chrono>
#include <Framework/Maths/Noise.h>
#include <glm/glm/vec2.hpp>
#include <glm/glm/gtc/constants.hpp>

namespace External {
	enum TerrainType { GRASS, TARMAC, ERROR_TYPE };

	class TerrainGenLayer {
	public:
		virtual void runHeights(std::vector<double>& previousLayerHeights) = 0;
		virtual void runSurfaceTypes(std::vector<unsigned char>& previousLayerSurfaceTypes) = 0;

	};

	class RoughGround : public TerrainGenLayer {
	public:
		virtual void runHeights(std::vector<double>& previousLayerHeights)
			/* Called by Terrain::generateHeightData
			* Modifies the data in previousLayerHeights by adding to it
			*/
		{
			//This ensures that the terrain is random for each run
			srand(time(NULL));

			const int
				terrainSize = sqrt(previousLayerHeights.size()),
				halfTerrainSize = 0.5 * terrainSize,
				randomXOffset = rand(),
				randomZOffset = rand();

			int currentHeightIndex = 0;

			double
				perlinCorrectX = 0.0, //Corrects for the fact that 1 Perlin noise 'unit' will be 16 meters
				perlinCorrectZ = 0.0, //^
				total = 0,            //A total value, used each iteration, to summate layers of Perlin noise
				amplitude = 0,		  //Used in the Perlin noise calculation
				frequency = 0;		  //^

			//Iterate over all heights in the buffer passed in
			for (int x = -halfTerrainSize; x <= halfTerrainSize; x++) {
				for (int z = -halfTerrainSize; z <= halfTerrainSize; z++) {
					currentHeightIndex = (x + halfTerrainSize) * terrainSize + (z + halfTerrainSize);

					perlinCorrectX = randomXOffset + x / 16.0;
					perlinCorrectZ = randomZOffset + z / 16.0;

					//Reset the total
					total = 0.0;

					//Add low frequency hills 
					amplitude = 8.0;
					frequency = 0.1;
					total += -abs(Framework::Maths::Noise::octavePerlin(perlinCorrectX * frequency, perlinCorrectZ * frequency, 3.0, 1.1) * amplitude);

					//Add higher frequency mounds of earth
					amplitude = 0.5;
					frequency = 1.0;
					total += -Framework::Maths::Noise::multiFractalRidged(perlinCorrectX * frequency, perlinCorrectZ * frequency, 2.0, 1.0, 1.0) * amplitude;

					//Set the current value in the buffer to this new total
					previousLayerHeights[(x + halfTerrainSize) * terrainSize + (z + halfTerrainSize)] = total;
				}
			}
		}

		virtual void runSurfaceTypes(std::vector<unsigned char>& previousLayerSurfaceTypes)
			/*Called by Terrain::generateSurfaceTypeData
			* The terrain is grass by default, so the RoughGround does not need to modify it
			*/
		{ }

	};

}

#endif
