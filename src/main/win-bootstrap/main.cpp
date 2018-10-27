#pragma once

#include <vulkan/glfw_engine.h>
#include <core/exception.h>
#include <core/logger.h>
#include <iostream>

#ifdef WIN32
#include <stdlib.h>
#include <crtdbg.h>

#define _CRTDBG_MAP_ALLOC 1
#endif // WIN32

struct Main{};

int main(int argc, char* argv[]) {
	std::string buffer;

#ifdef WIN32
	// Activate memory leaks detection
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif // WIN32

	// Get LOGGER
	ao::core::Logger LOGGER = ao::core::Logger::getInstance<Main>();

	// Define settings
	ao::vk::EngineSettings settings = { true, "TEST", 1280, 720 };
	ao::vk::AOEngine* engine;

	try {
		// Run engine
		engine = new ao::vk::GLFWEngine(settings);
		engine->run();
	} catch (ao::core::Exception & e) {
		LOGGER << LogLevel::FATAL << e;
		LOGGER << LogLevel::FATAL << e;
	} catch (std::exception& e) {
		LOGGER << LogLevel::FATAL << ao::core::Exception(e);
	} catch (...) {
		LOGGER << LogLevel::FATAL << "Unknown exception";
	}

	// Free engine
	delete engine;
	return 0;
}
