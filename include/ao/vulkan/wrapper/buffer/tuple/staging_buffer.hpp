// Copyright 2018-2019 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#include "../../../exception/buffer_unitialized.h"
#include "../staging_buffer.h"
#include "basic_buffer.hpp"

namespace ao::vulkan {
    /**
     * @brief Staging TupleBuffer
     *
     * @tparam T
     */
    template<class... T>
    class StagingTupleBuffer : public virtual TupleBuffer<T...>, public virtual StagingBuffer {
       public:
        /**
         * @brief Construct a new StagingTupleBuffer object
         *
         * @param device Device
         * @param usage_flags Usage flags
         * @param memory_barrier Enable memory barrier
         */
        StagingTupleBuffer(std::weak_ptr<Device> device, vk::CommandBufferUsageFlags usage_flags = vk::CommandBufferUsageFlagBits::eSimultaneousUse,
                           bool memory_barrier = false);

        /**
         * @brief Destroy the StagingTupleBuffer object
         *
         */
        virtual ~StagingTupleBuffer() = default;

        /**
         * @brief Initialize buffer
         *
         * @param sizes Sizes
         * @param usage_flags usage flags
         * @return StagingTupleBuffer<T...>* Buffer
         */
        StagingTupleBuffer<T...>* init(std::initializer_list<vk::DeviceSize> const& sizes,
                                       std::optional<vk::BufferUsageFlags> usage_flags = std::nullopt);

        TupleBuffer<T...>* update(T const*... data) override;
        TupleBuffer<T...>* updateFragment(std::size_t index, void* const data) override;
        bool hasBuffer() const override;
        vk::Buffer buffer() override;
        vk::DeviceSize size() const override;
        vk::DeviceSize offset(size_t index) const override;
        Buffer* map() override;
        void free() override;
    };

    template<class... T>
    StagingTupleBuffer<T...>::StagingTupleBuffer(std::weak_ptr<Device> device, vk::CommandBufferUsageFlags usage_flags, bool memory_barrier)
        : TupleBuffer<T...>(device), StagingBuffer(device, usage_flags, memory_barrier) {}

    template<class... T>
    StagingTupleBuffer<T...>* StagingTupleBuffer<T...>::init(std::initializer_list<vk::DeviceSize> const& sizes,
                                                             std::optional<vk::BufferUsageFlags> usage_flags) {
        if (this->hasBuffer()) {
            StagingBuffer::free();
        }

        // Init buffer in host's memory
        this->host_buffer = std::shared_ptr<TupleBuffer<T...>>(
            (new BasicTupleBuffer<T...>(StagingBuffer::device))
                ->init(vk::BufferUsageFlagBits::eTransferSrc, vk::SharingMode::eExclusive,
                       vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, sizes));

        // Init buffer in device's memory
        this->device_buffer = std::shared_ptr<TupleBuffer<T...>>(
            (new BasicTupleBuffer<T...>(StagingBuffer::device))
                ->init(usage_flags ? vk::BufferUsageFlagBits::eTransferDst | *usage_flags : vk::BufferUsageFlagBits::eTransferDst,
                       vk::SharingMode::eExclusive, vk::MemoryPropertyFlagBits::eDeviceLocal, sizes));

        auto _device = ao::core::shared(StagingBuffer::device);

        // Create command buffer
        if (!this->command_buffer) {
            if (!_device->transfer_command_pool) {
                throw ao::core::Exception("Transfer command pool is disabled, request a graphics queue to enable it");
            }

            this->command_buffer = _device->transfer_command_pool->allocateCommandBuffers(vk::CommandBufferLevel::ePrimary, 1).front();
        }

        // Create fence
        if (!this->fence) {
            this->fence = _device->logical.createFence(vk::FenceCreateInfo(vk::FenceCreateFlagBits::eSignaled));
        }
        return this;
    }

    template<class... T>
    TupleBuffer<T...>* StagingTupleBuffer<T...>::update(T const*... data) {
        if (!this->hasBuffer()) {
            throw BufferUninitialized();
        }

        // Update host buffer
        if (auto host = static_cast<TupleBuffer<T...>*>(this->host_buffer.get())) {
            host->update(data...);
        } else {
            throw core::Exception("Fail to update host buffer");
        }

        // Synchronize
        this->sync();

        return this;
    }

    template<class... T>
    TupleBuffer<T...>* StagingTupleBuffer<T...>::updateFragment(std::size_t index, void* const data) {
        if (!this->hasBuffer()) {
            throw BufferUninitialized();
        }

        // Update host buffer
        if (auto host = static_cast<TupleBuffer<T...>*>(this->host_buffer.get())) {
            host->updateFragment(index, data);
        } else {
            throw core::Exception(fmt::format("Fail to update host buffer fragment: {0}", index));
        }

        // Synchronize
        this->sync();

        return this;
    }

    template<class... T>
    bool StagingTupleBuffer<T...>::hasBuffer() const {
        return StagingBuffer::hasBuffer();
    }

    template<class... T>
    vk::Buffer StagingTupleBuffer<T...>::buffer() {
        return StagingBuffer::buffer();
    }

    template<class... T>
    vk::DeviceSize StagingTupleBuffer<T...>::size() const {
        return StagingBuffer::size();
    }

    template<class... T>
    vk::DeviceSize StagingTupleBuffer<T...>::offset(size_t index) const {
        return StagingBuffer::offset(index);
    }

    template<class... T>
    Buffer* StagingTupleBuffer<T...>::map() {
        return StagingBuffer::map();
    }

    template<class... T>
    void StagingTupleBuffer<T...>::free() {
        return StagingBuffer::free();
    }
}  // namespace ao::vulkan
