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

#include "../container/queue_container.h"
#include "../utilities/queue.h"
#include "../utilities/vulkan.h"
#include "command_pool.h"

namespace ao::vulkan {
    /**
     * @brief vk::Device wrapper
     *
     */
    struct Device {
       public:
        std::unique_ptr<CommandPool> transfer_command_pool;
        std::unique_ptr<QueueContainer> queues;
        vk::Format depth_format;

        vk::PhysicalDevice physical;
        vk::Device logical;

        /**
         * @brief Construct a new Device object
         *
         */
        Device() = default;

        /**
         * @brief Construct a new Device object
         *
         * @param device Device
         */
        explicit Device(vk::PhysicalDevice device);

        /**
         * @brief Destroy the Device object
         *
         */
        virtual ~Device();

        /**
         * @brief Initialize logical device
         *
         * @param device_extensions Device's extensions to enable
         * @param device_features Device's features to enable
         * @param requested_queues Requested queues
         */
        void initLogicalDevice(vk::ArrayProxy<char const* const> device_extensions, vk::ArrayProxy<vk::PhysicalDeviceFeatures const> device_features,
                               vk::ArrayProxy<QueueRequest const> requested_queues);

        /**
         * @brief Surface formats
         *
         * @param surface Surface
         * @return std::vector<vk::SurfaceFormatKHR> Formats
         */
        std::vector<vk::SurfaceFormatKHR> surfaceFormatKHRs(vk::SurfaceKHR surface);

        /**
         * @brief Swapchain's images
         *
         * @param swapchain Swapchain
         * @return std::vector<vk::Image> Images
         */
        std::vector<vk::Image> swapChainImages(vk::SwapchainKHR swapchain);

        /**
         * @brief Create an Image
         *
         * @param width Width
         * @param height height
         * @param mip_levels Mip levels
         * @param array_layers Array layers
         * @param format Format
         * @param type Type
         * @param tilling Tilling
         * @param usage_flags Usage flags
         * @param memory_flags Memory flags
         * @return std::pair<vk::Image, vk::DeviceMemory> Image
         */
        std::pair<vk::Image, vk::DeviceMemory> createImage(u32 width, u32 height, u32 mip_levels, u32 array_layers, vk::Format format,
                                                           vk::ImageType type, vk::ImageTiling tilling, vk::ImageUsageFlags usage_flags,
                                                           vk::MemoryPropertyFlags memory_flags);

        /**
         * @brief Create an ImageView
         *
         * @param image Image
         * @param format Format
         * @param view_type View type
         * @param subresource_range Subresource range
         * @return vk::ImageView ImageView
         */
        vk::ImageView createImageView(vk::Image image, vk::Format format, vk::ImageViewType view_type, vk::ImageSubresourceRange subresource_range);

        /**
         * @brief Process an image
         *
         * @param image Image
         * @param format Format
         * @param subresource_range Subresource range
         * @param old_layout Old layout
         * @param new_layout New layout
         */
        void processImage(vk::Image image, vk::Format format, vk::ImageSubresourceRange subresource_range, vk::ImageLayout old_layout,
                          vk::ImageLayout new_layout);

        /**
         * @brief Copy vk::Buffer into an vk::Image
         *
         * @param buffer Buffer
         * @param image Image
         * @param regions Regions
         */
        void copyBufferToImage(vk::Buffer buffer, vk::Image image, vk::ArrayProxy<vk::BufferImageCopy const> regions);

        /**
         * @brief Memory type
         *
         * @param type_bits Type bits
         * @param properties Properties
         * @return u32 Index
         */
        u32 memoryType(u32 type_bits, vk::MemoryPropertyFlags properties) const;

       protected:
        core::Logger LOGGER = core::Logger::GetInstance<Device>();

        std::unique_ptr<CommandPool> graphics_command_pool;
    };
}  // namespace ao::vulkan
