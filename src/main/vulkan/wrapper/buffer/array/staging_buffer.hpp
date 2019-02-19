// Copyright 2018-2019 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#include "../../../exception/buffer_unitialized.h"
#include "../staging_buffer.h"
#include "basic_buffer.hpp"

namespace ao::vulkan {
    /**
     * @brief Staging array of buffers
     *
     * @tparam T Buffer type
     */
    template<class T>
    class StagingDynamicArrayBuffer : public virtual DynamicArrayBuffer<T>, public virtual StagingBuffer {
       public:
        /**
         * @brief Construct a new StagingDynamicArrayBuffer object
         *
         * @param count Count
         * @param device Device
         * @param usage_flags usage flags
         * @param memory_barrier Enable memory barrier
         */
        StagingDynamicArrayBuffer(size_t count, std::weak_ptr<Device> device,
                                  vk::CommandBufferUsageFlags usage_flags = vk::CommandBufferUsageFlagBits::eSimultaneousUse,
                                  bool memory_barrier = false);

        /**
         * @brief Destroy the StagingDynamicArrayBuffer object
         *
         */
        virtual ~StagingDynamicArrayBuffer() = default;

        /**
         * @brief Initialize buffer
         *
         * @param size Size
         * @param usage_flags Usage flags
         * @return StagingDynamicArrayBuffer<T>* Buffer
         */
        StagingDynamicArrayBuffer<T>* init(vk::DeviceSize size, std::optional<vk::BufferUsageFlags> usage_flags = std::nullopt);

        DynamicArrayBuffer<T>* update(std::vector<T> const& data) override;
        DynamicArrayBuffer<T>* updateFragment(std::size_t index, T const* data) override;
        bool hasBuffer() const override;
        vk::Buffer buffer() override;
        vk::DeviceSize size() const override;
        vk::DeviceSize offset(size_t index) const override;
        Buffer* map() override;
        void free() override;
    };

    template<class T>
    StagingDynamicArrayBuffer<T>::StagingDynamicArrayBuffer(size_t count, std::weak_ptr<Device> device, vk::CommandBufferUsageFlags usage_flags,
                                                            bool memory_barrier)
        : DynamicArrayBuffer<T>(count, device), StagingBuffer(device, usage_flags, memory_barrier) {}

    template<class T>
    StagingDynamicArrayBuffer<T>* StagingDynamicArrayBuffer<T>::init(vk::DeviceSize size, std::optional<vk::BufferUsageFlags> usage_flags) {
        if (this->hasBuffer()) {
            StagingBuffer::free();
        }

        // Init buffer in host's memory
        this->host_buffer = std::shared_ptr<DynamicArrayBuffer<T>>(
            (new BasicDynamicArrayBuffer<T>(this->count, StagingBuffer::device))
                ->init(vk::BufferUsageFlagBits::eTransferSrc, vk::SharingMode::eExclusive,
                       vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, size));

        // Init buffer in device's memory
        this->device_buffer = std::shared_ptr<DynamicArrayBuffer<T>>(
            (new BasicDynamicArrayBuffer<T>(this->count, StagingBuffer::device))
                ->init(usage_flags ? vk::BufferUsageFlagBits::eTransferDst | *usage_flags : vk::BufferUsageFlagBits::eTransferDst,
                       vk::SharingMode::eExclusive, vk::MemoryPropertyFlagBits::eDeviceLocal, size));

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

    template<class T>
    DynamicArrayBuffer<T>* StagingDynamicArrayBuffer<T>::update(std::vector<T> const& data) {
        if (!this->hasBuffer()) {
            throw BufferUninitialized();
        }

        // Update host buffer
        if (auto host = static_cast<DynamicArrayBuffer<T>*>(this->host_buffer.get())) {
            host->update(data);
        } else {
            throw core::Exception("Fail to update host buffer");
        }

        // Synchronize
        this->sync();

        return this;
    }

    template<class T>
    DynamicArrayBuffer<T>* StagingDynamicArrayBuffer<T>::updateFragment(std::size_t index, T const* data) {
        if (!this->hasBuffer()) {
            throw BufferUninitialized();
        }

        // Update host buffer
        if (auto host = static_cast<DynamicArrayBuffer<T>*>(this->host_buffer.get())) {
            host->updateFragment(index, data);
        } else {
            throw BufferUninitialized();
        }

        // Synchronize
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

    template<class T>
    void StagingDynamicArrayBuffer<T>::free() {
        return StagingBuffer::free();
    }

    /**
     * @brief Staging array of buffers
     *
     * @tparam T Buffer type
     * @tparam N Array's size
     */
    template<class T, size_t N>
    class StagingArrayBuffer : public virtual ArrayBuffer<T, N>, public StagingBuffer {
       public:
        /**
         * @brief Construct a new StagingArrayBuffer object
         *
         * @param device Device
         * @param usage_flags Usage flags
         * @param memory_barrier Enable memory barrier
         */
        StagingArrayBuffer(std::weak_ptr<Device> device, vk::CommandBufferUsageFlags usage_flags = vk::CommandBufferUsageFlagBits::eSimultaneousUse,
                           bool memory_barrier = false);

        /**
         * @brief Destroy the StagingArrayBuffer object
         *
         */
        virtual ~StagingArrayBuffer() = default;

        /**
         * @brief Initialize buffer
         *
         * @param size Size
         * @param usage_flags Usage flags
         * @return StagingArrayBuffer<T, N>* Buffer
         */
        StagingArrayBuffer<T, N>* init(vk::DeviceSize size, std::optional<vk::BufferUsageFlags> usage_flags = std::nullopt);

        ArrayBuffer<T, N>* update(std::array<T, N> const& data) override;
        ArrayBuffer<T, N>* updateFragment(std::size_t index, T const* data) override;
        bool hasBuffer() const override;
        vk::Buffer buffer() override;
        vk::DeviceSize size() const override;
        vk::DeviceSize offset(size_t index) const override;
        Buffer* map() override;
        void free() override;
    };

    template<class T, size_t N>
    StagingArrayBuffer<T, N>::StagingArrayBuffer(std::weak_ptr<Device> device, vk::CommandBufferUsageFlags usage_flags, bool memory_barrier)
        : ArrayBuffer<T, N>(device), StagingBuffer(device, usage_flags, memory_barrier) {}

    template<class T, size_t N>
    StagingArrayBuffer<T, N>* StagingArrayBuffer<T, N>::init(vk::DeviceSize size, std::optional<vk::BufferUsageFlags> usage_flags) {
        if (this->hasBuffer()) {
            StagingBuffer::free();
        }

        // Init buffer in host's memory
        this->host_buffer = std::shared_ptr<ArrayBuffer<T, N>>(
            (new BasicArrayBuffer<T, N>(StagingBuffer::device))
                ->init(vk::BufferUsageFlagBits::eTransferSrc, vk::SharingMode::eExclusive,
                       vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, size));

        // Init buffer in device's memory
        this->device_buffer = std::shared_ptr<ArrayBuffer<T, N>>(
            (new BasicArrayBuffer<T, N>(StagingBuffer::device))
                ->init(usage_flags ? vk::BufferUsageFlagBits::eTransferDst | *usage_flags : vk::BufferUsageFlagBits::eTransferDst,
                       vk::SharingMode::eExclusive, vk::MemoryPropertyFlagBits::eDeviceLocal, size));

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

    template<class T, size_t N>
    ArrayBuffer<T, N>* StagingArrayBuffer<T, N>::update(std::array<T, N> const& data) {
        if (!this->hasBuffer()) {
            throw BufferUninitialized();
        }

        // Update host buffer
        if (auto host = static_cast<ArrayBuffer<T, N>*>(this->host_buffer.get())) {
            host->update(data);
        } else {
            throw core::Exception("Fail to update host buffer");
        }

        // Synchronize
        this->sync();

        return this;
    }

    template<class T, size_t N>
    ArrayBuffer<T, N>* StagingArrayBuffer<T, N>::updateFragment(std::size_t index, T const* data) {
        if (!this->hasBuffer()) {
            throw BufferUninitialized();
        }

        // Update host buffer
        if (auto host = static_cast<ArrayBuffer<T, N>*>(this->host_buffer.get())) {
            host->updateFragment(index, data);
        } else {
            throw core::Exception(fmt::format("Fail to update host buffer fragment: {0}", index));
        }

        // Synchronize
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

    template<class T, size_t N>
    void StagingArrayBuffer<T, N>::free() {
        return StagingBuffer::free();
    }
}  // namespace ao::vulkan
