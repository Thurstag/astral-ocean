#pragma once

#include <boost/exception/diagnostic_information.hpp>
#include <vulkan/glfw_engine.h>
#include <iostream>

int main(int argc, char* argv[]) {

	// Define settings
	ao::vk::EngineSettings settings = { true, "TEST", 1280, 720 };
	ao::vk::AOEngine* engine;

	try {
		// Run engine
		engine = new ao::vk::GLFWEngine(settings);
		engine->run();
	} catch (boost::exception & e) {
		std::cerr << boost::diagnostic_information(e);
	} catch (...) {
		std::cerr << "Unknown exception" << std::endl;
	}

    // Free engine
	delete engine;
}
