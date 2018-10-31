#pragma once

#include <algorithm>
#include <vector>
#include <tuple>

#include <ao/core/exception.h>
#include <vulkan/vulkan.hpp>
#include <ao/core/logger.h>

#include "vk_utilities.h"

#define AO_GRAPHICS_QUEUE_INDEX 0
#define AO_COMPUTE_QUEUE_INDEX 1
#define AO_TRANSFER_QUEUE_INDEX 2

namespace ao {
	namespace vulkan {
		/// <summary>
		/// vk::Device wrapper
		/// </summary>
		struct AODevice {
		public:
			/* FIELDS */
			std::vector<vk::QueueFamilyProperties> queueFamilyProperties;
			std::tuple<uint32_t, uint32_t, uint32_t> queueFamilyIndices;

			vk::PhysicalDevice physical;
			vk::Device logical;

			vk::CommandPool commandPool;
			vk::Format depthFormat;

			std::vector<vk::ExtensionProperties> extensions;

			/* CON/DESTRUCTORS */

			/// <summary>
			/// Constructor
			/// </summary>
			AODevice() = default;
			/// <summary>
			/// Constructor
			/// </summary>
			/// <param name="device"></param>
			AODevice(vk::PhysicalDevice& device);

			/// <summary>
			/// Destructor
			/// </summary>
			virtual ~AODevice();

			/* METHODS */ 
			
			/// <summary>
			///	Method to init logical device
			/// </summary>
			/// <param name="deviceExtensions">Extensions</param>
			/// <param name="qflags">Queue Flags</param>
			/// <param name="cflags">CommandPool Flags</param>
			/// <param name="swapChain">Enable swapChain</param>
			void initLogicalDevice(std::vector<char const*> deviceExtensions, vk::QueueFlags qflags, vk::CommandPoolCreateFlags cflags, bool swapChain = true);

			/// <summary>
			/// Method to get memory type
			/// </summary>
			/// <param name="typeBits">Type bits</param>
			/// <param name="properties">Properties</param>
			/// <returns>Index</returns>
			uint32_t memoryType(uint32_t typeBits, vk::MemoryPropertyFlags properties);
		protected:
			core::Logger LOGGER = core::Logger::getInstance<AODevice>();
		};
	}
}
