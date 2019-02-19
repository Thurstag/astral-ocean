// Copyright 2018-2019 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#include <memory>

#include <ao/core/logger/logger.h>
#include <ao/core/utilities/pointers.h>

#include "../device.h"

namespace ao::vulkan {
    /**
     * @brief vk::Buffer wrapper
     *
     */
    class Buffer {
       public:
        /**
         * @brief Construct a new Buffer object
         *
         * @param device Device
         */
        explicit Buffer(std::weak_ptr<Device> device);

        /**
         * @brief Destroy the Buffer object
         *
         */
        ~Buffer() = default;

        /**
         * @brief Has buffer
         *
         * @return true Buffer has a buffer
         * @return false Buffer hasn't a buffer
         */
        virtual bool hasBuffer() const = 0;

        /**
         * @brief Buffer
         *
         * @return vk::Buffer Buffer
         */
        virtual vk::Buffer buffer() = 0;

        /**
         * @brief Buffer's size
         *
         * @return vk::DeviceSize Size
         */
        virtual vk::DeviceSize size() const = 0;

        /**
         * @brief Buffer's offset at index {index}
         *
         * @param index Index
         * @return vk::DeviceSize Offset
         */
        virtual vk::DeviceSize offset(size_t index) const = 0;

        /**
         * @brief Map buffer
         *
         * @return Buffer* Buffer
         */
        virtual Buffer* map() = 0;

        /**
         * @brief Free buffer
         *
         */
        virtual void free() = 0;

        /**
         * @brief Calculate UBO size with aligment
         *
         * @param size Size
         * @return size_t Size with aligment
         */
        size_t calculateUBOAligmentSize(size_t size) const;

        /**
         * @brief Calculate UBO size with aligment
         *
         * @param device Device
         * @param size Size
         * @return size_t Size with aligment
         */
        static size_t CalculateUBOAligmentSize(vk::PhysicalDevice device, size_t size);

       protected:
        core::Logger LOGGER = core::Logger::GetInstance<Buffer>();
        std::weak_ptr<Device> device;
    };
}  // namespace ao::vulkan
