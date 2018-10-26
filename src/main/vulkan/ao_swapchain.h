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

			VkCommandPool commandPool;
			VkSwapchainKHR swapChain = nullptr;

			std::vector<std::pair<VkImage, VkImageView>> buffers;

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
			/// Method to init swap chain
			/// </summary>
			/// <param name="width">Width</param>
			/// <param name="height">Height</param>
			/// <param name="vsync">Vsync enabled or not</param>
			void init(uint64_t& width, uint64_t& height, bool vsync = false);
			/// <summary>
			/// Method to init surface
			/// </summary>
			void initSurface();
			/// <summary>
			/// Method to init command pool
			/// </summary>
			void initCommandPool();
		protected:
			ao::core::Logger LOGGER = ao::core::Logger::getInstance<AOSwapChain>();
		private:
			VkInstance* instance;
			AODevice* device;
		};
	}
}
