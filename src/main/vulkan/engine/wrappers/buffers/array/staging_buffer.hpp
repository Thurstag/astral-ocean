// Copyright 2018 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#include "../../../exception/buffer_unitialized.h"
#include "../staging_buffer.h"
#include "basic_buffer.hpp"

namespace ao::vulkan {
    /// <summary>
    /// StagingDynamicArrayBuffer class
    /// </summary>
    template<class T>
    class StagingDynamicArrayBuffer : public virtual DynamicArrayBuffer<T>, public virtual StagingBuffer {
       public:
        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="count">Count</param>
        /// <param name="device">Device</param>
        /// <param name="usage_flags">Usage</param>
        /// <param name="memory_barrier">Bind memory barrier on transfer</param>
        StagingDynamicArrayBuffer(size_t count, std::weak_ptr<Device> device,
                                  vk::CommandBufferUsageFlags usage_flags = vk::CommandBufferUsageFlagBits::eSimultaneousUse,
                                  bool memory_barrier = false);

        /// <summary>
        /// Destructor
        /// </summary>
        virtual ~StagingDynamicArrayBuffer() = default;

        /// <summary>
        /// Method to init buffer
        ///
        /// If object already stores a buffer, it will free the old one
        /// </summary>
        /// <param name="size">Fragment size</param>
        /// <param name="usage_flags">Usage flags</param>
        /// <returns>This</returns>
        StagingDynamicArrayBuffer<T>* init(vk::DeviceSize size, std::optional<vk::BufferUsageFlags> usage_flags = std::nullopt);

        DynamicArrayBuffer<T>* update(std::vector<T> const& data) override;
        DynamicArrayBuffer<T>* updateFragment(std::size_t index, T const* data) override;
        bool hasBuffer() const override;
        vk::Buffer buffer() override;
        vk::DeviceSize size() const override;
        vk::DeviceSize offset(size_t index) const override;
        Buffer* map() override;
    };

    template<class T>
    StagingDynamicArrayBuffer<T>::StagingDynamicArrayBuffer(size_t count, std::weak_ptr<Device> device, vk::CommandBufferUsageFlags usage_flags,
                                                            bool memory_barrier)
        : DynamicArrayBuffer<T>(count, device), StagingBuffer(device, usage_flags, memory_barrier) {}

    template<class T>
    StagingDynamicArrayBuffer<T>* StagingDynamicArrayBuffer<T>::init(vk::DeviceSize size, std::optional<vk::BufferUsageFlags> usage_flags) {
        if (this->hasBuffer()) {
            this->free();
        }

        // Init buffer in host's memory
        this->host_buffer = std::shared_ptr<DynamicArrayBuffer<T>>(
            (new BasicDynamicArrayBuffer<T>(this->count, StagingBuffer::device))
                ->init(vk::BufferUsageFlagBits::eTransferSrc, vk::SharingMode::eExclusive,
                       vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, size));

        // Init buffer in device's memory
        this->device_buffer = std::shared_ptr<DynamicArrayBuffer<T>>(
            (new BasicDynamicArrayBuffer<T>(this->count, StagingBuffer::device))
                ->init(usage_flags ? vk::BufferUsageFlagBits::eTransferDst | usage_flags.value() : vk::BufferUsageFlagBits::eTransferDst,
                       vk::SharingMode::eExclusive, vk::MemoryPropertyFlagBits::eDeviceLocal, size));

        auto _device = ao::core::shared(StagingBuffer::device);
        // Create command buffer
        this->command_buffer =
            _device->logical.allocateCommandBuffers(vk::CommandBufferAllocateInfo(_device->command_pool, vk::CommandBufferLevel::ePrimary, 1))[0];

        // Create fence
        this->fence = _device->logical.createFence(vk::FenceCreateInfo(vk::FenceCreateFlagBits::eSignaled));
        _device->logical.resetFences(fence);
        return this;
    }

    template<class T>
    DynamicArrayBuffer<T>* StagingDynamicArrayBuffer<T>::update(std::vector<T> const& data) {
        if (!this->hasBuffer()) {
            throw BufferUninitialized();
        }

        // Update host buffer & synchronize memories
        if (auto host = static_cast<DynamicArrayBuffer<T>*>(this->host_buffer.get())) {
            host->update(data);
        } else {
            throw core::Exception("Fail to update host buffer");
        }
        this->sync();

        return this;
    }

    template<class T>
    DynamicArrayBuffer<T>* StagingDynamicArrayBuffer<T>::updateFragment(std::size_t index, T const* data) {
        if (!this->hasBuffer()) {
            throw BufferUninitialized();
        }

        // Update host buffer & synchronize memories
        if (auto host = static_cast<DynamicArrayBuffer<T>*>(this->host_buffer.get())) {
            host->updateFragment(index, data);
        } else {
            throw BufferUninitialized();
        }
        this->sync();

        return this;
    }

    template<class T>
    bool StagingDynamicArrayBuffer<T>::hasBuffer() const {
        return StagingBuffer::hasBuffer();
    }

    template<class T>
    vk::Buffer StagingDynamicArrayBuffer<T>::buffer() {
        return StagingBuffer::buffer();
    }

    template<class T>
    vk::DeviceSize StagingDynamicArrayBuffer<T>::size() const {
        return StagingBuffer::size();
    }

    template<class T>
    vk::DeviceSize StagingDynamicArrayBuffer<T>::offset(size_t index) const {
        return StagingBuffer::offset(index);
    }

    template<class T>
    Buffer* StagingDynamicArrayBuffer<T>::map() {
        return StagingBuffer::map();
    }

    template<class T, size_t N>
    class StagingArrayBuffer : public virtual ArrayBuffer<T, N>, public StagingBuffer {
       public:
        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="device">Device</param>
        /// <param name="usage_flags">Usage</param>
        /// <param name="memory_barrier">Bind memory barrier on transfer</param>
        StagingArrayBuffer(std::weak_ptr<Device> device, vk::CommandBufferUsageFlags usage_flags = vk::CommandBufferUsageFlagBits::eSimultaneousUse,
                           bool memory_barrier = false);

        /// <summary>
        /// Destructor
        /// </summary>
        virtual ~StagingArrayBuffer() = default;

        /// <summary>
        /// Method to init buffer
        ///
        /// If object already stores a buffer, it will free the old one
        /// </summary>
        /// <param name="size">Fragment size</param>
        /// <param name="usage_flags">Usage flags</param>
        /// <returns>This</returns>
        StagingArrayBuffer<T, N>* init(vk::DeviceSize size, std::optional<vk::BufferUsageFlags> usage_flags = std::nullopt);

        ArrayBuffer<T, N>* update(std::array<T, N> const& data) override;
        ArrayBuffer<T, N>* updateFragment(std::size_t index, T const* data) override;
        bool hasBuffer() const override;
        vk::Buffer buffer() override;
        vk::DeviceSize size() const override;
        vk::DeviceSize offset(size_t index) const override;
        Buffer* map() override;
    };

    template<class T, size_t N>
    StagingArrayBuffer<T, N>::StagingArrayBuffer(std::weak_ptr<Device> device, vk::CommandBufferUsageFlags usage_flags, bool memory_barrier)
        : ArrayBuffer<T, N>(device), StagingBuffer(device, usage_flags, memory_barrier) {}

    template<class T, size_t N>
    StagingArrayBuffer<T, N>* StagingArrayBuffer<T, N>::init(vk::DeviceSize size, std::optional<vk::BufferUsageFlags> usage_flags) {
        if (this->hasBuffer()) {
            this->free();
        }

        // Init buffer in host's memory
        this->host_buffer = std::shared_ptr<ArrayBuffer<T, N>>(
            (new BasicArrayBuffer<T, N>(StagingBuffer::device))
                ->init(vk::BufferUsageFlagBits::eTransferSrc, vk::SharingMode::eExclusive,
                       vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, size));

        // Init buffer in device's memory
        this->device_buffer = std::shared_ptr<ArrayBuffer<T, N>>(
            (new BasicArrayBuffer<T, N>(StagingBuffer::device))
                ->init(usage_flags ? vk::BufferUsageFlagBits::eTransferDst | usage_flags.value() : vk::BufferUsageFlagBits::eTransferDst,
                       vk::SharingMode::eExclusive, vk::MemoryPropertyFlagBits::eDeviceLocal, size));

        auto _device = ao::core::shared(StagingBuffer::device);
        // Create command buffer
        this->command_buffer =
            _device->logical.allocateCommandBuffers(vk::CommandBufferAllocateInfo(_device->command_pool, vk::CommandBufferLevel::ePrimary, 1))[0];

        // Create fence
        this->fence = _device->logical.createFence(vk::FenceCreateInfo(vk::FenceCreateFlagBits::eSignaled));
        _device->logical.resetFences(fence);
        return this;
    }

    template<class T, size_t N>
    ArrayBuffer<T, N>* StagingArrayBuffer<T, N>::update(std::array<T, N> const& data) {
        if (!this->hasBuffer()) {
            throw BufferUninitialized();
        }

        // Update host buffer & synchronize memories
        if (auto host = static_cast<ArrayBuffer<T, N>*>(this->host_buffer.get())) {
            host->update(data);
        } else {
            throw core::Exception("Fail to update host buffer");
        }
        this->sync();

        return this;
    }

    template<class T, size_t N>
    ArrayBuffer<T, N>* StagingArrayBuffer<T, N>::updateFragment(std::size_t index, T const* data) {
        if (!this->hasBuffer()) {
            throw BufferUninitialized();
        }

        // Update host buffer & synchronize memories
        if (auto host = static_cast<ArrayBuffer<T, N>*>(this->host_buffer.get())) {
            host->updateFragment(index, data);
        } else {
            throw core::Exception(fmt::format("Fail to update host buffer fragment: {0}", index));
        }
        this->sync();

        return this;
    }

    template<class T, size_t N>
    bool StagingArrayBuffer<T, N>::hasBuffer() const {
        return StagingBuffer::hasBuffer();
    }

    template<class T, size_t N>
    vk::Buffer StagingArrayBuffer<T, N>::buffer() {
        return StagingBuffer::buffer();
    }

    template<class T, size_t N>
    vk::DeviceSize StagingArrayBuffer<T, N>::size() const {
        return StagingBuffer::size();
    }

    template<class T, size_t N>
    vk::DeviceSize StagingArrayBuffer<T, N>::offset(size_t index) const {
        return StagingBuffer::offset(index);
    }

    template<class T, size_t N>
    Buffer* StagingArrayBuffer<T, N>::map() {
        return StagingBuffer::map();
    }
}  // namespace ao::vulkan
