#include "Terrain.h"

namespace External {

	Terrain::Terrain()
		/* Called in External::Environment
		*/
	{
		mGenerationLayers.push_back(std::make_unique<RoughGround>());
		mGenerationLayers.push_back(std::make_unique<Track>(mSize));

		generate();
	}

	void Terrain::generate()
		/* Called by Terrain::Terrain
		*/
	{
		//Must be called in the following order due to normals needing height data for their calculation
		generateHeightData();
		generateNormalData();
		generateSurfaceTypeData();
	}

	double Terrain::getHeight(glm::dvec2 horizontalSamplePoint)
		/* Called by
		 * - FPVCamera::afterPositionConstraints
		 * - Car::basicCollision
		 * - DebugCarModel::updateVectorLines
		 * - TerrainModel::fillWithPositionData
		 * - UILayer::upsideDownWarning
		 * - WheelInterface::update
		 * Calculates the height of the terrain at an arbitrary	2D position on it
		*/
	{
		/*       -z
		 *        |
		 * -x --- + --- +x
		 *        |
		 *       +z
		*/

		using namespace glm;

		//The horizontalSamplePoint will be a non-integer coordinate within the bounds of a terrain square.
		//The location of the target within this square is required
		const dvec2 withinSquare = horizontalSamplePoint - floor(horizontalSamplePoint);

		//The 3 vertices of the triangle that the target horizontalSamplePoint lies within
		dvec3
			vertex1,
			vertex2,
			vertex3;

		const int halfTerrainSize = floor(0.5 * mSize);

		//Indices into the heights array
		const unsigned int
			heightsIndexBL_X = horizontalSamplePoint.x < -halfTerrainSize ? 0 : horizontalSamplePoint.x > halfTerrainSize ? mSize - 2 : floor(horizontalSamplePoint.x) + halfTerrainSize,
			heightsIndexBL_Z = horizontalSamplePoint.y < -halfTerrainSize ? 0 : horizontalSamplePoint.y > halfTerrainSize ? mSize - 2 : floor(horizontalSamplePoint.y) + halfTerrainSize,
			actualHeightsIndexBL = heightsIndexBL_X * mSize + heightsIndexBL_Z;

		vertex1 = dvec3(0.0, mHeights[actualHeightsIndexBL], 0.0);

		//If point is on lower right triangle
		if(withinSquare.x >= withinSquare.y)
			vertex2 = dvec3(1.0, mHeights[actualHeightsIndexBL + mSize], 0.0);
		//If point is on upper left triangle
		else
			vertex2 = dvec3(0.0, mHeights[actualHeightsIndexBL + 1], 1.0);

		vertex3 = dvec3(1.0, mHeights[actualHeightsIndexBL + mSize + 1], 1.0);

		//Use barycentric interpolation to calculate the final height given the 3 vertices and a between them
		return Framework::Maths::barycentric(vertex1, vertex2, vertex3, withinSquare);
	}

	glm::dvec3 Terrain::getNormal_world(glm::dvec2 horizontalSamplePoint)
		  /* Called by
		     - DebugCarModel::updateVectorLines
			 - WheelInterface::update
			 Returns the surface normal vector at any arbitrary 2D position on the terrain
		  */
	{
		return mNormals[calc_PerTriAttribute_Index(horizontalSamplePoint)];
	}

	void Terrain::generateHeightData()
		/* Called by Terrain::generate
		 * Calculates and stores height data in mHeights
		*/
	{
		mHeights.resize(mSize * mSize, 0.0);

		for (const auto& layer : mGenerationLayers)
			layer->runHeights(mHeights);
	}

	void Terrain::generateNormalData()
		/* Called by Terrain::generate
		 * Calculates and stores normal vectors in mNormals
		*/
	{
		/*
		 *       -z
		 *        |
		 * -x --- + --- +x
		 *        |
		 *       +z
		 *
		 * Left triangle 'L' / right triangle 'R'
		 *
		 *    TL ___   TR
		 *      |  / /|
		 *      |L/ /R|
		 *      |/ /__|
		 *    BL	   BR
		*/

		using namespace glm;

		mNormals.resize(pow(mSize - 1, 2) * 2);

		dvec3
			ThisBLpoint,         //The currently inspected terrain point is considered as the bottom left of a square
			TLpoint,			 //The top left terrain point in this square
			TRpoint,			 //The top right terrain point
			BRpoint,			 //The bottom right terrain point
			leftTriangleNormal,	 //Temporary variables used each iteration to store the normal calculation results for both triangles
			rightTriangleNormal; //^

		const int halfTerrainSize = floor(0.5 * mSize);

		int
			XIndex = 0,   //If mHeights was treated as a 2D array, this would be the X index into this array ie. mHeights[XIndex][...]
			currentIndex = 0; //This is the actual index into the (1D) mHeights array (from the bottom left position)

		//Iterate over the bottom left points of all squares in the terrain
		for (int x = -halfTerrainSize; x < halfTerrainSize; x++) {
			for (int z = -halfTerrainSize; z < halfTerrainSize; z++) {

				//Calculate the index into mHeights that gets the height at the *current* position
				currentIndex = (x + halfTerrainSize) * mSize + (z + halfTerrainSize);

				//Use this index to calculate the X index of mHeights if it was a 2D array (with the bottom left item being [0][0])
				XIndex = floor(currentIndex / mSize);

				//Retrieve the height values that will be needed for the normal calculation
				ThisBLpoint = dvec3(0.0, mHeights[currentIndex],             0.0);
				TLpoint =     dvec3(0.0, mHeights[currentIndex + 1],         1.0);
				TRpoint =     dvec3(1.0, mHeights[currentIndex + mSize + 1], 1.0);
				BRpoint =     dvec3(1.0, mHeights[currentIndex + mSize],     0.0);

				//Use the height values to calculate the normal vectors for both halves (triangles) of the currently inspected terrain square
				leftTriangleNormal = normalize(cross(TRpoint - TLpoint, ThisBLpoint - TLpoint));
				rightTriangleNormal = normalize(cross(ThisBLpoint - BRpoint, TRpoint - BRpoint));

				//Insert these normal vectors into the correct positions in mNormals
				mNormals[2 * (currentIndex - XIndex)] = leftTriangleNormal;
				mNormals[2 * (currentIndex - XIndex) + 1] = rightTriangleNormal;
			}
		}
	}

	void Terrain::generateSurfaceTypeData()
		/* Called by Terrain::generate
		* Calculates and stores terrain surface types in mSurfaceTypes
		*/
	{
		mSurfaceTypes.resize(pow(mSize - 1, 2) * 2);

		for (const auto& layer : mGenerationLayers)
			layer->runSurfaceTypes(mSurfaceTypes);
	}

	unsigned int Terrain::calc_PerTriAttribute_Index(glm::dvec2 horizontalSamplePoint)
		/* Called by Terrain::getNormal_world
		 * Maps any arbitrary 2D position in space, onto (the index of) the triangle it is contained within.
		 * Works for mSurfaceTypes and mNormals (both are one-per-triangle)
		*/
	{
		/*       -y
		 *        |
		 * -x --- + --- +x
		 *        |
		 *       +y
		*/

		//Used to determine which triangle (half of a terrain square) the horizontalSamplePoint lies within
		const glm::dvec2 withinSquare = horizontalSamplePoint - floor(horizontalSamplePoint);

		const int halfTerrainSize = floor(0.5 * mSize);

		//Indices into the per-triangle-attribute array
		const unsigned int
			heightsIndexBL_X = horizontalSamplePoint.x < -halfTerrainSize ? 0 : horizontalSamplePoint.x > halfTerrainSize ? mSize - 2 : floor(horizontalSamplePoint.x) + halfTerrainSize,
			heightsIndexBL_Z = horizontalSamplePoint.y < -halfTerrainSize ? 0 : horizontalSamplePoint.y > halfTerrainSize ? mSize - 2 : floor(horizontalSamplePoint.y) + halfTerrainSize,
			actualHeightsIndexBL = heightsIndexBL_X * mSize + heightsIndexBL_Z;

		return 2 * (actualHeightsIndexBL - heightsIndexBL_X) + (withinSquare.x > withinSquare.y);
	}

}