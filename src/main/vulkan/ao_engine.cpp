#include "ao_engine.h"

ao::vk::AOEngine::AOEngine(EngineSettings settings) {
	this->settings = settings;
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
		throw ao::core::Exception("Unable to find GPUs");
	}

	// Select a VkPhysicalDevice & wrap it
	this->device = new AODevice(devices[this->selectVkPhysicalDevice(devices)]);

	LOGGER << LogLevel::DEBUG << "Select physical device: " << this->device->properties.deviceName;

	// Init logical device
	ao::vk::utilities::vkAssert(this->device->initLogicalDevice(this->deviceExtensions(), this->queueFlags(), this->commandPoolFlags()), "Fail to init logical device");

	// TODO: depth format
}

void ao::vk::AOEngine::freeVulkan() {
    /* TODO: Clean up Vulkan resources
	swapChain.cleanup();
	if (descriptorPool != VK_NULL_HANDLE) {
		vkDestroyDescriptorPool(device, descriptorPool, nullptr);
	}
	destroyCommandBuffers();
	vkDestroyRenderPass(device, renderPass, nullptr);
	for (uint32_t i = 0; i < frameBuffers.size(); i++) {
		vkDestroyFramebuffer(device, frameBuffers[i], nullptr);
	}

	for (auto& shaderModule : shaderModules) {
		vkDestroyShaderModule(device, shaderModule, nullptr);
	}
	vkDestroyImageView(device, depthStencil.view, nullptr);
	vkDestroyImage(device, depthStencil.image, nullptr);
	vkFreeMemory(device, depthStencil.mem, nullptr);

	vkDestroyPipelineCache(device, pipelineCache, nullptr);

	vkDestroyCommandPool(device, cmdPool, nullptr);

	vkDestroySemaphore(device, semaphores.presentComplete, nullptr);
	vkDestroySemaphore(device, semaphores.renderComplete, nullptr);
	for (auto& fence : waitFences) {
		vkDestroyFence(device, fence, nullptr);
	}

	if (settings.overlay) {
		UIOverlay.freeResources();
	} */

	delete this->device;
	vkDestroyInstance(this->instance, nullptr);
}

void ao::vk::AOEngine::freeWindow() {}

std::vector<char const*> ao::vk::AOEngine::deviceExtensions() {
	return std::vector<char const*>();
}

VkQueueFlags ao::vk::AOEngine::queueFlags() {
	return VkQueueFlagBits::VK_QUEUE_GRAPHICS_BIT | VkQueueFlagBits::VK_QUEUE_COMPUTE_BIT;
}

VkCommandPoolCreateFlags ao::vk::AOEngine::commandPoolFlags() {
	return VkCommandPoolCreateFlagBits::VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
}

uint8_t ao::vk::AOEngine::selectVkPhysicalDevice(std::vector<VkPhysicalDevice>& devices) {
	return 0;    // First device
}
