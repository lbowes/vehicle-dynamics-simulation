#include "EnvironmentModel.h"

namespace Visual {

	EnvironmentModel::EnvironmentModel(Framework::ResourceSet& resourceBucket) :
		/* Called by VisualShell::load
		*/
		mResourceBucket(resourceBucket)
	{
		loadResources();
	}

	void EnvironmentModel::render(Framework::Graphics::Renderer& renderer)
		/* Called by VisualShell::renderAll
		 * Renders everything in the Environment
		*/
	{
		mTerrainModel->render(renderer);
		mSkyBox->render(renderer);
	}

	void EnvironmentModel::loadResources()
		/* Called by EnvironmentModel::EnvironmentModel
		 * Instantiation and initialisation of all resources needed to represent the environment graphically
		*/
	{
		//Setting uniforms in the terrain shader
		mTerrainShader = mResourceBucket.addShader("res/shaders/terrain.vert", "res/shaders/terrain.frag", "terrainShader");
		mTerrainShader->addUniform("modelMatrix");
		mTerrainShader->addUniform("viewMatrix");
		mTerrainShader->addUniform("projectionMatrix");
		mTerrainShader->addUniformWithDefault("sunDirection", mSunDirection);
		mTerrainShader->addUniformWithDefault("fogDensity", mFogDensity);
		mTerrainShader->addUniformWithDefault("fogGradient", mFogGradient);
		mTerrainShader->addUniformWithDefault("skyColour", mSkyColour);

		//TerrainModel object
		mTerrainModel = std::make_unique<TerrainModel>(mResourceBucket, mTerrainShader);

		//SkyBox object
		mSkyBox = std::make_unique<Framework::Graphics::SkyBox>("res/shaders/skyBox.vert", "res/shaders/skyBox.frag");

		//Settings uniforms in the SkyBox shader
		mSkyboxShader = mSkyBox->getShader();
		mSkyboxShader->addUniformWithDefault("skyColour", mSkyColour);
		mSkyboxShader->addUniformWithDefault("sunColour", mSunColour);
		mSkyboxShader->addUniformWithDefault("sunDir", mSunDirection);
	}

}