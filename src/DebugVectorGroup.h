/* CLASS OVERVIEW
 * Responsible for creating and maintaining renderable buffers of data, that represent the DebugVectors in mVectors
*/

#ifndef DEBUGVECTORGROUP_H
#define DEBUGVECTORGROUP_H
#pragma once

#include <vector>

#include "DebugVector.hpp"

namespace Visual {
	class DebugVectorGroup {
	private:
		std::vector<DebugVector> mVectors;

		std::vector<float>
			mPositionBuffer,
			mColourBuffer;

		std::vector<unsigned int> mIndexBuffer;

	public:
		DebugVectorGroup() = default;
		~DebugVectorGroup() = default;

		void addVector(DebugVector newVector);
		void update();

		inline DebugVector* getVector(unsigned int index) { return (index >= 0 && index < mVectors.size()) ? &mVectors[index] : (DebugVector*)nullptr; }
		inline DebugVector* operator[](unsigned int index) { return getVector(index); }
		inline unsigned int getNumVectors() const { return mVectors.size(); }
		inline std::vector<float>& getPositionBuffer() { return mPositionBuffer; }
		inline std::vector<float>& getColourBuffer() { return mColourBuffer; }
		inline std::vector<unsigned int>& getIndexBuffer() { return mIndexBuffer; }

	private:
		unsigned int recalcNumVisbleVectors();

	};
}

#endif
