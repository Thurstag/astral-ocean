#pragma once

#include <vector>
#include <limits>
#include <tuple>
#include <array>

#include <vulkan/vulkan.hpp>

#include "device.h"

namespace ao {
	namespace vulkan {
		/// <summary>
		/// Wrapper for vulkan swap chain
		/// </summary>
		struct SwapChain {
		public:
			std::vector<std::pair<vk::Image, vk::ImageView>> buffers;
			vk::SwapchainKHR swapChain;

			std::pair<std::array<vk::ClearValue, 2>, vk::Rect2D> commandBufferHelpers;
			std::vector<vk::CommandBuffer> secondaryCommandBuffers;
			std::vector<vk::CommandBuffer> primaryCommandBuffers;
			vk::CommandPool commandPool;

			vk::ColorSpaceKHR colorSpace;
			vk::Extent2D currentExtent;
			vk::Format colorFormat;
			vk::SurfaceKHR surface;
			uint32_t queueIndex;

			/// <summary>
			/// Constructor
			/// </summary>
			/// <param name="_instance">Instance</param>
			/// <param name="_device">Device</param>
			SwapChain(vk::Instance* _instance, Device* _device);

			/// <summary>
			/// Destructor
			/// </summary>
			virtual ~SwapChain();

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
			core::Logger LOGGER = core::Logger::getInstance<SwapChain>();

			vk::Instance* const instance;
			Device* const device;
		};
	}
}
