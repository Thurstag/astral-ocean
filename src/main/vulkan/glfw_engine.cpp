#include "glfw_engine.h"

/// <summary>
/// Destructor
/// </summary>
ao::vk::GLFWEngine::~GLFWEngine() {

}

void ao::vk::GLFWEngine::initWindow() {
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, this->settings.winSettings.rezisable ? GLFW_TRUE : GLFW_FALSE);

	// TODO: The monitor to use for full screen mode, or NULL for windowed mode (http://www.glfw.org/docs/latest/group__window.html#ga5c336fddf2cbb5b92f65f10fb6043344)
	this->window = glfwCreateWindow((int)this->settings.winSettings.width, (int)this->settings.winSettings.height, this->settings.winSettings.name.c_str(), nullptr, nullptr);
}

void ao::vk::GLFWEngine::freeWindow() {
	ao::vk::AOEngine::freeWindow();

	glfwDestroyWindow(this->window);
	glfwTerminate();
}

void ao::vk::GLFWEngine::loop() {
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
	}
}

std::vector<char const*> ao::vk::GLFWEngine::instanceExtensions() {
	uint32_t count;

	// Get extensions
	char const** extensions = glfwGetRequiredInstanceExtensions(&count);
	return std::vector<char const*>(extensions, extensions + count);
}
