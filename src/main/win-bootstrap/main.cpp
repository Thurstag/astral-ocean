// Copyright 2018 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#include <iostream>

#ifdef DEBUG
#include <vld.h>
#endif

#define GLM_FORCE_RADIANS 1

#include <ao/vulkan/engine/plugins/title_fps_plugin.h>
#include <ao/core/exception/exception.h>
#include <ao/core/logger/logger.h>

#include "demo/RectangleDemo.h"
#include "demo/TriangleDemo.h"
#include "demo/TextureDemo.h"

struct Main{};

ao::vulkan::AOEngine* choose(std::vector<std::string> choices, ao::vulkan::EngineSettings settings);

int main(int argc, char* argv[]) {
	ao::core::Logger::Init();

	// Get LOGGER
	ao::core::Logger LOGGER = ao::core::Logger::getInstance<Main>();

	// Define settings
	ao::vulkan::EngineSettings settings(
		ao::vulkan::WindowSettings("Win-Bootstrap", 1280, 720, true, true),
		ao::vulkan::CoreSettings(std::thread::hardware_concurrency(), true)
	);
	ao::vulkan::AOEngine* engine;

	try {
		engine = choose({ "Triangle", "Rectangle", "Texture" }, settings);
		
		// Add plug-ins
		engine->add(new ao::vulkan::TitleFPSPlugin(engine));

		// Run engine
		engine->run();
	} catch (ao::core::Exception & e) {
		LOGGER << ao::core::LogLevel::fatal << e;
	} catch (std::exception& e) {
		LOGGER << ao::core::LogLevel::fatal << ao::core::Exception(e.what(), false);
	} catch (...) {
		LOGGER << ao::core::LogLevel::fatal << "Unknown exception";
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

		case 2:
			return new TextureDemo(settings);

		default:
			throw ao::core::Exception("Invalid choice");
	}
}
