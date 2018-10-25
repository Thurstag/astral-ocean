#include "ao_device.h"

ao::vk::AODevice::AODevice(VkPhysicalDevice& device) {
	this->device = device;

	// Get properties
	vkGetPhysicalDeviceProperties(this->device, &this->properties);
	vkGetPhysicalDeviceFeatures(this->device, &this->features);
	vkGetPhysicalDeviceMemoryProperties(this->device, &this->memoryProperties);

	// Get QueueFamilyProperties
	this->queueFamilyProperties = ao::vk::utilities::vkQueueFamilyProperties(this->device);

	// Check count
	if (this->queueFamilyProperties.empty()) {
		throw ao::core::Exception("Empty queueFamilyProperties");
	}

	// Get supported extensions
	this->extensions = ao::vk::utilities::vkExtensionProperties(this->device);

	// Check count
	if (this->extensions.empty()) {
		LOGGER << LogLevel::WARN << "Extensions vector is empty";
	}
}

ao::vk::AODevice::~AODevice() {
	if (this->commandPool) {
		vkDestroyCommandPool(this->logicalDevice, this->commandPool, nullptr);
	}
	if (this->logicalDevice) {
		vkDestroyDevice(this->logicalDevice, nullptr);
	}
}

VkResult ao::vk::AODevice::initLogicalDevice(std::vector<char const*> deviceExtensions, VkQueueFlags qflags, VkCommandPoolCreateFlags cflags, bool swapChain) {
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	float const DEFAULT_QUEUE_PRIORITY = 0.0f;

	/* GRAPHICS QUEUE */
	if (qflags & VkQueueFlagBits::VK_QUEUE_GRAPHICS_BIT) {
		std::get<AO_GRAPHICS_QUEUE_INDEX>(this->queueFamilyIndices) = ao::vk::utilities::findQueueFamilyIndex(this->queueFamilyProperties, VkQueueFlagBits::VK_QUEUE_GRAPHICS_BIT);

		// Check result
		if (std::get<AO_GRAPHICS_QUEUE_INDEX>(this->queueFamilyIndices) < 0) {
			throw ao::core::Exception("Fail to find a queueFamily that supports VK_QUEUE_GRAPHICS_BIT");
		}

		VkDeviceQueueCreateInfo queueInfo{};
		queueInfo.queueFamilyIndex = std::get<AO_GRAPHICS_QUEUE_INDEX>(this->queueFamilyIndices);
		queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueInfo.pQueuePriorities = &DEFAULT_QUEUE_PRIORITY;
		queueInfo.queueCount = 1;

		// Add info
		queueCreateInfos.push_back(queueInfo);
	}
	else {
		std::get<AO_GRAPHICS_QUEUE_INDEX>(this->queueFamilyIndices) = VK_NULL_HANDLE;
	}

	/* COMPUTE QUEUE */
	if (qflags & VkQueueFlagBits::VK_QUEUE_COMPUTE_BIT) {
		std::get<AO_COMPUTE_QUEUE_INDEX>(this->queueFamilyIndices) = ao::vk::utilities::findQueueFamilyIndex(this->queueFamilyProperties, VkQueueFlagBits::VK_QUEUE_COMPUTE_BIT);

		// Check result
		if (std::get<AO_COMPUTE_QUEUE_INDEX>(this->queueFamilyIndices) < 0) {
			throw ao::core::Exception("Fail to find a queueFamily that supports AO_COMPUTE_QUEUE_INDEX");
		}

		// Create info if it's a new queue
		if (std::get<AO_COMPUTE_QUEUE_INDEX>(this->queueFamilyIndices) != std::get<AO_GRAPHICS_QUEUE_INDEX>(this->queueFamilyIndices)) {
			VkDeviceQueueCreateInfo queueInfo{};
			queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueInfo.queueFamilyIndex = std::get<AO_COMPUTE_QUEUE_INDEX>(this->queueFamilyIndices);
			queueInfo.pQueuePriorities = &DEFAULT_QUEUE_PRIORITY;
			queueInfo.queueCount = 1;

			// Add info
			queueCreateInfos.push_back(queueInfo);
		}
	}
	else { // Use graphics queue
		std::get<AO_COMPUTE_QUEUE_INDEX>(this->queueFamilyIndices) = std::get<AO_GRAPHICS_QUEUE_INDEX>(this->queueFamilyIndices);
	}

	/* TRANSFER QUEUE */
	if (qflags & VkQueueFlagBits::VK_QUEUE_TRANSFER_BIT) {
		std::get<AO_TRANSFER_QUEUE_INDEX>(this->queueFamilyIndices) = ao::vk::utilities::findQueueFamilyIndex(this->queueFamilyProperties, VkQueueFlagBits::VK_QUEUE_TRANSFER_BIT);

		// Check result
		if (std::get<AO_TRANSFER_QUEUE_INDEX>(this->queueFamilyIndices) < 0) {
			throw ao::core::Exception("Fail to find a queueFamily that supports AO_TRANSFER_QUEUE_INDEX");
		}

		// Create info if it's a new queue
		if (std::get<AO_TRANSFER_QUEUE_INDEX>(this->queueFamilyIndices) != std::get<AO_GRAPHICS_QUEUE_INDEX>(this->queueFamilyIndices) && std::get<AO_TRANSFER_QUEUE_INDEX>(this->queueFamilyIndices) != std::get<AO_COMPUTE_QUEUE_INDEX>(this->queueFamilyIndices)) {
			VkDeviceQueueCreateInfo queueInfo{};
			queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueInfo.queueFamilyIndex = std::get<AO_TRANSFER_QUEUE_INDEX>(this->queueFamilyIndices);
			queueInfo.pQueuePriorities = &DEFAULT_QUEUE_PRIORITY;
			queueInfo.queueCount = 1;

			// Add info
			queueCreateInfos.push_back(queueInfo);
		}
	}
	else { // Use graphics queue
		std::get<AO_TRANSFER_QUEUE_INDEX>(this->queueFamilyIndices) = std::get<AO_GRAPHICS_QUEUE_INDEX>(this->queueFamilyIndices);
	}

	// Request swap chain extension
	if (swapChain) {
		deviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
	}

	VkDeviceCreateInfo deviceCreateInfo = {};
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
	deviceCreateInfo.pEnabledFeatures = &this->features;

	// Enable the debug marker extension if it is present
	if (std::find_if(this->extensions.begin(), this->extensions.end(), [](VkExtensionProperties properties) { return properties.extensionName == VK_EXT_DEBUG_MARKER_EXTENSION_NAME; }) != this->extensions.end()) {
		deviceExtensions.push_back(VK_EXT_DEBUG_MARKER_EXTENSION_NAME);
		// TODO
	}

	if (!deviceExtensions.empty()) {
		deviceCreateInfo.enabledExtensionCount = (uint32_t)deviceExtensions.size();
		deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();
	}

	// Create device
	ao::vk::utilities::vkAssert(vkCreateDevice(this->device, &deviceCreateInfo, nullptr, &this->logicalDevice), "Fail to create logical device");

	// Create command pool
	return this->initCommandPool(cflags);
}

VkResult ao::vk::AODevice::initCommandPool(VkCommandPoolCreateFlags flags) {
	VkCommandPoolCreateInfo cmdPoolInfo = {};
	cmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	cmdPoolInfo.queueFamilyIndex = std::get<AO_GRAPHICS_QUEUE_INDEX>(this->queueFamilyIndices);
	cmdPoolInfo.flags = flags;

	return vkCreateCommandPool(logicalDevice, &cmdPoolInfo, nullptr, &this->commandPool);
}
