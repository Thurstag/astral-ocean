#pragma once

#include <core/exception.h>
#include <vulkan/vulkan.h>
#include <core/logger.h> 
#include <algorithm>
#include <vector>
#include <tuple>

#include "vk_utilities.h"

#define AO_GRAPHICS_QUEUE_INDEX 0
#define AO_COMPUTE_QUEUE_INDEX 1
#define AO_TRANSFER_QUEUE_INDEX 2

namespace ao {
	namespace vk {
		struct AODevice {
		public:
			/* FIELDS */
			std::vector<VkQueueFamilyProperties> queueFamilyProperties;
			std::tuple<uint32_t, uint32_t, uint32_t> queueFamilyIndices;

			VkPhysicalDeviceMemoryProperties memoryProperties;
			VkPhysicalDeviceProperties properties;
			VkPhysicalDeviceFeatures features;

			VkPhysicalDevice device;
			VkDevice logicalDevice;

			VkCommandPool commandPool;

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

			/// <summary>
			/// Destructor
			/// </summary>
			~AODevice();

			/* METHODS */ 
			
			/// <summary>
			///	Method to init logical device
			/// </summary>
			/// <param name="deviceExtensions">Extensions</param>
			/// <param name="qflags">Queue Flags</param>
			/// <param name="cflags">CommandPool Flags</param>
			/// <param name="swapChain">Enable swapChain</param>
			/// <returns>VkResult</returns>
			VkResult initLogicalDevice(std::vector<char const*> deviceExtensions, VkQueueFlags qflags, VkCommandPoolCreateFlags cflags, bool swapChain = true);
			/// <summary>
			/// Method to init command pool
			/// </summary>
			/// <param name="flags">Flags</param>
			/// <returns>VkResult</returns>
			VkResult initCommandPool(VkCommandPoolCreateFlags flags);
		protected:
			ao::core::Logger LOGGER = ao::core::Logger::getInstance<AODevice>();
		};
	}
}