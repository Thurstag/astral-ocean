// Copyright 2018-2019 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#include <ao/vulkan/wrapper/device.h>
#include <ao/vulkan/buffer/tuple/basic_buffer.hpp>
#include <ao/vulkan/buffer/tuple/staging_buffer.hpp>

namespace ao::test {
    template<class... T>
    class TestBasicTupleBuffer : public vulkan::BasicTupleBuffer<T...> {
       public:
        /**
         * @brief Construct a new TestBasicTupleBuffer object
         *
         * @param device
         */
        explicit TestBasicTupleBuffer(std::shared_ptr<vulkan::Device> device);

        /**
         * @brief Expose mapper member
         *
         * @param index Index
         * @return void* Mapper
         */
        void* mapper(size_t index);
    };

    template<class... T>
    TestBasicTupleBuffer<T...>::TestBasicTupleBuffer(std::shared_ptr<vulkan::Device> device)
        : vulkan::BasicTupleBuffer<T...>(device), vulkan::Buffer(device) {}

    template<class... T>
    void* TestBasicTupleBuffer<T...>::mapper(size_t index) {
        return this->fragments[index].second;
    }

    template<class... T>
    class TestStagingTupleBuffer : public vulkan::StagingTupleBuffer<T...> {
       public:
        /**
         * @brief Construct a new TestStagingTupleBuffer object
         *
         * @param device
         */
        explicit TestStagingTupleBuffer(std::shared_ptr<vulkan::Device> device);

        /**
         * @brief Expose mapper member
         *
         * @param index Index
         * @return void* Mapper
         */
        void* mapper(size_t index);
    };

    template<class... T>
    TestStagingTupleBuffer<T...>::TestStagingTupleBuffer(std::shared_ptr<vulkan::Device> device)
        : vulkan::StagingTupleBuffer<T...>(device, vk::CommandBufferUsageFlagBits::eOneTimeSubmit), vulkan::Buffer(device) {}

    template<class... T>
    void* TestStagingTupleBuffer<T...>::mapper(size_t index) {
        if (auto host = static_cast<TestBasicTupleBuffer<T...>*>(this->host_buffer.get())) {
            return host->mapper(index);
        }
        throw core::Exception("Fail to get mapper");
    }
}  // namespace ao::test
