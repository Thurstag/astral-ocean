#include "device.h"

ao::vulkan::Device::Device(vk::PhysicalDevice& device) {
	this->physical = device;

	// Get QueueFamilyProperties
	this->queueFamilyProperties = this->physical.getQueueFamilyProperties();

	// Check count
	if (this->queueFamilyProperties.empty()) {
		throw ao::core::Exception("Empty queueFamilyProperties");
	}

	// Get supported extensions
	this->extensions = ao::vulkan::utilities::vkExtensionProperties(this->physical);

	// Check count
	if (this->extensions.empty()) {
		LOGGER << LogLevel::WARN << "Extensions vector is empty";
	}
}

ao::vulkan::Device::~Device() {
	this->logical.destroy();
}

void ao::vulkan::Device::initLogicalDevice(std::vector<char const*>& deviceExtensions, std::vector<vk::PhysicalDeviceFeatures>& deviceFeatures, vk::QueueFlags qflags, vk::CommandPoolCreateFlags cflags, bool swapChain) {
	std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
	float const DEFAULT_QUEUE_PRIORITY = 0.0f;

	/* GRAPHICS QUEUE */
	if (qflags & vk::QueueFlagBits::eGraphics) {
		std::get<AO_GRAPHICS_QUEUE_INDEX>(this->queueFamilyIndices) = ao::vulkan::utilities::findQueueFamilyIndex(this->queueFamilyProperties, vk::QueueFlagBits::eGraphics);

		// Check result
		if (std::get<AO_GRAPHICS_QUEUE_INDEX>(this->queueFamilyIndices) < 0) {
			throw ao::core::Exception("Fail to find a queueFamily that supports vk::QueueFlagBits::eGraphics");
		}

		// Add info
		queueCreateInfos.push_back(vk::DeviceQueueCreateInfo(vk::DeviceQueueCreateFlags(), std::get<AO_GRAPHICS_QUEUE_INDEX>(this->queueFamilyIndices), 1, &DEFAULT_QUEUE_PRIORITY));
	}
	else {
		std::get<AO_GRAPHICS_QUEUE_INDEX>(this->queueFamilyIndices) = VK_NULL_HANDLE;
	}

	/* COMPUTE QUEUE */
	if (qflags & vk::QueueFlagBits::eCompute) {
		std::get<AO_COMPUTE_QUEUE_INDEX>(this->queueFamilyIndices) = ao::vulkan::utilities::findQueueFamilyIndex(this->queueFamilyProperties, vk::QueueFlagBits::eCompute);

		// Check result
		if (std::get<AO_COMPUTE_QUEUE_INDEX>(this->queueFamilyIndices) < 0) {
			throw ao::core::Exception("Fail to find a queueFamily that supports vk::QueueFlagBits::eCompute");
		}

		// Add info if it's a new queue
		if (std::get<AO_COMPUTE_QUEUE_INDEX>(this->queueFamilyIndices) != std::get<AO_GRAPHICS_QUEUE_INDEX>(this->queueFamilyIndices)) {
			queueCreateInfos.push_back(vk::DeviceQueueCreateInfo(vk::DeviceQueueCreateFlags(), std::get<AO_COMPUTE_QUEUE_INDEX>(this->queueFamilyIndices), 1, &DEFAULT_QUEUE_PRIORITY));
		}
	}
	else { // Use graphics queue
		std::get<AO_COMPUTE_QUEUE_INDEX>(this->queueFamilyIndices) = std::get<AO_GRAPHICS_QUEUE_INDEX>(this->queueFamilyIndices);
	}

	/* TRANSFER QUEUE */
	if (qflags & vk::QueueFlagBits::eTransfer) {
		std::get<AO_TRANSFER_QUEUE_INDEX>(this->queueFamilyIndices) = ao::vulkan::utilities::findQueueFamilyIndex(this->queueFamilyProperties, vk::QueueFlagBits::eTransfer);

		// Check result
		if (std::get<AO_TRANSFER_QUEUE_INDEX>(this->queueFamilyIndices) < 0) {
			throw ao::core::Exception("Fail to find a queueFamily that supports vk::QueueFlagBits::eTransfer");
		}

		// Add info if it's a new queue
		if (std::get<AO_TRANSFER_QUEUE_INDEX>(this->queueFamilyIndices) != std::get<AO_GRAPHICS_QUEUE_INDEX>(this->queueFamilyIndices) && std::get<AO_TRANSFER_QUEUE_INDEX>(this->queueFamilyIndices) != std::get<AO_COMPUTE_QUEUE_INDEX>(this->queueFamilyIndices)) {
			queueCreateInfos.push_back(vk::DeviceQueueCreateInfo(vk::DeviceQueueCreateFlags(), std::get<AO_TRANSFER_QUEUE_INDEX>(this->queueFamilyIndices), 1, &DEFAULT_QUEUE_PRIORITY));
		}
	}
	else { // Use graphics queue
		std::get<AO_TRANSFER_QUEUE_INDEX>(this->queueFamilyIndices) = std::get<AO_GRAPHICS_QUEUE_INDEX>(this->queueFamilyIndices);
	}

	// Request swap chain extension
	if (swapChain) {
		deviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
	}

	vk::DeviceCreateInfo deviceCreateInfo(vk::DeviceCreateFlags(), static_cast<uint32_t>(queueCreateInfos.size()), queueCreateInfos.data());
	deviceCreateInfo.setPEnabledFeatures(deviceFeatures.data());

	// Add extensions
	if (!deviceExtensions.empty()) {
		deviceCreateInfo.setEnabledExtensionCount(static_cast<uint32_t>(deviceExtensions.size()));
		deviceCreateInfo.setPpEnabledExtensionNames(deviceExtensions.data());
	}

	// Create device
	this->logical = this->physical.createDevice(deviceCreateInfo);
}

uint32_t ao::vulkan::Device::memoryType(uint32_t typeBits, vk::MemoryPropertyFlags properties) {
	vk::PhysicalDeviceMemoryProperties memoryProperties = this->physical.getMemoryProperties();

	for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++) {
		if ((typeBits & 1) == 1) {
			if ((memoryProperties.memoryTypes[i].propertyFlags & properties) == properties) {
				return i;
			}
		}
		typeBits >>= 1;
	}
	throw ao::core::Exception("Fail to find a matching memory type");
}
