// Copyright 2018-2019 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#include "allocator.h"

namespace ao::vulkan {
    /**
     * @brief Allocator for device memory
     *
     */
    class DeviceAllocator : public Allocator {
       public:
        struct Allocation {
            std::pair<Allocator::BufferInfo, vk::DeviceMemory> host, device;
            vk::CommandBuffer command;
            Fence fence;
        };

        /**
         * @brief Construct a new DeviceAllocator object
         *
         * @param device Device
         * @param cmd_usage Command usage
         * @param alignment Alignment
         */
        DeviceAllocator(std::shared_ptr<Device> device, vk::CommandBufferUsageFlags cmd_usage, size_t alignment = 0);

        /**
         * @brief Destroy the Device Allocator object
         *
         */
        ~DeviceAllocator();

        /**
         * @brief Free host buffer
         *
         * @param info Info
         */
        virtual void freeHost(Allocator::BufferInfo const& info);

        /**
         * @brief Get memory used on device
         *
         * @return vk::DeviceSize Memory size (in bytes)
         */
        virtual vk::DeviceSize sizeOnDevice() const;

        /**
         * @brief Get memory used on device
         *
         * @return vk::DeviceSize Memory size (in bytes)
         */
        virtual vk::DeviceSize sizeOnHost() const;

        virtual void invalidate(BufferInfo const& info, vk::DeviceSize const& offset, vk::DeviceSize const& size) override;
        virtual Allocator::BufferInfo allocate(vk::DeviceSize size, vk::BufferUsageFlags usage) override;
        virtual void free(Allocator::BufferInfo const& info) override;
        virtual bool own(BufferInfo const& info) const override;
        virtual size_t alignSize(size_t size) const override;
        virtual vk::DeviceSize size() const override;

       protected:
        std::map<vk::Buffer, Allocation> allocations;
        std::mutex allocations_mutex;

        vk::CommandBufferUsageFlags cmd_usage;
        size_t alignment;
    };
}  // namespace ao::vulkan