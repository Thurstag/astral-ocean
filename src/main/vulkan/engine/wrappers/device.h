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

#include "../../utilities/vulkan.h"
#include "../containers/queue_container.h"

namespace ao::vulkan {
    /// <summary>
    /// Wrapper for vulkan devices
    /// </summary>
    struct Device {
       public:
        vk::CommandPool command_pool;
        QueueContainer queues;

        std::vector<vk::ExtensionProperties> extensions;
        vk::Format depth_format;

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
        explicit Device(vk::PhysicalDevice const& device);

        /// <summary>
        /// Destructor
        /// </summary>
        virtual ~Device();

        /// <summary>
        ///	Method to init logical device
        /// </summary>
        /// <param name="device_extensions">Extensions</param>
        /// <param name="device_features">Features</param>
        /// <param name="qflags">Queue Flags</param>
        /// <param name="default_queue">Default queue for queue container</param>
        /// <param name="cflags">CommandPool Flags</param>
        /// <param name="swapchain_support">Enable swapchain_support</param>
        void initLogicalDevice(std::vector<char const*> device_extensions, std::vector<vk::PhysicalDeviceFeatures> const& device_features,
                               vk::QueueFlags qflags, vk::CommandPoolCreateFlags cflags,
                               vk::QueueFlagBits default_queue = vk::QueueFlagBits::eGraphics, bool swapchain_support = true);

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
        vk::ImageView createImageView(vk::Image& image, vk::Format format, vk::ImageViewType view_type, vk::ImageAspectFlags aspect_flags);

        /// <summary>
        /// Method to process an image
        /// </summary>
        /// <param name="image">Image</param>
        /// <param name="format">Format</param>
        /// <param name="old_layout">Old layout</param>
        /// <param name="new_layout">New layout</param>
        void processImage(vk::Image& image, vk::Format format, vk::ImageLayout old_layout, vk::ImageLayout new_layout);

        /// <summary>
        /// Method to get memory type
        /// </summary>
        /// <param name="type_bits">Type bits</param>
        /// <param name="properties">Properties</param>
        /// <returns>Index</returns>
        u32 memoryType(u32 type_bits, vk::MemoryPropertyFlags properties) const;

       protected:
        core::Logger LOGGER = core::Logger::GetInstance<Device>();
    };
}  // namespace ao::vulkan
