#pragma once

#include <iostream>
#ifdef WIN32
#include <stdlib.h>
#include <crtdbg.h>
#endif // WIN32

#include <ao/vulkan/engine/plugins/title_fps_plugin.h>
#include <ao/core/exception/exception.h>
#include <ao/core/logger/logger.h>

#include "RectangleDemo.h"
#include "TriangleDemo.h"

#define _CRTDBG_MAP_ALLOC 1
struct Main{};

ao::vulkan::AOEngine* choose(std::vector<std::string> choices, ao::vulkan::EngineSettings settings);

int main(int argc, char* argv[]) {
	std::string buffer;

#ifdef WIN32
	// Activate memory leaks detection
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif // WIN32

	// Get LOGGER
	ao::core::Logger LOGGER = ao::core::Logger::getInstance<Main>();

	// Define settings
	ao::vulkan::EngineSettings settings(
		ao::vulkan::WindowSettings("Win-Bootstrap", 1280, 720, true, true),
		ao::vulkan::CoreSettings(std::thread::hardware_concurrency(), true)
	);
	ao::vulkan::AOEngine* engine;

	try {
		engine = choose({ "Triangle", "Rectangle" }, settings);
		
		// Add plug-ins
		engine->add(new ao::vulkan::TitleFPSPlugin(engine));

		// Run engine
		engine->run();
	} catch (ao::core::Exception & e) {
		LOGGER << LogLevel::FATAL << e;
	} catch (std::exception& e) {
		LOGGER << LogLevel::FATAL << ao::core::Exception(e.what());
	} catch (...) {
		LOGGER << LogLevel::FATAL << "Unknown exception";
	}

	// Free engine
	delete engine;
	return 0;
}

ao::vulkan::AOEngine * choose(std::vector<std::string> choices, ao::vulkan::EngineSettings settings) {
	if (choices.empty() || choices.size() == 1) {
		return new TriangleDemo(settings);
	}

	// Display choices
	int i = 0;
	for (auto& choice : choices) {
		std::cout << i << "> " << choice << std::endl;

		i++;
	}

	// Ask choice
	std::cout << std::endl << "Choose a demo: ";
	std::cin >> i;
	std::cout << std::endl;

	// Return
	switch (i) {
		case 0:
			return new TriangleDemo(settings);

		case 1:
			return new RectangleDemo(settings);

		default:
			throw ao::core::Exception("Invalid choice");
	}
}
