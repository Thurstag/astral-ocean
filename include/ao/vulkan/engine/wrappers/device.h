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
        vk::CommandPool transfer_command_pool;
        QueueContainer queues;

        std::vector<vk::ExtensionProperties> extensions;

        vk::PhysicalDevice physical;
        vk::Format depth_format;
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
        /// <param name="deviceExtensions">Extensions</param>
        /// <param name="deviceFeatures">Features</param>
        /// <param name="qflags">Queue Flags</param>
        /// <param name="defaultQueue">Default queue for queue container</param>
        /// <param name="cflags">CommandPool Flags</param>
        /// <param name="swapChain">Enable swapChain</param>
        void initLogicalDevice(std::vector<char const*> deviceExtensions, std::vector<vk::PhysicalDeviceFeatures> const& deviceFeatures,
                               vk::QueueFlags qflags, vk::CommandPoolCreateFlags cflags,
                               vk::QueueFlagBits defaultQueue = vk::QueueFlagBits::eGraphics, bool swapChain = true);

        /// <summary>
        /// Method to get memory type
        /// </summary>
        /// <param name="typeBits">Type bits</param>
        /// <param name="properties">Properties</param>
        /// <returns>Index</returns>
        u32 memoryType(u32 typeBits, vk::MemoryPropertyFlags properties) const;

       protected:
        core::Logger LOGGER = core::Logger::GetInstance<Device>();
    };
}  // namespace ao::vulkan
