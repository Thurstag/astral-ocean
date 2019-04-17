// Copyright 2018-2019 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#include <ao/core/utilities/optional.h>
#include <vulkan/vulkan.hpp>

#include "../../wrapper/device.h"

namespace ao::vulkan {
    /**
     * @brief Allocator
     *
     */
    class Allocator {
       public:
        /**
         * @brief Buffer into
         *
         */
        struct BufferInfo {
            std::optional<void*> ptr;
            vk::DeviceSize size;
            vk::Buffer buffer;

            /**
             * @brief Construct a new BufferInfo object
             *
             * @param size Size
             * @param buffer Buffer
             * @param ptr Pointer
             */
            BufferInfo(vk::DeviceSize size = vk::DeviceSize(), vk::Buffer buffer = nullptr, std::optional<void*> ptr = std::nullopt)
                : size(size), buffer(buffer), ptr(ptr){};
        };

        /**
         * @brief Construct a new Allocator object
         *
         * @param device Device
         */
        Allocator(std::shared_ptr<Device> device) : device(device) {}

        /**
         * @brief Destroy the Allocator object
         *
         */
        virtual ~Allocator() = default;

        /**
         * @brief Allocate memory
         *
         * @param size Memory's size
         * @param usage Buffer usage
         * @return Allocator::BufferInfo Buffer info
         */
        virtual Allocator::BufferInfo allocate(vk::DeviceSize size, vk::BufferUsageFlags usage) = 0;

        /**
         * @brief Free memory
         *
         * @param info Buffer info
         */
        virtual void free(BufferInfo const& info) = 0;

        /**
         * @brief Align a size
         *
         * @param size Size
         * @return size_t aligned size
         */
        virtual size_t alignSize(size_t size) const = 0;

        /**
         * @brief Get memory size used by allocator
         *
         * @return vk::DeviceSize Memory size (in bytes)
         */
        virtual vk::DeviceSize size() const = 0;

        /**
         * @brief Notify allocator that buffer was updated
         *
         * @param info Buffer info
         * @param offset Memory offset
         * @param size Memory size
         */
        virtual void invalidate(BufferInfo const& info, vk::DeviceSize const& offset, vk::DeviceSize const& size) = 0;

        /**
         * @brief Check if allocator owns given buffer
         *
         * @param info Buffer info
         * @return true Allocator owns buffer
         * @return false Allocator doesn't own buffer
         */
        virtual bool own(BufferInfo const& info) const = 0;

       protected:
        std::shared_ptr<Device> device;
    };
}  // namespace ao::vulkan