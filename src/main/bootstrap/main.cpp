#pragma once

#include <vulkan/glfw_engine.h>
#include <core/exception.h>
#include <core/logger.h>
#include <iostream>
#include <conio.h>

struct Main{};

void onError();

int main(int argc, char* argv[]) {
	std::string buffer;

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
		onError();
	} catch (std::exception& e) {
		LOGGER << LogLevel::FATAL << ao::core::Exception(e);
		onError();
	} catch (...) {
		LOGGER << LogLevel::FATAL << "Unknown exception";
		onError();
	}

	// Free engine
	delete engine;
}

void onError() {
	// Display a message
	std::cout << "Press enter to exit...";

	// Wait input
	getch();
}
