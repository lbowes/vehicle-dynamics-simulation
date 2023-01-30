/* CLASS OVERVIEW
 * Visual model to represent the internal shape of the terrain
 * Contains a skybox, colours and fog/sun parameters to enhance the appearance of the environment
*/

#ifndef ENVIRONMENTMODEL_H
#define ENVIRONMENTMODEL_H
#pragma once

#include <glm/glm/gtx/rotate_vector.hpp>
#include <glm/glm/gtx/vector_angle.hpp>
#include <glm/glm/gtx/compatibility.hpp>
#include <Framework/Graphics/Renderer/Renderer.h>
#include <Framework/Core/ResourceSet.h>
#include <Framework/Objects/Model3D.h>
#include <Framework/Camera/PerspectiveCamera.h>
#include <Framework/Graphics/SkyBox.h>

#include "TerrainModel.h"

#define FOG 1

namespace Visual {
	class EnvironmentModel {
	private:
		Framework::ResourceSet& mResourceBucket;
		Framework::Model3D mModel;
		std::unique_ptr<Framework::Graphics::SkyBox> mSkyBox;
		std::unique_ptr<TerrainModel> mTerrainModel;

		Framework::Graphics::Shader
			*mTerrainShader = nullptr,
			*mSkyboxShader = nullptr;

		const float
			mFogDensity = FOG ? 0.005f : 0.0f, //0.005f
			mFogGradient = 2.0f;  //2.0f

		const glm::vec3
			mSkyColour = glm::vec3(0.4823529411764706f, 0.6549019607843137f, 0.8588235294117647f),
			mSunColour = glm::vec3(1.0f),
			mSunDirection = glm::normalize(glm::vec3(10.0f, 5.0f, 0.0f));

	public:
		EnvironmentModel(Framework::ResourceSet& resourceBucket);
		~EnvironmentModel() = default;

		void render(Framework::Graphics::Renderer& renderer);
		inline float getFogDensity() const { return mFogDensity; }
		inline float getFogGradient() const { return mFogGradient; }
		inline glm::vec3 getSkyColour() const { return mSkyColour; }
		inline glm::vec3 getSunColour() const { return mSunColour; }
		inline glm::vec3 getSunDirection() const { return mSunDirection; }

	private:
		void loadResources();

	};
}

#endif
