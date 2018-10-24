#include "engine.h"

ao::vk::Engine::Engine(EngineSettings settings) {
	this->settings = settings;

	this->instance = {};
}

ao::vk::Engine::~Engine() {
	this->freeVulkan();
	this->freeWindow();
}

void ao::vk::Engine::run() {
	this->initWindow();
	this->initVulkan();

	this->loop();
}

void ao::vk::Engine::initVulkan() {
	// Create instance
	ao::vk::utilities::vkAssert(ao::vk::utilities::createVkInstance(this->settings, this->instance, this->instanceExtensions()), "Fail to create instance");

	// TODO: Set-up debugging if validation layer
}

void ao::vk::Engine::freeVulkan() {
	vkDestroyInstance(this->instance, nullptr);
}

void ao::vk::Engine::freeWindow() {}
