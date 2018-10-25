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

	// TODO: Create logical device
}
