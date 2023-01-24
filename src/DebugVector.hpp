/* CLASS OVERVIEW
 * - A visual representation of a vector quantity in the simulation (a line with a position, direction, length and colour)
*/

#ifndef DEBUGVECTOR_H
#define DEBUGVECTOR_H
#pragma once

#include <glm/glm/vec3.hpp>
#include <glm/glm/vec4.hpp>

namespace Visual {
	class DebugVector {
	private:
		static const unsigned char
			mNumPositionComponents = 6,
			mNumColourComponents = 8,
			mNumIndices = 2;
		
		glm::dvec3
			mPosition_world,
			mDirection_world;

		glm::dvec4 mColour;

		bool mIsVisible = true;
	
	public:
		DebugVector(glm::dvec3 position_world, glm::dvec3 direction_world, glm::dvec4 colour) :
			/* Called by DebugCarModel::addVectorLines
			*/
			mPosition_world(position_world),
			mDirection_world(direction_world),
			mColour(colour)
		{ }
		
		~DebugVector() = default;

		static inline unsigned char getNumPositionComponents() { return mNumPositionComponents; }
		static inline unsigned char getNumColourComponents() { return mNumColourComponents; }
		static inline unsigned char getNumIndices() { return mNumIndices; }
		inline glm::dvec3 getPosition_world() const { return mPosition_world; }
		inline glm::dvec3 getDirection_world() const { return mDirection_world; }
		inline glm::dvec4 getColour() const { return mColour; }
		inline bool isVisible() const { return mIsVisible; }

		inline void setPosition_world(glm::dvec3 newPosition_world) { mPosition_world = newPosition_world; }
		inline void setDirection_world(glm::dvec3 newDirection_world) { mDirection_world = newDirection_world; }
		inline void setColour(glm::dvec4 newColour) { mColour = newColour; }
		inline void setVisible(bool newVisibility) { mIsVisible = newVisibility; }

	};

}

#endif
