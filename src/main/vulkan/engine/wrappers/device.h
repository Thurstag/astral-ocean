#pragma once

#include <algorithm>
#include <vector>
#include <tuple>

#include <ao/core/exception.h>
#include <vulkan/vulkan.hpp>
#include <ao/core/logger.h>

#include "../../utilities/vulkan.h"

#define AO_GRAPHICS_QUEUE_INDEX 0
#define AO_COMPUTE_QUEUE_INDEX 1
#define AO_TRANSFER_QUEUE_INDEX 2

namespace ao {
	namespace vulkan {
		/// <summary>
		/// Wrapper for vulkan devices
		/// </summary>
		struct Device {
		public:
			std::vector<vk::QueueFamilyProperties> queueFamilyProperties;
			std::tuple<uint32_t, uint32_t, uint32_t> queueFamilyIndices;

			std::vector<vk::ExtensionProperties> extensions;

			vk::PhysicalDevice physical;
			vk::Format depthFormat;
			vk::Device logical;

			/// <summary>
			/// Constructor
			/// </summary>
			Device() = default;
			/// <summary>
			/// Constructor
			/// </summary>
			/// <param name="device"></param>
			Device(vk::PhysicalDevice& device);

			/// <summary>
			/// Destructor
			/// </summary>
			virtual ~Device();
			
			/// <summary>
			///	Method to init logical device
			/// </summary>
			/// <param name="deviceExtensions">Extensions</param>
			/// <param name="deviceFeatures">Features</param>
			/// <param name="qflags">Queue Flags</param>
			/// <param name="cflags">CommandPool Flags</param>
			/// <param name="swapChain">Enable swapChain</param>
			void initLogicalDevice(std::vector<char const*>& deviceExtensions, std::vector<vk::PhysicalDeviceFeatures>& deviceFeatures, vk::QueueFlags qflags, vk::CommandPoolCreateFlags cflags, bool swapChain = true);

			/// <summary>
			/// Method to get memory type
			/// </summary>
			/// <param name="typeBits">Type bits</param>
			/// <param name="properties">Properties</param>
			/// <returns>Index</returns>
			uint32_t memoryType(uint32_t typeBits, vk::MemoryPropertyFlags properties);
		protected:
			core::Logger LOGGER = core::Logger::getInstance<Device>();
		};
	}
}
