#pragma once

#include <iostream>
#ifdef WIN32
#include <stdlib.h>
#include <crtdbg.h>
#endif // WIN32

#include <ao/vulkan/plugins/title_fps_plugin.h>
#include <ao/vulkan/glfw_engine.h>
#include <ao/core/exception.h>
#include <ao/core/logger.h>

#define _CRTDBG_MAP_ALLOC 1
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
	ao::vk::EngineSettings settings = { true, "TEST", 1280, 720, true, false };
	ao::vk::AOEngine* engine;

	try {
		engine = new ao::vk::GLFWEngine(settings);
		
		// Add plug-ins
		engine->add(new ao::vk::TitleFPSPlugin(engine));

		// Run engine
		engine->run();
	} catch (ao::core::Exception & e) {
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
