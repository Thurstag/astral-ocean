#include "glfw_engine.h"

/// <summary>
/// Destructor
/// </summary>
ao::vk::GLFWEngine::~GLFWEngine() {
	this->freeWindow();
}

void ao::vk::GLFWEngine::initWindow() {
	glfwInit();

	// Define properties
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, this->settings.winSettings.rezisable ? GLFW_TRUE : GLFW_FALSE);

	// Create window
	this->window = glfwCreateWindow((int)this->settings.winSettings.width, (int)this->settings.winSettings.height, this->settings.winSettings.name.c_str(), nullptr, nullptr);

	LOGGER << LogLevel::DEBUG << "Init window(" << this->settings.winSettings.width << "x" << this->settings.winSettings.height << ")";
}

void ao::vk::GLFWEngine::initSurface(VkSurfaceKHR& surface) {
	ao::vk::utilities::vkAssert(glfwCreateWindowSurface(this->instance, this->window, nullptr, &surface), "Fail to create surface");
}

void ao::vk::GLFWEngine::freeWindow() {
	glfwDestroyWindow(this->window);
	glfwTerminate();
}

void ao::vk::GLFWEngine::loop() {
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
	}
}

std::vector<char const*> ao::vk::GLFWEngine::instanceExtensions() {
	return ao::glfw::utilities::getExtensions();
}
