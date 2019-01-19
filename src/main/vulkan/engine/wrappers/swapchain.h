// Copyright 2018 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#include <vector>
#include <limits>
#include <tuple>
#include <array>

#include <vulkan/vulkan.hpp>

#include "../containers/commandbuffer_container.h"
#include "device.h"

namespace ao::vulkan {
	/// <summary>
	/// Wrapper for vulkan swap chain
	/// </summary>
	struct SwapChain {
	public:
		std::vector<std::pair<vk::Image, vk::ImageView>> buffers;
		vk::Queue presentQueue;
		vk::SwapchainKHR swapChain;

		std::pair<std::array<vk::ClearValue, 2>, vk::Rect2D> commandBufferHelpers;
		CommandBufferContainer commandBuffers;
		vk::CommandPool commandPool;

		vk::ColorSpaceKHR colorSpace;
		vk::Extent2D currentExtent;
		vk::Format colorFormat;
		vk::SurfaceKHR surface;

		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="_instance">Instance</param>
		/// <param name="_device">Device</param>
		SwapChain(std::weak_ptr<vk::Instance> _instance, std::weak_ptr<Device> _device);

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
		void init(u64& width, u64& height, bool vsync);
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
		/// <param name="acquire">Acquire semaphore (Signal semaphore)</param>
		/// <param name="imageIndex">Image index</param>
		vk::Result nextImage(vk::Semaphore& acquire, u32& imageIndex);
		/// <summary>
		/// Method to enqueue an image
		/// </summary>
		/// <param name="imageIndex">ImageIndex</param>
		/// <param name="waitSemaphores">Waiting semaphores</param>
		/// <returns>Result</returns>
		vk::Result enqueueImage(u32& imageIndex, std::vector<vk::Semaphore>& waitSemaphores);
	protected:
		core::Logger LOGGER = core::Logger::getInstance<SwapChain>();

		std::weak_ptr<vk::Instance> instance;
		std::weak_ptr<Device> device;
	};
}
