#pragma once

#include <vector>

#include "vulkan/vulkan.h"
#include "utilities.h"

namespace ao {
	namespace vk {
		struct AODevice {
			/* FIELDS */
			std::vector<VkQueueFamilyProperties> queueFamilyProperties;

			VkPhysicalDeviceMemoryProperties memoryProperties;
			VkPhysicalDeviceProperties properties;
			VkPhysicalDeviceFeatures features;

			VkPhysicalDevice device;

			std::vector<VkExtensionProperties> extensions;

			/* CON/DESTRUCTORS */

			/// <summary>
			/// Constructor
			/// </summary>
			AODevice() = default;

			/// <summary>
			/// Constructor
			/// </summary>
			/// <param name="device"></param>
			AODevice(VkPhysicalDevice& device);

			/* METHODS */
		};
	}
}
