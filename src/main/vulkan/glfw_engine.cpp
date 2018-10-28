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
	glfwWindowHint(GLFW_RESIZABLE, this->_settings.window.rezisable ? GLFW_TRUE : GLFW_FALSE);

	// Create window
	this->window = glfwCreateWindow((int)this->_settings.window.width, (int)this->_settings.window.height, this->_settings.window.name.c_str(), nullptr, nullptr);

	LOGGER << LogLevel::DEBUG << "Init " << this->_settings.window.width << "x" << this->_settings.window.height << " window";
}

void ao::vk::GLFWEngine::initSurface(VkSurfaceKHR& surface) {
	ao::vk::utilities::vkAssert(glfwCreateWindowSurface(this->instance, this->window, nullptr, &surface), "Fail to create surface");
}

void ao::vk::GLFWEngine::freeWindow() {
	glfwDestroyWindow(this->window);
	glfwTerminate();
}

bool ao::vk::GLFWEngine::isIconified() {
	return glfwGetWindowAttrib(this->window, GLFW_ICONIFIED);
}

void ao::vk::GLFWEngine::setWindowTitle(std::string title) {
	ao::vk::AOEngine::setWindowTitle(title);
	glfwSetWindowTitle(this->window, title.c_str());
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
