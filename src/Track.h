/* CLASS OVERVIEW
 * - Imprints the recessed shape and surface of a race track into the terrain
 * - All code in this class is executed at load-time
*/

#ifndef TRACK_H
#define TRACK_H
#pragma once

#include <algorithm>
#include <glm/glm/gtx/rotate_vector.hpp>
#include <glm/glm/trigonometric.hpp>
#include <glm/glm/geometric.hpp>
#include <glm/glm/gtc/constants.hpp>
#include <Framework/Maths/Noise.h>

#include "TerrainGenLayers.hpp"

namespace External {
	class Track : public TerrainGenLayer {
	private:
		struct PilotRunResults {
			glm::dvec2
				mShapeDimensions,
				mLowerBoundDisplacement,
				mUpperBoundDisplacement;

			inline bool shapeIsLandscape() { return mShapeDimensions.x > mShapeDimensions.y; }
		};

		PilotRunResults mPilotResults;

		std::vector<glm::dvec2> mPoints_graph;

		std::vector<unsigned char> mTempSurfaceTypes;

		const unsigned int
			mTerrainSize_heightSamples = 0,
			mNumSamplesOverTotal = 2000, //10000
			mWidth = 20,
			mTerrainBorderPadding = 10;

		unsigned int mSizeLimit = 0;

		const double mMaxDepth = 2.0;

		double
			mPilotToMainScaleFactor = 0.0,
			mHeightCounter = 0.0;

		const glm::dvec2 mStartDirection = glm::dvec2(1.0, 0.0);

		glm::dvec2 mStartPosition = glm::dvec2(0.0);

	public:
		Track(const unsigned int terrainSize_heightSamples);
		~Track() = default;

		virtual void runHeights(std::vector<double>& previousLayerHeights);
		virtual void runSurfaceTypes(std::vector<unsigned char>& previousLayerSurfaceTypes);

	private:
		void addAllPoints();
		void addPoint_graph(double percent, double angle);
		double lookUpAngleAtPercent_graph(double percent);
		void imprintCircle(std::vector<double>& toImprintHeights, int centreX, int centreZ);
		void runPilotVersion();
		void updateStartingPosition();

	};
}

#endif
