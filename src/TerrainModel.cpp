#include "TerrainModel.h"

namespace Visual {

	TerrainModel::TerrainModel(Framework::ResourceSet& resourceBucket, Framework::Graphics::Shader* terrainShader) :
		/* Called by EnvironmentModel::loadResources
		*/
		mResourceBucket(resourceBucket),
		mShader(terrainShader)
	{
		loadModel();
	}

	void TerrainModel::render(Framework::Graphics::Renderer& renderer) {
		mModel.sendRenderCommands(renderer);
	}

	void TerrainModel::loadModel() 
		/* Called by TerrainModel::TerrainModel
		*/
	{
		using namespace Framework::Graphics;
		
		//This function needs to load the model with procedurally generated terrain

		std::vector<float> positionData;
		fillWithPositionData(positionData);

		std::vector<float> normalData;
		fillWithNormalData(normalData);

		std::vector<float> colourData;
		fillWithColourData(colourData);

		std::vector<unsigned int> indexData;
		fillWithIndexData(indexData);

		//Add an(empty) mesh for the terrain
		Mesh* terrainMesh = mResourceBucket.addMesh("terrainMesh", GL_TRIANGLES, nullptr, mShader);

		//Add a vertex buffer for terrain positions
		mResourceBucket.addVertexBuffer(GL_STATIC_DRAW, VertexFormat(0, 3, GL_FLOAT, false), positionData, "terrainPositions");

		//Add a vertex buffer for terrain positions
		mResourceBucket.addVertexBuffer(GL_STATIC_DRAW, VertexFormat(1, 3, GL_FLOAT, false), normalData, "terrainNormals");

		//Add a vertex buffer for terrain positions
		mResourceBucket.addVertexBuffer(GL_STATIC_DRAW, VertexFormat(2, 3, GL_FLOAT, false), colourData, "terrainColours");

		//Add an index buffer for terrain indices
		mResourceBucket.addIndexBuffer(GL_STATIC_DRAW, indexData, "terrainIndices");

		//Add buffers to mesh
		terrainMesh->addBuffer(mResourceBucket.getResource<VertexBuffer>("terrainPositions"));
		terrainMesh->addBuffer(mResourceBucket.getResource<VertexBuffer>("terrainNormals"));
		terrainMesh->addBuffer(mResourceBucket.getResource<VertexBuffer>("terrainColours"));
		terrainMesh->addIndexBuffer(mResourceBucket.getResource<IndexBuffer>("terrainIndices"));

		mModel.addMesh(terrainMesh);
	}

	void TerrainModel::fillWithPositionData(std::vector<float>& toFill) 
		/* Called by TerrainModel::loadModel
		*/
	{
		/*
		 * Vector comes in empty, and leaves full of position data
		 *
		 * Uses the following coordinate system for x and z
		 *
		 *       -z
		 *        |
		 * -x --- + --- +x
		 *        |
		 *       +z
		*/

		using namespace External;

		const int halfTerrainSize = floor(0.5 * Environment::mTerrain.getSize());

		//These represent the terrain heights at each of the four corners of the current terrain square being inspected
		double
			BL = 0.0,
			TL = 0.0,
			TR = 0.0,
			BR = 0.0;

		for (int x = -halfTerrainSize; x < halfTerrainSize; x++) {
			for (int z = -halfTerrainSize; z < halfTerrainSize; z++) {
				//An (x,z) coordinate in here sits on the bottom left (origin) of one of the terrain squares.
				//This can be used to add the position data.

				//First get the heights
				BL = Environment::mTerrain.getHeight(glm::dvec2(x, z));
				TL = Environment::mTerrain.getHeight(glm::dvec2(x, z + 1.0));
				TR = Environment::mTerrain.getHeight(glm::dvec2(x + 1.0, z + 1.0));
				BR = Environment::mTerrain.getHeight(glm::dvec2(x + 1.0, z));

				//Now add each position to the buffer

				//v0
				toFill.push_back(x);
				toFill.push_back(BL);
				toFill.push_back(z);

				//v1
				toFill.push_back(x);
				toFill.push_back(TL);
				toFill.push_back(z + 1.0);

				//v2
				toFill.push_back(x + 1.0);
				toFill.push_back(TR);
				toFill.push_back(z + 1.0);

				//v2
				toFill.push_back(x + 1.0);
				toFill.push_back(TR);
				toFill.push_back(z + 1.0);

				//v3
				toFill.push_back(x + 1.0);
				toFill.push_back(BR);
				toFill.push_back(z);

				//v0
				toFill.push_back(x);
				toFill.push_back(BL);
				toFill.push_back(z);
			}
		}
	}

	void TerrainModel::fillWithNormalData(std::vector<float>& toFill) 
		/* Called by TerrainModel::loadModel
		 * Iterates over the terrain, and retrieves the surface normal vector for each triangle before adding it to the buffer passed in
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
		*   ___   
		*  |  / /|
		*  |L/ /R|
		*  |/ /__|
		*    	      
		*/

		using namespace glm;
		using namespace External;

		const int 
			terrainSize = Environment::mTerrain.getSize(),
			halfTerrainSize = floor(0.5 * terrainSize);

		vec3
			leftTriangleNormal,
			rightTriangleNormal;

		int
			normalsArrayIndexX = 0, 
			heightsIndex = 0;       

		for (int x = -halfTerrainSize; x < halfTerrainSize; x++) {
			for (int z = -halfTerrainSize; z < halfTerrainSize; z++) {
				heightsIndex = (x + halfTerrainSize) * terrainSize + (z + halfTerrainSize);
				normalsArrayIndexX = floor(heightsIndex / terrainSize);
				
				leftTriangleNormal = Environment::mTerrain.mNormals[2 * (heightsIndex - normalsArrayIndexX)];
				rightTriangleNormal = Environment::mTerrain.mNormals[2 * (heightsIndex - normalsArrayIndexX) + 1];

				//v0
				toFill.push_back(leftTriangleNormal.x);
				toFill.push_back(leftTriangleNormal.y);
				toFill.push_back(leftTriangleNormal.z);

				//v1
				toFill.push_back(leftTriangleNormal.x);
				toFill.push_back(leftTriangleNormal.y);
				toFill.push_back(leftTriangleNormal.z);

				//v2
				toFill.push_back(leftTriangleNormal.x);
				toFill.push_back(leftTriangleNormal.y);
				toFill.push_back(leftTriangleNormal.z);

				//v3
				toFill.push_back(rightTriangleNormal.x);
				toFill.push_back(rightTriangleNormal.y);
				toFill.push_back(rightTriangleNormal.z);

				//v4
				toFill.push_back(rightTriangleNormal.x);
				toFill.push_back(rightTriangleNormal.y);
				toFill.push_back(rightTriangleNormal.z);

				//v5
				toFill.push_back(rightTriangleNormal.x);
				toFill.push_back(rightTriangleNormal.y);
				toFill.push_back(rightTriangleNormal.z);
			}
		}
	}

	void TerrainModel::fillWithColourData(std::vector<float>& toFill) 
		/* Called by TerrainModel::loadModel
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
		*   ___
		*  |  / /|
		*  |L/ /R|
		*  |/ /__|
		*
		*/

		using namespace glm;
		using namespace External;

		const int
			terrainSize = Environment::mTerrain.getSize(),
			halfTerrainSize = floor(0.5 * terrainSize);

		vec3
			leftTriangleColour,
			rightTriangleColour;

		int
			surfaceTypesArrayIndexX = 0,
			surfaceTypesArrayIndexZ = 0,
			heightsIndex = 0;

		vec3 terrainSurfaceColours[3];
		terrainSurfaceColours[External::TerrainType::GRASS] = glm::vec3(0.2509803921568627f, 0.3529411764705882f, 0.1215686274509804f);
		terrainSurfaceColours[External::TerrainType::TARMAC] = glm::vec3(0.3725490196078431f, 0.2509803921568627f, 0.1411764705882353f);
		terrainSurfaceColours[External::TerrainType::ERROR_TYPE] = glm::vec3(1.0f, 0.0f, 0.0f);

		for (int x = -halfTerrainSize; x < halfTerrainSize; x++) {
			for (int z = -halfTerrainSize; z < halfTerrainSize; z++) {
				heightsIndex = (x + halfTerrainSize) * terrainSize + (z + halfTerrainSize);
				surfaceTypesArrayIndexX = floor(heightsIndex / terrainSize);
				surfaceTypesArrayIndexZ = heightsIndex - terrainSize * surfaceTypesArrayIndexX;

				leftTriangleColour = terrainSurfaceColours[Environment::mTerrain.mSurfaceTypes[2 * (heightsIndex - surfaceTypesArrayIndexX)]];
				rightTriangleColour = terrainSurfaceColours[Environment::mTerrain.mSurfaceTypes[2 * (heightsIndex - surfaceTypesArrayIndexX) + 1]];

				//Add some distortion
				double distortionLevel = 0.08;
				leftTriangleColour += vec3(distortionLevel) * ((((float)rand() / RAND_MAX) * 2.0f - 1.0f) / 2.0f);
				rightTriangleColour += vec3(distortionLevel) * ((((float)rand() / RAND_MAX) * 2.0f - 1.0f) / 2.0f);

				//v0
				toFill.push_back(leftTriangleColour.x);
				toFill.push_back(leftTriangleColour.y);
				toFill.push_back(leftTriangleColour.z);

				//v1
				toFill.push_back(leftTriangleColour.x);
				toFill.push_back(leftTriangleColour.y);
				toFill.push_back(leftTriangleColour.z);

				//v2
				toFill.push_back(leftTriangleColour.x);
				toFill.push_back(leftTriangleColour.y);
				toFill.push_back(leftTriangleColour.z);

				//v3
				toFill.push_back(rightTriangleColour.x);
				toFill.push_back(rightTriangleColour.y);
				toFill.push_back(rightTriangleColour.z);

				//v4
				toFill.push_back(rightTriangleColour.x);
				toFill.push_back(rightTriangleColour.y);
				toFill.push_back(rightTriangleColour.z);

				//v5
				toFill.push_back(rightTriangleColour.x);
				toFill.push_back(rightTriangleColour.y);
				toFill.push_back(rightTriangleColour.z);
			}
		}
	}

	void TerrainModel::fillWithIndexData(std::vector<unsigned int>& toFill) 
		/*Called by TerrainModel::loadModel
		*/
	{
		for (unsigned int i = 0; i < pow(External::Environment::mTerrain.getSize() - 1, 2) * 6; i++)
			toFill.push_back(i);
	}

}