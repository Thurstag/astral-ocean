// Copyright 2018 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#include <memory>

#include <ao/core/logger/logger.h>
#include <ao/core/utilities/pointers.h>

#include "../device.h"

namespace ao::vulkan {
    class Buffer {
       public:
        /// <summary>
        /// Constructor
        /// </summary>
        explicit Buffer(std::weak_ptr<Device> _device);

        /// <summary>
        /// Destructor
        /// </summary>
        ~Buffer() = default;

        /// <summary>
        /// Method to know if object has a buffer
        /// </summary>
        /// <returns>True or False</returns>
        virtual bool hasBuffer() const = 0;

        /// <summary>
        /// Method to get buffer
        /// </summary>
        /// <returns>vk::Buffer</returns>
        virtual vk::Buffer buffer() = 0;

        /// <summary>
        /// Method to get buffer's size
        /// </summary>
        /// <returns>Size</returns>
        virtual vk::DeviceSize size() const = 0;

        /// <summary>
        /// Method to get offset of fragment at index
        /// </summary>
        /// <param name="index">Fragment index</param>
        /// <returns>Offset</returns>
        virtual vk::DeviceSize offset(size_t index) const = 0;

        /// <summary>
        /// Method to map buffer
        /// </summary>
        /// <returns></returns>
        virtual Buffer* map() = 0;

        /// <summary>
        /// Method to calculate ubo size with aligment
        /// </summary>
        /// <param name="size">UBO's size</param>
        /// <returns>Aligment size</returns>
        size_t calculateUBOAligmentSize(size_t size) const;

        /// <summary>
        /// Method to calculate ubo size with aligment
        /// </summary>
        /// <param name="device">Device</param>
        /// <param name="size">UBO's size</param>
        /// <returns>Aligment size</returns>
        static size_t CalculateUBOAligmentSize(vk::PhysicalDevice device, size_t size);

       protected:
        core::Logger LOGGER = core::Logger::GetInstance<Buffer>();
        std::weak_ptr<Device> device;
    };
}  // namespace ao::vulkan