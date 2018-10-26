#include "ao_engine.h"

ao::vk::AOEngine::AOEngine(EngineSettings settings) {
	this->settings = settings;
}

ao::vk::AOEngine::~AOEngine() {
	this->freeVulkan();
}

void ao::vk::AOEngine::run() {
	this->initWindow();
	this->initVulkan();

	this->prepareVulkan();

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

	// Find suitable depth format
	ao::vk::utilities::vkAssert(ao::vk::utilities::getSupportedDepthFormat(this->device->device, this->device->depthFormat), "Fail to find suitable depth format");

	// Create swapChain
	this->swapchain = new AOSwapChain(&this->instance, this->device);

	// Create semaphores
	VkSemaphoreCreateInfo semaphoreInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
	ao::vk::utilities::vkAssert(vkCreateSemaphore(this->device->logicalDevice, &semaphoreInfo, nullptr, &this->semaphores.first), "Fail to create present semaphore");
	ao::vk::utilities::vkAssert(vkCreateSemaphore(this->device->logicalDevice, &semaphoreInfo, nullptr, &this->semaphores.second), "Fail to create render semaphore");
}

void ao::vk::AOEngine::freeVulkan() {
	delete this->swapchain;
    /* TODO: Clean up Vulkan resources
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

	vkDestroyPipelineCache(device, pipelineCache, nullptr);	*/

	vkDestroySemaphore(this->device->logicalDevice, this->semaphores.first, nullptr);
	vkDestroySemaphore(this->device->logicalDevice, this->semaphores.second, nullptr);
	
	/* TODO Clean up Vulkan resources
	for (auto& fence : waitFences) {
		vkDestroyFence(device, fence, nullptr);
	}

	if (settings.overlay) {
		UIOverlay.freeResources();
	} */

	delete this->device;
	vkDestroyInstance(this->instance, nullptr);
}

void ao::vk::AOEngine::prepareVulkan() {
	/*if (this->device.debugMarkers) {
	   // TODO
	}*/

	// Init surface
	this->initSurface(this->swapchain->surface);
	this->swapchain->initSurface();

	// Init command pool
	this->swapchain->initCommandPool();

	// Init swap chain
	this->swapchain->init(this->settings.winSettings.width, this->settings.winSettings.height);

	// TODO: Create command buffers
}

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
