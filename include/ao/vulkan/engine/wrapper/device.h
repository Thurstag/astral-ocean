// Copyright 2018 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#include <algorithm>
#include <set>
#include <tuple>
#include <vector>

#include <ao/core/exception/exception.h>
#include <ao/core/logger/logger.h>
#include <vulkan/vulkan.hpp>

#include "../../utilities/queue.h"
#include "../../utilities/vulkan.h"
#include "../container/queue_container.h"
#include "command_pool.h"

namespace ao::vulkan {
    /// <summary>
    /// Wrapper for vulkan devices
    /// </summary>
    struct Device {
       public:
        std::unique_ptr<CommandPool> transfer_command_pool;
        vk::Format depth_format;
        QueueContainer queues;

        vk::PhysicalDevice physical;
        vk::Device logical;

        /// <summary>
        /// Constructor
        /// </summary>
        Device() = default;

        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="device"></param>
        explicit Device(vk::PhysicalDevice device);

        /// <summary>
        /// Destructor
        /// </summary>
        virtual ~Device();

        /// <summary>
        ///	Method to init logical device
        /// </summary>
        /// <param name="device_extensions">Extensions</param>
        /// <param name="device_features">Features</param>
        /// <param name="requested_queues">Requested queues</param>
        /// <param name="swapchain_support">Enable swapchain support</param>
        void initLogicalDevice(std::vector<char const*> device_extensions, std::vector<vk::PhysicalDeviceFeatures> const& device_features,
                               std::vector<QueueRequest> requested_queues, bool swapchain_support = true);

        /// <summary>
        /// Method to get surface formats
        /// </summary>
        /// <param name="surface">Surface</param>
        /// <returns>Surface formats</returns>
        std::vector<vk::SurfaceFormatKHR> surfaceFormatKHRs(vk::SurfaceKHR surface);

        /// <summary>
        /// Method to get swap chain images
        /// </summary>
        /// <param name="device">Device</param>
        /// <param name="swapChain">Swap chain</param>
        /// <returns>Images</returns>
        std::vector<vk::Image> swapChainImages(vk::SwapchainKHR swapChain);

        /// <summary>
        /// Method to create an image
        /// </summary>
        /// <param name="width">Width</param>
        /// <param name="height">Height</param>
        /// <param name="format">Format</param>
        /// <param name="type">Type</param>
        /// <param name="tilling">Tilling</param>
        /// <param name="usage_flags">Usage flags</param>
        /// <param name="memory_flags">Memory flags</param>
        /// <returns>Image and it's memory</returns>
        std::pair<vk::Image, vk::DeviceMemory> createImage(u32 width, u32 height, vk::Format format, vk::ImageType type, vk::ImageTiling tilling,
                                                           vk::ImageUsageFlags usage_flags, vk::MemoryPropertyFlags memory_flags);

        /// <summary>
        /// Method to create an image view
        /// </summary>
        /// <param name="image">Image</param>
        /// <param name="format">Format</param>
        /// <param name="view_type">View type</param>
        /// <param name="aspect_flags">Aspect flags</param>
        /// <returns>Image view</returns>
        vk::ImageView createImageView(vk::Image image, vk::Format format, vk::ImageViewType view_type, vk::ImageAspectFlags aspect_flags);

        /// <summary>
        /// Method to process an image
        /// </summary>
        /// <param name="image">Image</param>
        /// <param name="format">Format</param>
        /// <param name="old_layout">Old layout</param>
        /// <param name="new_layout">New layout</param>
        void processImage(vk::Image image, vk::Format format, vk::ImageLayout old_layout, vk::ImageLayout new_layout);

        /// <summary>
        /// Method to copy a buffer into an image
        /// </summary>
        /// <param name="buffer">Buffer</param>
        /// <param name="image">Image</param>
        /// <param name="width">Width</param>
        /// <param name="height">Height</param>
        void copyBufferToImage(vk::Buffer buffer, vk::Image image, u32 width, u32 height);

        /// <summary>
        /// Method to get memory type
        /// </summary>
        /// <param name="type_bits">Type bits</param>
        /// <param name="properties">Properties</param>
        /// <returns>Index</returns>
        u32 memoryType(u32 type_bits, vk::MemoryPropertyFlags properties) const;

       protected:
        core::Logger LOGGER = core::Logger::GetInstance<Device>();

        std::unique_ptr<CommandPool> graphics_command_pool;
    };
}  // namespace ao::vulkan