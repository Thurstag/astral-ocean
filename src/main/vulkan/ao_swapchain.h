#pragma once

#include <vulkan/vulkan.hpp>

#include "ao_device.h"

namespace ao {
	namespace vulkan {
		/// <summary>
		/// vk::SwapChain wrapper
		/// </summary>
		struct AOSwapChain {
		public:
		    /* FIELDS */

			vk::CommandPool commandPool;
			vk::SwapchainKHR swapChain = nullptr;

			std::vector<std::pair<vk::Image, vk::ImageView>> buffers;
			std::vector<vk::CommandBuffer> commandBuffers;

			vk::ColorSpaceKHR colorSpace;
			vk::SurfaceKHR surface;
			uint32_t queueIndex;
			vk::Format colorFormat;
			
			/* CON/DESTRUCTORS */

			/// <summary>
			/// Constructor
			/// </summary>
			/// <param name="instance">Instance</param>
			/// <param name="device">Device</param>
			AOSwapChain(vk::Instance* instance, AODevice* device);

			/// <summary>
			/// Destructor
			/// </summary>
			virtual ~AOSwapChain();

			/* METHODS */

			/// <summary>
			/// Method to init swap chain
			/// </summary>
			/// <param name="width">Width</param>
			/// <param name="height">Height</param>
			/// <param name="vsync">Vsync enabled or not</param>
			void init(uint64_t& width, uint64_t& height, bool vsync);
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
			void initCommandBuffers(std::vector<vk::Framebuffer>& frameBuffers, vk::RenderPass& renderPass, WindowSettings& winSettings);
			/// <summary>
			/// Method to free command buffers
			/// </summary>
			void freeCommandBuffers();

			/// <summary>
			/// Method to get next image
			/// </summary>
			vk::Result nextImage(vk::Semaphore& present, uint32_t& imageIndex);
			/// <summary>
			/// Method to enqueue an image
			/// </summary>
			/// <returns></returns>
			vk::Result enqueueImage(vk::Queue& queue, uint32_t& imageIndex, vk::Semaphore& render);
		protected:
			ao::core::Logger LOGGER = ao::core::Logger::getInstance<AOSwapChain>();
		private:
			vk::Instance* instance;
			AODevice* device;
		};
	}
}
