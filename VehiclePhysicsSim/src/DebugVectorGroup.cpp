#include "DebugVectorGroup.h"

namespace Visual {

	void DebugVectorGroup::addVector(DebugVector newVector) 
		/* Called by DebugCarModel::addVectorLines
		 * Adds a new DebugVector object instance to mVectors
		 * Automatically updates the buffers to account for this change
		*/
	{
		mVectors.push_back(newVector);

		static unsigned int
			positionBufferSize = 0,
			colourBufferSize = 0,
			indexBufferSize = 0;

		positionBufferSize += DebugVector::getNumPositionComponents();
		mPositionBuffer.resize(positionBufferSize, 0.0f);

		colourBufferSize += DebugVector::getNumColourComponents();
		mColourBuffer.resize(colourBufferSize, 0.0f);

		indexBufferSize += DebugVector::getNumIndices();
		mIndexBuffer.resize(indexBufferSize, 0);

		update();
	}

	void DebugVectorGroup::update() 
		/* Called by 
		 * - DebugVectorGroup::addVector
		 * - DebugCarModel::updateVectorLines
		 * This function is responsible for rebuilding the position and colour buffers based on the data contained within the VectorLine objects in mVectors.
		*/
	{
		const unsigned int
			numVisibleVectors = recalcNumVisbleVectors(),
			posComponentsPerVec = DebugVector::getNumPositionComponents(),
			colourComponentsPerVec = DebugVector::getNumColourComponents(),
			indicesPerVec = DebugVector::getNumIndices();

		DebugVector* currentVector = nullptr;
		
		glm::dvec3
			vecStartPos,
			vecEndPos;

		glm::dvec4 vecColour;

		//Remove all data currently in the 3 buffers before repopulating them
		mPositionBuffer.resize(0);
		mColourBuffer.resize(0);
		mIndexBuffer.resize(0);

		//Run through each vector
		for (unsigned int i = 0; i < numVisibleVectors; i++) {
			currentVector = &mVectors[i];
			
			//Retrieve the properties of the current vector
			vecStartPos = currentVector->isVisible() ? currentVector->getPosition_world() : glm::dvec3(0.0);
			vecEndPos = currentVector->isVisible() ? vecStartPos + currentVector->getDirection_world() : glm::dvec3(0.0);
			vecColour = currentVector->getColour();
			
			//Position data
			{
				//Start point
				mPositionBuffer.push_back(vecStartPos.x);
				mPositionBuffer.push_back(vecStartPos.y);
				mPositionBuffer.push_back(vecStartPos.z);

				//End point
				mPositionBuffer.push_back(vecEndPos.x);
				mPositionBuffer.push_back(vecEndPos.y);
				mPositionBuffer.push_back(vecEndPos.z);
			}

			//Colour data
			{
				//Start point
				mColourBuffer.push_back(vecColour.r);
				mColourBuffer.push_back(vecColour.g);
				mColourBuffer.push_back(vecColour.b);
				mColourBuffer.push_back(vecColour.a);

				//End point
				mColourBuffer.push_back(vecColour.r);
				mColourBuffer.push_back(vecColour.g);
				mColourBuffer.push_back(vecColour.b);
				mColourBuffer.push_back(vecColour.a);
			}

			//Index data
			{
				mIndexBuffer.push_back(i * indicesPerVec);
				mIndexBuffer.push_back(i * indicesPerVec + 1);
			}
		}
	}

	unsigned int DebugVectorGroup::recalcNumVisbleVectors() 
		/* Called by DebugVectorGroup::update
		*/
	{
		unsigned int count = 0;
		
		for (DebugVector& v : mVectors) {
			if (v.isVisible())
				count++;
		}
		
		return count;
	}

}