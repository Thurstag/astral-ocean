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
