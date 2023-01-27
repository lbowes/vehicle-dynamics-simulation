#include "Track.h"

#define CIRCULAR_TRACK      0
#define COMPLEX_TRACK       1
#define SQUARE_TRACK        0
#define ALMOST_SQUARE_TRACK 0
#define P_SHAPED_TRACK      0

namespace External {

	Track::Track(const unsigned int terrainSize_heightSamples) :
		/* Called by Terrain::Terrain
		 * Prepares the class for runHeights and runSurfaceTypes to be called, by running a pilot version of the track generation algorithm
		*/
		mTerrainSize_heightSamples(terrainSize_heightSamples)
	{
		mTempSurfaceTypes.resize(pow(terrainSize_heightSamples - 1, 2) * 2);

		addAllPoints();
		runPilotVersion();

		mSizeLimit = (mTerrainSize_heightSamples - 1) - (2 * mTerrainBorderPadding) - mWidth;
		mPilotToMainScaleFactor = mSizeLimit / std::max(mPilotResults.mShapeDimensions.x, mPilotResults.mShapeDimensions.y);

		updateStartingPosition();
	}

	void Track::runHeights(std::vector<double>& previousLayerHeights)
		/* Called by Terrain::generateHeightData
		 * Adds the track shape to the terrain height buffer passed in
		*/
	{
		const int
			terrainSize = sqrt(previousLayerHeights.size()),
			halfTerrainSize = 0.5 * terrainSize;

		int
			currentX = 0,
			currentZ = 0,
			lastX = 0,
			lastZ = 0;

		double currentAngle = 0.0;

		glm::dvec2
			positionTracker = mStartPosition,
			currentDirection = mStartDirection;

		//Iterate over the shape of the track by taking small steps along a changing direction vector
		for (unsigned int i = 0; i < mNumSamplesOverTotal; i++) {

			//Look up the angle of the 2D direction vector, clockwise from the -Z axis, at the current position on the track
			currentAngle = lookUpAngleAtPercent_graph((double)i / (double)mNumSamplesOverTotal);

			//Create a direction (unit) vector using this angle
			currentDirection = normalize(glm::rotate(mStartDirection, glm::radians(currentAngle)));

			//Take a step along the vector
			positionTracker += currentDirection * mPilotToMainScaleFactor;

			//If the track could sit inside the terrain at our position after taking this step...
			if (
				positionTracker.x > (-halfTerrainSize - 0.5 * mWidth) &&
				positionTracker.x < (halfTerrainSize + 0.5 * mWidth) &&
				positionTracker.y >(-halfTerrainSize - 0.5 * mWidth) &&
				positionTracker.y < (halfTerrainSize + 0.5 * mWidth))
			{
				//...then find the terrain square that our current position lies within
				currentX = floor(positionTracker.x);
				currentZ = floor(positionTracker.y);

				//If this is different to the previous square...
				if (currentX != lastX || currentZ != lastZ) {

					//...then imprint/'stamp' a circle with the track width as its diameter around this point and continue
					imprintCircle(previousLayerHeights, currentX, currentZ);
					lastX = currentX;
					lastZ = currentZ;
				}
			}
			else
				continue;
		}
	}

	void Track::runSurfaceTypes(std::vector<unsigned char>& previousLayerSurfaceTypes)
		/* Called by Terrain::generateSurfaceTypeData
		 * Sets the terrain surface types for the track
		*/
	{
		//To avoid unnecessarily iterating over the track twice, mTempSurfaceTypes is filled DURING the height iteration and then
		//just copied into previousLayerSurfaceTypes here.
		previousLayerSurfaceTypes = mTempSurfaceTypes;
	}

	void Track::addAllPoints()
		/* Called by Track::Track
		 * Defines the shape of a continuous loop (independent of position, orientation or scale)
		*/
	{
		//The angle of the tangent to the track (clockwise from the starting direction) is a function of the percentage along the track.
		//Points are added to a graph of this function, and linear interpolation between them is used to make it continuous.

		mPoints_graph.push_back(glm::dvec2(0.0, 0.0));

		//Multiple track shapes were used during testing, but 'COMPLEX_TRACK' is the final version used
		//in the application

#if COMPLEX_TRACK
		double
			pi = glm::pi<double>(),
			total = 0.0,
			L = 6.0 + (21.0 / 2.0) * pi;

		total += 0.5 * pi;
		addPoint_graph(total / L, 90.0); //b

		total += 3.0;
		addPoint_graph(total / L, 90.0); //c

		total += (3.0 / 2.0) * pi;
		addPoint_graph(total / L, 180.0); //d

		total += pi;
		addPoint_graph(total / L, 270.0); //e

		total += 0.5 * pi;
		addPoint_graph(total / L, 360.0); //f

		total += pi;
		addPoint_graph(total / L, 270.0); //g

		total += 0.5 * pi;
		addPoint_graph(total / L, 180.0); //h

		total += 0.5 * pi;
		addPoint_graph(total / L, 90.0); //i

		total += 0.5 * pi;
		addPoint_graph(total / L, 180.0); //j

		total += 3;
		addPoint_graph(total / L, 180.0); //k

		total += 0.5 * pi;
		addPoint_graph(total / L, 270.0); //l

		total += 0.5 * pi;
		addPoint_graph(total / L, 360.0); //m

		total += (3.0 / 2.0) * pi;
		addPoint_graph(total / L, 270.0); //n

		total += 0.5 * pi;
		addPoint_graph(total / L, 360.0); //o

		total += pi;
		addPoint_graph(total / L, 90.0); //p
#endif

#if P_SHAPED_TRACK
		addPoint_graph(0.242597, 180);
		addPoint_graph(0.485194, 0);
		addPoint_graph(0.588155, 0);
		addPoint_graph(0.588155, 270);
		addPoint_graph(0.897039, 270);
		addPoint_graph(0.897039, 180);
		addPoint_graph(1, 180);
#endif

#if ALMOST_SQUARE_TRACK
		addPoint_graph(0.25, 0);
		addPoint_graph(0.25, 90);
		addPoint_graph(0.375, 90);
		addPoint_graph(0.375, 180);
		addPoint_graph(0.5, 180);
		addPoint_graph(0.5, 90);
		addPoint_graph(0.625, 90);
		addPoint_graph(0.625, 180);
		addPoint_graph(0.75, 180);
		addPoint_graph(0.75, 270);
		addPoint_graph(1, 270);
#endif

#if SQUARE_TRACK
		addPoint_graph(0.25, 0.0);
		addPoint_graph(0.25, 90.0);
		addPoint_graph(0.5, 90.0);
		addPoint_graph(0.5, 180.0);
		addPoint_graph(0.75, 180.0);
		addPoint_graph(0.75, 270.0);
		addPoint_graph(1.0, 270.0);
#endif

		mPoints_graph.push_back(glm::dvec2(1.0, 360.0));
	}

	void Track::addPoint_graph(double percent, double angle)
		/* Called by Track::addAllPoints
		*/
	{
		if (percent >= 0.0 && percent <= 1.0 && angle >= 0.0 && angle <= 360.0)
			mPoints_graph.push_back(glm::dvec2(percent, angle));
		else
			printf("ERROR: Point out of bounds: (%g, %g)\n", percent, angle);
	}

	double Track::lookUpAngleAtPercent_graph(double percent)
		/* Called by
		 * - Track::runPilotVersion
		 * - Track::runHeights
		 * Input: a percentage along the track, given between 0.0 and 1.0
		 * Output: the angle of the tangent to the track at this percentage
		*/
	{
		//Lower and upper bounds for interpolation are both points on the graph
		glm::dvec2
			behind,
			ahead;

		//Make sure that the track wraps round
		if (percent < 0.0) percent += 1.0;
		if (percent > 1.0) percent -= 1.0;

		//Iterate over all points on the percentage-angle graph
		for (unsigned int i = 0; i < mPoints_graph.size(); i++) {

			//If there's a point on the graph with the exact target percentage...
			if (mPoints_graph[i].x == percent) {

				//... then no interpolation is required, and this can be returned directly.

				unsigned int latest = 0;

				//Just check that there aren't any other more recent points with the same coordinate
				for (unsigned int j = i; j < mPoints_graph.size(); j++) {
					if (mPoints_graph[j].x == percent)
						latest = j;
				}

				return mPoints_graph[latest].y;
			}
			//Just overshot the target point
			else if (mPoints_graph[i].x > percent) {

				//The point behind the current position
				behind = mPoints_graph[i - 1];

				//The point ahead of the current position
				ahead = mPoints_graph[i];

				//Handle the fact that 0 degrees is the same as 360 degrees
				if (behind.y == 360.0 && (360.0 - ahead.y >= 180.0))
					behind.y = 0.0;
				else if (ahead.y == 360.0 && (360.0 - behind.y >= 180.0) && (behind.y != 0.0))
					ahead.y = 0.0;

				//Interpolate between points to return the answer
				return behind.y + (ahead.y - behind.y) * ((percent - behind.x) / (ahead.x - behind.x));
			}
		}
	}

	void Track::imprintCircle(std::vector<double>& toImprintHeights, int centreX, int centreZ)
		/* Called by Track::runHeights
		 * Imprints a circle with a diameter of mWidth, and centre (centreX, centreZ) in the terrain height data
		*/
	{
		const int
			halfTerrainSize = floor(mTerrainSize_heightSamples * 0.5),
			circleRadius = floor(0.5 * mWidth);

		//X and Z need to be integer coordinates within the bounds of the heights array
		int
			targetX = 0,
			targetZ = 0,
			currentHeightIndex = 0;

		double
			distToCircleCentre = 0.0, //The distance from the current point to the centre of the circle
			circleWeighting = 0.0,    //A value between 0.0 and 1.0 describing the depth of the 'bowl' shape at a point
			newHeight = 0.0;		  //Used to store the new height being added to the terrain

		//Iterate over a square of points around (centreX, centreZ)
		for (int x = -circleRadius; x <= circleRadius; x++) {
			for (int z = -circleRadius; z <= circleRadius; z++) {

				//Calculate the distance of the current point to the centre of the circle
				distToCircleCentre = sqrt(pow(x, 2.0) + pow(z, 2.0));

				//If this distance is greater than the radius, then the point is outside the circle
				if (distToCircleCentre > circleRadius)  continue;

				//Otherwise, establish where the target point is on the terrain (given the position of the centre,
				//and the position of the point relative to the centre).
				targetX = centreX + x;
				targetZ = centreZ + z;

				//Use these values to get the index into the height array of this point
				currentHeightIndex = (targetX + halfTerrainSize) * mTerrainSize_heightSamples + (targetZ + halfTerrainSize);

				//Check that this point is actually within the terrain
				if (targetX >= -halfTerrainSize && targetX <= halfTerrainSize && targetZ >= -halfTerrainSize && targetZ <= halfTerrainSize) {

					//This line is used to give the track a 'bowl' like shape, by calculating a depth based on the distance to the centre of the circle
					circleWeighting = (1.0 - pow(distToCircleCentre / circleRadius, 3.0));
					newHeight = circleWeighting * -mMaxDepth;

					//Avoids a shape issue with overlapping circles
					if (newHeight < toImprintHeights[currentHeightIndex])
						toImprintHeights[currentHeightIndex] = newHeight;

					//Lastly, modify the surface types to represent the track
					mTempSurfaceTypes[2 * (currentHeightIndex - halfTerrainSize - targetX)] = TerrainType::TARMAC;
					mTempSurfaceTypes[2 * (currentHeightIndex - halfTerrainSize - targetX) + 1] = TerrainType::TARMAC;
				}
			}
		}
	}

	void Track::runPilotVersion()
		/* Called by Track::Track
		 * Iterates over the track shape once to determine various parameters required before the full iteration is completed,
		 * like the bounds of shape, its dimensions and a
		*/
	{
		//Used to store values during the iteration
		double
			percent = 0.0,
			currentAngle = 0.0;

		glm::dvec2
			direction = mStartDirection,
			displacementTracker = glm::dvec2(0.0);

		//Iterate round the shape with a given number of samples
		for (unsigned int i = 0; i < mNumSamplesOverTotal; i++) {

			//The the current percentage round the track, between 0.0 and 1.0
			percent = (double)i / (double)mNumSamplesOverTotal;
			currentAngle = lookUpAngleAtPercent_graph(percent);

			direction = normalize(glm::rotate(mStartDirection, glm::radians(currentAngle)));
			displacementTracker += direction;

			//The track shape's bounding rectangle grows as the track covers more area
			if (displacementTracker.x < mPilotResults.mLowerBoundDisplacement.x)
				mPilotResults.mLowerBoundDisplacement.x = displacementTracker.x;
			else if (displacementTracker.x > mPilotResults.mUpperBoundDisplacement.x)
				mPilotResults.mUpperBoundDisplacement.x = displacementTracker.x;

			if (displacementTracker.y < mPilotResults.mLowerBoundDisplacement.y)
				mPilotResults.mLowerBoundDisplacement.y = displacementTracker.y;
			else if (displacementTracker.y > mPilotResults.mUpperBoundDisplacement.y)
				mPilotResults.mUpperBoundDisplacement.y = displacementTracker.y;
		}

		mPilotResults.mShapeDimensions = mPilotResults.mUpperBoundDisplacement - mPilotResults.mLowerBoundDisplacement;
	}

	void Track::updateStartingPosition()
		/* Called by Track::Track
		 * Calculates the starting position of the track, for the full iteration, given the results obtained by the pilot run.
		 * This accounts for the track dimensions, width, and mTerrainBorderPadding (the space that should always be left
		 * clear of the track at the edges of the terrain).
		*/
	{
		//Occurs on both axes
		double
			terrainSize_units = mTerrainSize_heightSamples - 1,
			completePadding = mTerrainBorderPadding + 0.5 * mWidth;

		//Scale the results of the pilot run to match the size that the full track should cover
		glm::dvec2
			lowerBound = mPilotResults.mLowerBoundDisplacement * mPilotToMainScaleFactor,
			upperBound = mPilotResults.mUpperBoundDisplacement * mPilotToMainScaleFactor,
			dimensions = mPilotResults.mShapeDimensions * mPilotToMainScaleFactor;

		//Calculate a starting position for the track that allows its bounding rectangle to fit within that of the terrain (square)
		if (mPilotResults.shapeIsLandscape()) {
			mStartPosition.x = completePadding + abs(lowerBound.x);
			mStartPosition.y = completePadding + abs(lowerBound.y) + 0.5 * (terrainSize_units - (completePadding * 2.0) - dimensions.y);
		}
		else {
			mStartPosition.x = completePadding + abs(lowerBound.x) + 0.5 * (terrainSize_units - (completePadding * 2.0) - dimensions.x);
			mStartPosition.y = completePadding + abs(lowerBound.y);
		}

		mStartPosition -= glm::dvec2(0.5 * terrainSize_units);
	}

}