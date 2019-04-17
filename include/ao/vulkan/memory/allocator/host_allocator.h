// Copyright 2018-2019 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#include <tuple>

#include "allocator.h"

namespace ao::vulkan {
    /**
     * @brief Allocator for host memory
     *
     */
    class HostAllocator : public Allocator {
       public:
        /**
         * @brief Construct a new HostAllocator object
         *
         * @param device Device
         * @param alignment Alignment
         */
        HostAllocator(std::shared_ptr<Device> device, size_t alignment = 0);

        /**
         * @brief Destroy the HostAllocator object
         *
         */
        virtual ~HostAllocator();

        virtual void invalidate(BufferInfo const& info, vk::DeviceSize const& offset, vk::DeviceSize const& size) override;
        virtual Allocator::BufferInfo allocate(vk::DeviceSize size, vk::BufferUsageFlags usage) override;
        virtual void free(Allocator::BufferInfo const& info) override;
        virtual bool own(BufferInfo const& info) const override;
        virtual size_t alignSize(size_t size) const override;
        virtual vk::DeviceSize size() const override;

       protected:
        std::map<vk::Buffer, std::pair<Allocator::BufferInfo, vk::DeviceMemory>> allocations;
        std::mutex allocations_mutex;

        size_t alignment;
    };
}  // namespace ao::vulkan