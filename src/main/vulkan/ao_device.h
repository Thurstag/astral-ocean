#pragma once

#include <core/exception.h>
#include <vulkan/vulkan.h>
#include <core/logger.h>
#include <vector>

#include "vk_utilities.h"

namespace ao {
	namespace vk {
		struct AODevice {
		public:
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

		protected:
			ao::core::Logger LOGGER = ao::core::Logger::getInstance<AODevice>();
		};
	}
}
