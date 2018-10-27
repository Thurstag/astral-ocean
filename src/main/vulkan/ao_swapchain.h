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
			std::vector<VkCommandBuffer> commandBuffers;

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
			void init(uint64_t& width, uint64_t& height, bool vsync = false); // TODO: Remove vsync = false and search in AOEngine::settings.window
			/// <summary>
			/// Method to init surface
			/// </summary>
			void initSurface();
			/// <summary>
			/// Method to init command pool
			/// </summary>
			void initCommandPool();

			/// <summary>
			/// Method to create command buffers
			/// </summary>
			void createCommandBuffers();
			/// <summary>
			/// Method to init command buffers
			/// </summary>
			/// <param name="frameBuffers">Frame buffers</param>
			/// <param name="renderPass">Render pass</param>
			/// <param name="winSettings">Window settings</param>
			void initCommandBuffers(std::vector<VkFramebuffer>& frameBuffers, VkRenderPass& renderPass, WindowSettings& winSettings);
			/// <summary>
			/// Method to free command buffers
			/// </summary>
			void freeCommandBuffers();

			/// <summary>
			/// Method to get next image
			/// </summary>
			VkResult nextImage(VkSemaphore& present, uint32_t& imageIndex);
			/// <summary>
			/// Method to enqueue an image
			/// </summary>
			/// <returns></returns>
			VkResult enqueueImage(VkQueue& queue, uint32_t& imageIndex, VkSemaphore& render);
		protected:
			ao::core::Logger LOGGER = ao::core::Logger::getInstance<AOSwapChain>();
		private:
			VkInstance* instance;
			AODevice* device;
		};
	}
}
