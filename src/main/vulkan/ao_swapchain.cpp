#include "ao_swapchain.h"

ao::vk::AOSwapChain::AOSwapChain(VkInstance* instance, AODevice* device) {
	this->instance = instance;
	this->device = device;

	this->fpGetPhysicalDeviceSurfaceSupportKHR = ao::vk::utilities::instanceProcAddr<PFN_vkGetPhysicalDeviceSurfaceSupportKHR>(*this->instance, "vkGetPhysicalDeviceSurfaceSupportKHR");
	this->fpGetPhysicalDeviceSurfaceCapabilitiesKHR = ao::vk::utilities::instanceProcAddr<PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR>(*this->instance, "vkGetPhysicalDeviceSurfaceCapabilitiesKHR");
	this->fpGetPhysicalDeviceSurfaceFormatsKHR = ao::vk::utilities::instanceProcAddr<PFN_vkGetPhysicalDeviceSurfaceFormatsKHR>(*this->instance, "vkGetPhysicalDeviceSurfaceFormatsKHR");
	this->fpGetPhysicalDeviceSurfacePresentModesKHR = ao::vk::utilities::instanceProcAddr<PFN_vkGetPhysicalDeviceSurfacePresentModesKHR>(*this->instance, "vkGetPhysicalDeviceSurfacePresentModesKHR");

	this->fpCreateSwapchainKHR = ao::vk::utilities::deviceProcAddr<PFN_vkCreateSwapchainKHR>(this->device->logicalDevice, "vkCreateSwapchainKHR");
	this->fpDestroySwapchainKHR = ao::vk::utilities::deviceProcAddr<PFN_vkDestroySwapchainKHR>(this->device->logicalDevice, "vkDestroySwapchainKHR");
	this->fpGetSwapchainImagesKHR = ao::vk::utilities::deviceProcAddr<PFN_vkGetSwapchainImagesKHR>(this->device->logicalDevice, "vkGetSwapchainImagesKHR");
	this->fpAcquireNextImageKHR = ao::vk::utilities::deviceProcAddr<PFN_vkAcquireNextImageKHR>(this->device->logicalDevice, "vkAcquireNextImageKHR");
	this->fpQueuePresentKHR = ao::vk::utilities::deviceProcAddr<PFN_vkQueuePresentKHR>(this->device->logicalDevice, "vkQueuePresentKHR");

	// Check function pointers
	this->checkPFNs();
}

ao::vk::AOSwapChain::~AOSwapChain() {
	/* TODO
	if (this->swapChain) {
		for (uint32_t i = 0; i < imageCount; i++) {
			vkDestroyImageView(this->device, buffers[i].view, nullptr);
		}
	}
	if (this->surface != VK_NULL_HANDLE) {
		fpDestroySwapchainKHR(this->device, swapChain, nullptr);
		vkDestroySurfaceKHR(instance, surface, nullptr);
	} */
}

void ao::vk::AOSwapChain::checkPFNs() {
	// Instance functions
	if (this->fpGetPhysicalDeviceSurfaceCapabilitiesKHR == nullptr) {
		throw ao::core::Exception("Fail to get vkGetPhysicalDeviceSurfaceSupportKHR()");
	}
	if (this->fpGetPhysicalDeviceSurfaceSupportKHR == nullptr) {
		throw ao::core::Exception("Fail to get vkGetPhysicalDeviceSurfaceCapabilitiesKHR()");
	}
	if (this->fpGetPhysicalDeviceSurfaceFormatsKHR == nullptr) {
		throw ao::core::Exception("Fail to get vkGetPhysicalDeviceSurfaceFormatsKHR()");
	}
	if (this->fpGetPhysicalDeviceSurfacePresentModesKHR == nullptr) {
		throw ao::core::Exception("Fail to get vkGetPhysicalDeviceSurfacePresentModesKHR()");
	}

	// Device functions
	if (this->fpCreateSwapchainKHR == nullptr) {
		throw ao::core::Exception("Fail to get vkCreateSwapchainKHR()");
	}
	if (this->fpDestroySwapchainKHR == nullptr) {
		throw ao::core::Exception("Fail to get vkDestroySwapchainKHR()");
	}
	if (this->fpGetSwapchainImagesKHR == nullptr) {
		throw ao::core::Exception("Fail to get vkGetSwapchainImagesKHR()");
	}
	if (this->fpAcquireNextImageKHR == nullptr) {
		throw ao::core::Exception("Fail to get vkAcquireNextImageKHR()");
	}
	if (this->fpQueuePresentKHR == nullptr) {
		throw ao::core::Exception("Fail to get vkQueuePresentKHR()");
	}
}

void ao::vk::AOSwapChain::initSurface(VkSurfaceKHR& surface) {
	this->surface = surface;

	// Detect if a queue supports present
	std::vector<VkBool32> supportsPresent(this->device->queueFamilyProperties.size());
	for (uint32_t i = 0; i < supportsPresent.size(); i++) {
		fpGetPhysicalDeviceSurfaceSupportKHR(this->device->device, i, this->surface, &supportsPresent[i]);
	}

	// Try to find a queue that support graphics & present
	int graphicsQueueIndex = -1, presentQueueIndex = -1;
	for (size_t i = 0; i < this->device->queueFamilyProperties.size(); i++) {
		if (this->device->queueFamilyProperties[i].queueFlags & VkQueueFlagBits::VK_QUEUE_GRAPHICS_BIT) {
			if (graphicsQueueIndex == -1) {
				graphicsQueueIndex = (int)i;
			}

			if (supportsPresent[i] == VK_TRUE) {
				graphicsQueueIndex = presentQueueIndex = (int)i;
				break;
			}
		}
	}

	// Try to find separate queues
	if (presentQueueIndex == -1) {
		for (size_t i = 0; i < supportsPresent.size(); i++) {
			if (supportsPresent[i] == VK_TRUE) {
				presentQueueIndex = (int)i;
				break;
			}
		}
	}

	// Check indexes
	if (graphicsQueueIndex == -1 || presentQueueIndex == -1) {
		throw ao::core::Exception("Fail to find a queue that supports graphics & present");
	}
	if (graphicsQueueIndex != presentQueueIndex) {
		throw ao::core::Exception("Separate queue for graphics & present is not supported"); // TODO: Add support
	}
	this->queueIndex = graphicsQueueIndex;

	// Get surface formats
	std::vector<VkSurfaceFormatKHR> formats = ao::vk::utilities::surfaceFormatKHRs(this->device->device, this->surface, this->fpGetPhysicalDeviceSurfaceFormatsKHR);

	// No prefered format case
	if (formats.size() == 1 && formats.front().format == VkFormat::VK_FORMAT_UNDEFINED) {
		this->colorFormat = VkFormat::VK_FORMAT_B8G8R8A8_UNORM;
		this->colorSpace = formats.front().colorSpace;
	}
	else { // Find VK_FORMAT_B8G8R8A8_UNORM
		bool found = false;

		for (VkSurfaceFormatKHR& format : formats) {
			if (format.format == VkFormat::VK_FORMAT_B8G8R8A8_UNORM) {
				colorFormat = format.format;
				colorSpace = format.colorSpace;
				found = true;
				break;
			}
		}

		// Select the first format
		if (!found) {
			this->colorFormat = formats.front().format;
			this->colorSpace = formats.front().colorSpace;
		}
	}
}
