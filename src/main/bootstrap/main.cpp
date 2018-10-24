#pragma once

#include <iostream>

#include "engine.h"

int main(int argc, char* argv[]) {
	// Run engine
	ao::vk::Engine* engine = new ao::vk::Engine();
	engine->run();

	// Free engine
	delete engine;
}
