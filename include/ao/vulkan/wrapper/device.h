// Copyright 2018-2019 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#include <algorithm>
#include <set>
#include <tuple>
#include <vector>

#include <ao/core/exception/exception.h>
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
    class Device {
       public:
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
        std::vector<vk::SurfaceFormatKHR> surfaceFormatKHRs(vk::SurfaceKHR surface) const {
            return this->physical_.getSurfaceFormatsKHR(surface);
        }

        /**
         * @brief Swapchain's images
         *
         * @param swapchain Swapchain
         * @return std::vector<vk::Image> Images
         */
        std::vector<vk::Image> swapChainImages(vk::SwapchainKHR swapchain) const {
            return this->logical_->getSwapchainImagesKHR(swapchain);
        }

        /**
         * @brief Get transfer command pool
         *
         * @return CommandPool& Command pool
         */
        CommandPool& transferPool();

        /**
         * @brief Get graphics command pool
         *
         * @return CommandPool& Command pool
         */
        CommandPool& graphicsPool();

        /**
         * @brief Get queues
         *
         * @return QueueContainer const& Queue
         */
        std::unique_ptr<QueueContainer> const& queues() const {
            return this->queues_;
        }

        /**
         * @brief Get logical device
         *
         * @return vk::Device Device
         */
        std::shared_ptr<vk::Device> const logical() const {
            return this->logical_;
        }

        vk::PhysicalDevice physical() const {
            return this->physical_;
        }

       protected:
        std::unique_ptr<CommandPool> transfer_command_pool;
        std::unique_ptr<CommandPool> graphics_command_pool;
        std::unique_ptr<QueueContainer> queues_;

        std::shared_ptr<vk::Device> logical_;
        vk::PhysicalDevice physical_;
    };
}  // namespace ao::vulkan
