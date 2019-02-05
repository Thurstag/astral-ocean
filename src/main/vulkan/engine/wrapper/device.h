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
    /**
     * @brief vk::Device wrapper
     *
     */
    struct Device {
       public:
        std::unique_ptr<CommandPool> transfer_command_pool;
        vk::Format depth_format;
        QueueContainer queues;

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
         * @param swapchain_support Swapchain support
         */
        void initLogicalDevice(std::vector<char const*> device_extensions, std::vector<vk::PhysicalDeviceFeatures> const& device_features,
                               std::vector<QueueRequest> requested_queues, bool swapchain_support = true);

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
         * @param format Format
         * @param type Type
         * @param tilling Tilling
         * @param usage_flags Usage flags
         * @param memory_flags Memory flags
         * @return std::pair<vk::Image, vk::DeviceMemory> Image
         */
        std::pair<vk::Image, vk::DeviceMemory> createImage(u32 width, u32 height, vk::Format format, vk::ImageType type, vk::ImageTiling tilling,
                                                           vk::ImageUsageFlags usage_flags, vk::MemoryPropertyFlags memory_flags);

        /**
         * @brief Create an ImageView
         *
         * @param image Image
         * @param format Format
         * @param view_type View type
         * @param aspect_flags Aspect flags
         * @return vk::ImageView ImageView
         */
        vk::ImageView createImageView(vk::Image image, vk::Format format, vk::ImageViewType view_type, vk::ImageAspectFlags aspect_flags);

        /**
         * @brief Process an image
         *
         * @param image Image
         * @param format Format
         * @param old_layout Old layout
         * @param new_layout New layout
         */
        void processImage(vk::Image image, vk::Format format, vk::ImageLayout old_layout, vk::ImageLayout new_layout);

        /**
         * @brief Copy vk::Buffer into an vk::Image
         *
         * @param buffer Buffer
         * @param image Image
         * @param width Width
         * @param height Height
         */
        void copyBufferToImage(vk::Buffer buffer, vk::Image image, u32 width, u32 height);

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
