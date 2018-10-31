#include "glfw_engine.h"

/// <summary>
/// Destructor
/// </summary>
ao::vulkan::GLFWEngine::~GLFWEngine() {
	this->freeWindow();
}

void ao::vulkan::GLFWEngine::initWindow() {
	glfwInit();

	// Define properties
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, this->_settings.window.rezisable ? GLFW_TRUE : GLFW_FALSE);

	// Create window
	this->window = glfwCreateWindow((int)this->_settings.window.width, (int)this->_settings.window.height, this->_settings.window.name.c_str(), nullptr, nullptr);

	LOGGER << LogLevel::INFO << "Init " << this->_settings.window.width << "x" << this->_settings.window.height << " window";
}

void ao::vulkan::GLFWEngine::initSurface(vk::SurfaceKHR& surface) {
	VkSurfaceKHR _s;
	ao::vulkan::utilities::vkAssert(glfwCreateWindowSurface(this->instance, this->window, nullptr, &_s), "Fail to create surface");

	surface = _s;
}

void ao::vulkan::GLFWEngine::freeWindow() {
	glfwDestroyWindow(this->window);
	glfwTerminate();
}

bool ao::vulkan::GLFWEngine::isIconified() {
	return glfwGetWindowAttrib(this->window, GLFW_ICONIFIED);
}

void ao::vulkan::GLFWEngine::setWindowTitle(std::string title) {
	ao::vulkan::AOEngine::setWindowTitle(title);
	glfwSetWindowTitle(this->window, title.c_str());
}

bool ao::vulkan::GLFWEngine::loopingCondition() {
	return !glfwWindowShouldClose(this->window);
}

void ao::vulkan::GLFWEngine::onLoopIteration() {
	ao::vulkan::AOEngine::onLoopIteration();

	glfwPollEvents();
}

void ao::vulkan::GLFWEngine::waitMaximized() {
	glfwWaitEvents();
}

std::vector<char const*> ao::vulkan::GLFWEngine::instanceExtensions() {
	return ao::glfw::utilities::getExtensions();
}
