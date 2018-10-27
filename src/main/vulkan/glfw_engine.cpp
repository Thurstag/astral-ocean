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
	glfwWindowHint(GLFW_RESIZABLE, this->settings.window.rezisable ? GLFW_TRUE : GLFW_FALSE);

	// Create window
	this->window = glfwCreateWindow((int)this->settings.window.width, (int)this->settings.window.height, this->settings.window.name.c_str(), nullptr, nullptr);

	LOGGER << LogLevel::DEBUG << "Init " << this->settings.window.width << "x" << this->settings.window.height << " window";
}

void ao::vk::GLFWEngine::initSurface(VkSurfaceKHR& surface) {
	ao::vk::utilities::vkAssert(glfwCreateWindowSurface(this->instance, this->window, nullptr, &surface), "Fail to create surface");
}

void ao::vk::GLFWEngine::freeWindow() {
	glfwDestroyWindow(this->window);
	glfwTerminate();
}

bool ao::vk::GLFWEngine::isIconified() {
	return glfwGetWindowAttrib(window, GLFW_ICONIFIED);
}

bool ao::vk::GLFWEngine::loopingCondition() {
	return !glfwWindowShouldClose(this->window);
}

void ao::vk::GLFWEngine::onLoopIteration() {
	ao::vk::AOEngine::onLoopIteration();

	glfwPollEvents();
}

std::vector<char const*> ao::vk::GLFWEngine::instanceExtensions() {
	return ao::glfw::utilities::getExtensions();
}
