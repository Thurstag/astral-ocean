#pragma once

#include <vulkan/glfw_engine.h>
#include <core/exception.h>
#include <core/logger.h>
#include <iostream>

#include <stdlib.h>
#include <crtdbg.h>

#define _CRTDBG_MAP_ALLOC 1

struct Main{};

int main(int argc, char* argv[]) {
	std::string buffer;

	// Activate memory leak debugger
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

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
