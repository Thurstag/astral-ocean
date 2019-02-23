// Copyright 2018-2019 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#include <ao/vulkan/buffer/array/basic_buffer.hpp>
#include <ao/vulkan/buffer/array/staging_buffer.hpp>

namespace ao::test {
    template<class T, size_t N>
    class TestBasicArrayBuffer : public vulkan::BasicArrayBuffer<T, N> {
       public:
        /**
         * @brief Construct a new TestBasicArrayBuffer object
         *
         * @param device Device
         */
        explicit TestBasicArrayBuffer(std::shared_ptr<vulkan::Device> device);

        /**
         * @brief Expose mapper member
         *
         * @return void* Mapper
         */
        void* getMapper();
    };

    template<class T, size_t N>
    TestBasicArrayBuffer<T, N>::TestBasicArrayBuffer(std::shared_ptr<vulkan::Device> device)
        : vulkan::BasicArrayBuffer<T, N>(device), vulkan::Buffer(device) {}

    template<class T, size_t N>
    void* TestBasicArrayBuffer<T, N>::getMapper() {
        return this->mapper;
    }

    template<class T>
    class TestBasicDynamicArrayBuffer : public vulkan::BasicDynamicArrayBuffer<T> {
       public:
        /**
         * @brief Construct a new TestBasicDynamicArrayBuffer object
         *
         * @param count Count
         * @param device Device
         */
        TestBasicDynamicArrayBuffer(size_t count, std::shared_ptr<vulkan::Device> device);

        /**
         * @brief Expose mapper member
         *
         * @return void* Mapper
         */
        void* getMapper();
    };

    template<class T>
    TestBasicDynamicArrayBuffer<T>::TestBasicDynamicArrayBuffer(size_t count, std::shared_ptr<vulkan::Device> device)
        : vulkan::BasicDynamicArrayBuffer<T>(count, device), vulkan::Buffer(device) {}

    template<class T>
    void* TestBasicDynamicArrayBuffer<T>::getMapper() {
        return this->mapper;
    }

    template<class T>
    class TestStagingDynamicArrayBuffer : public vulkan::StagingDynamicArrayBuffer<T> {
       public:
        /**
         * @brief Construct a new TestStagingDynamicArrayBuffer object
         *
         * @param count
         * @param device
         */
        TestStagingDynamicArrayBuffer(size_t count, std::shared_ptr<vulkan::Device> device);

        /**
         * @brief Expose mapper member
         *
         * @return void* Mapper
         */
        void* getMapper();
    };

    template<class T>
    TestStagingDynamicArrayBuffer<T>::TestStagingDynamicArrayBuffer(size_t count, std::shared_ptr<vulkan::Device> device)
        : vulkan::StagingDynamicArrayBuffer<T>(count, device, vk::CommandBufferUsageFlagBits::eOneTimeSubmit), vulkan::Buffer(device) {}

    template<class T>
    void* TestStagingDynamicArrayBuffer<T>::getMapper() {
        if (auto host = static_cast<TestBasicDynamicArrayBuffer<T>*>(this->host_buffer.get())) {
            return host->getMapper();
        }
        throw core::Exception("Fail to get mapper");
    }

    template<class T, size_t N>
    class TestStagingArrayBuffer : public vulkan::StagingArrayBuffer<T, N> {
       public:
        /**
         * @brief Construct a new TestStagingArrayBuffer object
         *
         * @param device Device
         */
        explicit TestStagingArrayBuffer(std::shared_ptr<vulkan::Device> device);

        /**
         * @brief Expose mapper member
         *
         * @return void* Mapper
         */
        void* getMapper();
    };

    template<class T, size_t N>
    TestStagingArrayBuffer<T, N>::TestStagingArrayBuffer(std::shared_ptr<vulkan::Device> device)
        : vulkan::StagingArrayBuffer<T, N>(device, vk::CommandBufferUsageFlagBits::eOneTimeSubmit), vulkan::Buffer(device) {}

    template<class T, size_t N>
    void* TestStagingArrayBuffer<T, N>::getMapper() {
        if (auto host = static_cast<TestBasicArrayBuffer<T, N>*>(this->host_buffer.get())) {
            return host->getMapper();
        }
        throw core::Exception("Fail to get mapper");
    }
}  // namespace ao::test
