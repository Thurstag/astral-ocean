#include "ao_engine.h"

ao::vk::AOEngine::AOEngine(EngineSettings settings) {
	this->settings = settings;

	this->instance = {};
}

ao::vk::AOEngine::~AOEngine() {
	this->freeVulkan();
	this->freeWindow();
}

void ao::vk::AOEngine::run() {
	this->initWindow();
	this->initVulkan();

	this->loop();
}

void ao::vk::AOEngine::initVulkan() {
	// Create instance
	ao::vk::utilities::vkAssert(ao::vk::utilities::createVkInstance(this->settings, this->instance, this->instanceExtensions()), "Fail to create instance");

	// TODO: Set-up debugging if validation layer is enabled
	if (this->settings.vkValidationLayers) {
		ao::vk::utilities::initDebugging(this->instance, VkDebugReportFlagBitsEXT::VK_DEBUG_REPORT_ERROR_BIT_EXT | VkDebugReportFlagBitsEXT::VK_DEBUG_REPORT_WARNING_BIT_EXT, VK_NULL_HANDLE);
	}

	// Get GPUs
	std::vector<VkPhysicalDevice> devices = ao::vk::utilities::vkPhysicalDevices(this->instance);

	// Check count
	if (devices.empty()) {
		BOOST_THROW_EXCEPTION(std::exception("Unable to find GPUs"));
	}

	// Select a VkPhysicalDevice & wrap it
	AODevice device(devices[this->selectVkPhysicalDevice(devices)]);
}

void ao::vk::AOEngine::freeVulkan() {
	vkDestroyInstance(this->instance, nullptr);
}

void ao::vk::AOEngine::freeWindow() {}

uint64_t ao::vk::AOEngine::selectVkPhysicalDevice(std::vector<VkPhysicalDevice>& devices) {
	return 0;    // First device
}
