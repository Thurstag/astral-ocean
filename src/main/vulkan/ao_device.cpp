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
		BOOST_THROW_EXCEPTION(std::exception("Empty queueFamilyProperties"));
	}

	// Get supported extensions
	this->extensions = ao::vk::utilities::vkExtensionProperties(this->device);

	// Check count
	if (this->extensions.empty()) {
		// TODO: WARNING
	}

	// TODO: Create logical device
}
