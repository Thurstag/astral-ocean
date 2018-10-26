#pragma once

#include <vulkan/vulkan.h>

#include "ao_device.h"

namespace ao {
	namespace vk {
		/// <summary>
		/// VkSwapChain	wrapper
		/// </summary>
		struct AOSwapChain {
		public:
		    /* FIELDS */

			VkColorSpaceKHR colorSpace;
			VkSurfaceKHR surface;
			uint32_t queueIndex;
			VkFormat colorFormat;
			
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

			/// <summary>
			/// Method to check function pointers
			/// </summary>
			void checkPFNs();
			/// <summary>
			/// Method to init surface
			/// </summary>
			/// <param name="device">Device</param>
			void initSurface(VkSurfaceKHR& surface);
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
