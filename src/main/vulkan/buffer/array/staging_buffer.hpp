// Copyright 2018-2019 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#include "../../exception/buffer_unitialized.h"
#include "../staging_buffer.hpp"
#include "basic_buffer.hpp"

namespace ao::vulkan {
    /**
     * @brief Staging array of buffers
     *
     * @tparam T Buffer type
     */
    template<class T>
    class StagingDynamicArrayBuffer : public DynamicArrayBuffer<T>, public StagingBuffer<BasicDynamicArrayBuffer<T>> {
       public:
        /**
         * @brief Construct a new StagingDynamicArrayBuffer object
         *
         * @param capacity Capacity
         * @param device Device
         * @param usage_flags usage flags
         */
        StagingDynamicArrayBuffer(size_t capacity, std::shared_ptr<Device> device,
                                  vk::CommandBufferUsageFlags usage_flags = vk::CommandBufferUsageFlagBits::eSimultaneousUse);

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
    StagingDynamicArrayBuffer<T>::StagingDynamicArrayBuffer(size_t capacity, std::shared_ptr<Device> device, vk::CommandBufferUsageFlags usage_flags)
        : DynamicArrayBuffer<T>(capacity, device), StagingBuffer<BasicDynamicArrayBuffer<T>>(device, usage_flags), Buffer(device) {}

    template<class T>
    StagingDynamicArrayBuffer<T>* StagingDynamicArrayBuffer<T>::init(vk::DeviceSize size, std::optional<vk::BufferUsageFlags> usage_flags) {
        if (this->hasBuffer()) {
            StagingBuffer<BasicDynamicArrayBuffer<T>>::free();
        }

        // Init buffer in host's memory
        this->host_buffer = std::make_unique<BasicDynamicArrayBuffer<T>>(this->capacity_, Buffer::device);
        this->host_buffer->init(vk::BufferUsageFlagBits::eTransferSrc, vk::SharingMode::eExclusive,
                                vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, size);

        // Init buffer in device's memory
        this->device_buffer = std::make_unique<BasicDynamicArrayBuffer<T>>(this->capacity_, Buffer::device);
        this->device_buffer->init(usage_flags ? vk::BufferUsageFlagBits::eTransferDst | *usage_flags : vk::BufferUsageFlagBits::eTransferDst,
                                  vk::SharingMode::eExclusive, vk::MemoryPropertyFlagBits::eDeviceLocal, size);

        // Create command buffer
        if (!this->command_buffer) {
            this->command_buffer = Buffer::device->transferPool().allocateCommandBuffers(vk::CommandBufferLevel::ePrimary, 1).front();
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
        return StagingBuffer<BasicDynamicArrayBuffer<T>>::hasBuffer();
    }

    template<class T>
    vk::Buffer StagingDynamicArrayBuffer<T>::buffer() {
        return StagingBuffer<BasicDynamicArrayBuffer<T>>::buffer();
    }

    template<class T>
    vk::DeviceSize StagingDynamicArrayBuffer<T>::size() const {
        return StagingBuffer<BasicDynamicArrayBuffer<T>>::size();
    }

    template<class T>
    vk::DeviceSize StagingDynamicArrayBuffer<T>::offset(size_t index) const {
        return StagingBuffer<BasicDynamicArrayBuffer<T>>::offset(index);
    }

    template<class T>
    Buffer* StagingDynamicArrayBuffer<T>::map() {
        return StagingBuffer<BasicDynamicArrayBuffer<T>>::map();
    }

    template<class T>
    void StagingDynamicArrayBuffer<T>::free() {
        return StagingBuffer<BasicDynamicArrayBuffer<T>>::free();
    }

    /**
     * @brief Staging array of buffers
     *
     * @tparam T Buffer type
     * @tparam N Array's size
     */
    template<class T, size_t N>
    class StagingArrayBuffer : public ArrayBuffer<T, N>, public StagingBuffer<BasicArrayBuffer<T, N>> {
       public:
        /**
         * @brief Construct a new StagingArrayBuffer object
         *
         * @param device Device
         * @param usage_flags Usage flags
         */
        StagingArrayBuffer(std::shared_ptr<Device> device,
                           vk::CommandBufferUsageFlags usage_flags = vk::CommandBufferUsageFlagBits::eSimultaneousUse);

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
    StagingArrayBuffer<T, N>::StagingArrayBuffer(std::shared_ptr<Device> device, vk::CommandBufferUsageFlags usage_flags)
        : ArrayBuffer<T, N>(device), StagingBuffer<BasicArrayBuffer<T, N>>(device, usage_flags), Buffer(device) {}

    template<class T, size_t N>
    StagingArrayBuffer<T, N>* StagingArrayBuffer<T, N>::init(vk::DeviceSize size, std::optional<vk::BufferUsageFlags> usage_flags) {
        if (this->hasBuffer()) {
            StagingBuffer<BasicArrayBuffer<T, N>>::free();
        }

        // Init buffer in host's memory
        this->host_buffer = std::make_unique<BasicArrayBuffer<T, N>>(Buffer::device);
        this->host_buffer->init(vk::BufferUsageFlagBits::eTransferSrc, vk::SharingMode::eExclusive,
                                vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, size);

        // Init buffer in device's memory
        this->device_buffer = std::make_unique<BasicArrayBuffer<T, N>>(Buffer::device);
        this->device_buffer->init(usage_flags ? vk::BufferUsageFlagBits::eTransferDst | *usage_flags : vk::BufferUsageFlagBits::eTransferDst,
                                  vk::SharingMode::eExclusive, vk::MemoryPropertyFlagBits::eDeviceLocal, size);

        // Create command buffer
        if (!this->command_buffer) {
            this->command_buffer = Buffer::device->transferPool().allocateCommandBuffers(vk::CommandBufferLevel::ePrimary, 1).front();
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
        return StagingBuffer<BasicArrayBuffer<T, N>>::hasBuffer();
    }

    template<class T, size_t N>
    vk::Buffer StagingArrayBuffer<T, N>::buffer() {
        return StagingBuffer<BasicArrayBuffer<T, N>>::buffer();
    }

    template<class T, size_t N>
    vk::DeviceSize StagingArrayBuffer<T, N>::size() const {
        return StagingBuffer<BasicArrayBuffer<T, N>>::size();
    }

    template<class T, size_t N>
    vk::DeviceSize StagingArrayBuffer<T, N>::offset(size_t index) const {
        return StagingBuffer<BasicArrayBuffer<T, N>>::offset(index);
    }

    template<class T, size_t N>
    Buffer* StagingArrayBuffer<T, N>::map() {
        return StagingBuffer<BasicArrayBuffer<T, N>>::map();
    }

    template<class T, size_t N>
    void StagingArrayBuffer<T, N>::free() {
        return StagingBuffer<BasicArrayBuffer<T, N>>::free();
    }
}  // namespace ao::vulkan
