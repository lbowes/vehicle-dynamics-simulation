#ifndef SUSPENSIONSYSTEM_H
#define SUSPENSIONSYSTEM_H
#pragma once

#include <vector>
#include <Framework/Physics/Spring.hpp>

namespace Internal {
	class SuspensionSystem {
	private:
		std::vector<Framework::Physics::Spring> mSprings;

	public:
		SuspensionSystem();
		~SuspensionSystem() = default;

	};
}

#endif
