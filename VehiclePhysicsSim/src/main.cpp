#include "VehicleSimulation.h"

int main() {
	VehicleSimulation sim;

	if (!glfwGetCurrentContext()) {
		printf("This application requires OpenGL 3.3 or higher.\n");
		printf("OpenGL version supported by this platform (%s).\n", glGetString(GL_VERSION));
		std::cin.get();
		return 0;
	}
	else
		FreeConsole();

	sim.run();
}