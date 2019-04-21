// Copyright 2018-2019 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#include <ao/core/utilities/memory.h>
#include <ao/core/utilities/types.h>
#include <ao/vulkan/memory/allocator/device_allocator.h>
#include <ao/vulkan/memory/allocator/host_allocator.h>
#include <gtest/gtest.h>
#include <ao/vulkan/memory/vector.hpp>

#include "../helpers/tests.h"
#include "../helpers/vk_instance.hpp"

namespace ao::test {
    struct Object {
        u64 i;

        explicit Object(u64 _i) : i(_i) {}
    };

    TEST(HostVector, ValueConstructor) {
        // 'Mute' logger
        core::Logger::Init();
        core::Logger::SetMinLevel(core::Logger::Level::fatal);

        // Init instance
        VkInstance instance;
        SKIP_TEST(!instance.init(), FAIL_INIT_VULKAN);

        auto allocator = std::make_shared<vulkan::HostAllocator>(instance.device);
        vulkan::Vector<Object> vector(10, Object(15), allocator);

        // Assert content
        ASSERT_EQ(10, vector.size());
        for (auto& obj : vector) {
            ASSERT_EQ(15, obj.i);
        }
    }

    TEST(DeviceVector, ValueConstructor) {
        // Init instance
        VkInstance instance;
        SKIP_TEST(!instance.init(), FAIL_INIT_VULKAN);

        auto allocator = std::make_shared<vulkan::DeviceAllocator>(instance.device, vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
        vulkan::Vector<Object> vector(10, Object(15), allocator);

        // Assert content
        ASSERT_EQ(10, vector.size());
        for (auto& obj : vector) {
            ASSERT_EQ(15, obj.i);
        }
    }

    TEST(HostVector, AccessOperator) {
        // Init instance
        VkInstance instance;
        SKIP_TEST(!instance.init(), FAIL_INIT_VULKAN);

        auto allocator = std::make_shared<vulkan::HostAllocator>(instance.device);
        vulkan::Vector<Object> vector(10, Object(15), allocator);

        // Assert content
        for (size_t i = 0; i < vector.size(); i++) {
            ASSERT_EQ(15, vector[i].i);
        }
    }

    TEST(DeviceVector, AccessOperator) {
        // Init instance
        VkInstance instance;
        SKIP_TEST(!instance.init(), FAIL_INIT_VULKAN);

        auto allocator = std::make_shared<vulkan::DeviceAllocator>(instance.device, vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
        vulkan::Vector<Object> vector(10, Object(15), allocator);

        // Assert content
        for (size_t i = 0; i < vector.size(); i++) {
            ASSERT_EQ(15, vector[i].i);
        }
    }

    TEST(HostVector, Update) {
        // Init instance
        VkInstance instance;
        SKIP_TEST(!instance.init(), FAIL_INIT_VULKAN);

        auto allocator = std::make_shared<vulkan::HostAllocator>(instance.device);
        vulkan::Vector<Object> vector(10, Object(15), allocator);

        vector[3].i = 10;
        vector.invalidate(3);

        // Assert content
        ASSERT_EQ(10, vector[3].i);
    }

    TEST(DeviceVector, Update) {
        // Init instance
        VkInstance instance;
        SKIP_TEST(!instance.init(), FAIL_INIT_VULKAN);

        auto allocator = std::make_shared<vulkan::DeviceAllocator>(instance.device, vk::CommandBufferUsageFlagBits::eRenderPassContinue);
        vulkan::Vector<Object> vector(10, Object(15), allocator);

        vector[3].i = 10;
        vector.invalidate(3);

        // Assert content
        ASSERT_EQ(10, vector[3].i);
    }

    TEST(HostVector, BufferInfo) {
        // Init instance
        VkInstance instance;
        SKIP_TEST(!instance.init(), FAIL_INIT_VULKAN);

        auto allocator = std::make_shared<vulkan::HostAllocator>(instance.device);
        vulkan::Vector<Object> vector(10, Object(15), allocator);

        // Assert
        ASSERT_NE(nullptr, vector.info().buffer);
        ASSERT_NE(nullptr, *vector.info().ptr);
        ASSERT_EQ(core::utilities::calculateAligmentSize(10 * sizeof(Object), instance.minAligment()), vector.info().size);
    }

    TEST(DeviceVector, BufferInfo) {
        // Init instance
        VkInstance instance;
        SKIP_TEST(!instance.init(), FAIL_INIT_VULKAN);

        auto allocator = std::make_shared<vulkan::DeviceAllocator>(instance.device, vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
        vulkan::Vector<Object> vector(10, Object(15), allocator);

        // Assert
        ASSERT_NE(nullptr, vector.info().buffer);
        ASSERT_TRUE(vector.info().ptr);
        ASSERT_EQ(core::utilities::calculateAligmentSize(10 * sizeof(Object), instance.minAligment()), vector.info().size);
    }

    TEST(HostVector, Offset) {
        // Init instance
        VkInstance instance;
        SKIP_TEST(!instance.init(), FAIL_INIT_VULKAN);

        auto allocator = std::make_shared<vulkan::HostAllocator>(instance.device);
        vulkan::Vector<Object> vector(10, Object(15), allocator);

        // Assert
        ASSERT_EQ(sizeof(Object) * 3, vector.offset(3));
    }

    TEST(DeviceVector, Offset) {
        // Init instance
        VkInstance instance;
        SKIP_TEST(!instance.init(), FAIL_INIT_VULKAN);

        auto allocator = std::make_shared<vulkan::DeviceAllocator>(instance.device, vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
        vulkan::Vector<Object> vector(10, Object(15), allocator);

        // Assert
        ASSERT_EQ(sizeof(Object) * 3, vector.offset(3));
    }
}  // namespace ao::test
