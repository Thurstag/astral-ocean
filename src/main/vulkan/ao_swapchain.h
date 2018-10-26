#pragma once

#include <vulkan/vulkan.h>

#include "ao_device.h"

namespace ao {
	namespace vk {
		/// <summary>
		/// VkSwapChain	wrapper
		/// </summary>
		struct AOSwapChain {
		    /* FIELDS */

			
			/* CON/DESTRUCTORS */

			/// <summary>
			/// Constructor
			/// </summary>
			/// <param name="instance">Instance</param>
			/// <param name="device">Device</param>
			AOSwapChain(VkInstance* instance, AODevice* device);

			/// <summary>
			/// Destructor
			/// </summary>
			~AOSwapChain();

			/* METHODS */

			void checkPFNs();
		private:
			VkInstance* instance;
			AODevice* device;

			// Function pointers
			PFN_vkGetPhysicalDeviceSurfaceSupportKHR fpGetPhysicalDeviceSurfaceSupportKHR;
			PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR fpGetPhysicalDeviceSurfaceCapabilitiesKHR;
			PFN_vkGetPhysicalDeviceSurfaceFormatsKHR fpGetPhysicalDeviceSurfaceFormatsKHR;
			PFN_vkGetPhysicalDeviceSurfacePresentModesKHR fpGetPhysicalDeviceSurfacePresentModesKHR;
			PFN_vkCreateSwapchainKHR fpCreateSwapchainKHR;
			PFN_vkDestroySwapchainKHR fpDestroySwapchainKHR;
			PFN_vkGetSwapchainImagesKHR fpGetSwapchainImagesKHR;
			PFN_vkAcquireNextImageKHR fpAcquireNextImageKHR;
			PFN_vkQueuePresentKHR fpQueuePresentKHR;
		};
	}
}
