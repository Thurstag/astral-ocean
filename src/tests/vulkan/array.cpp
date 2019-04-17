// Copyright 2018-2019 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#include <ao/core/utilities/memory.h>
#include <ao/core/utilities/types.h>
#include <ao/vulkan/memory/allocator/device_allocator.h>
#include <ao/vulkan/memory/allocator/host_allocator.h>
#include <gtest/gtest.h>
#include <ao/vulkan/memory/array.hpp>

#include "../helpers/tests.h"
#include "../helpers/vk_instance.hpp"

namespace ao::test {
    struct Object {
        u64 i;

        explicit Object(u64 _i) : i(_i) {}
    };

    TEST(HostArray, ValueConstructor) {
        // 'Mute' logger
        core::Logger::Init();
        core::Logger::SetMinLevel(core::Logger::Level::fatal);

        // Init instance
        VkInstance instance;
        SKIP_TEST(!instance.init(), FAIL_INIT_VULKAN);

        auto allocator = std::make_shared<vulkan::HostAllocator>(instance.device);
        vulkan::Array<10, Object> array(Object(15), allocator);

        // Assert content
        ASSERT_EQ(10, array.size());
        for (auto& obj : array) {
            ASSERT_EQ(15, obj.i);
        }
    }

    TEST(DeviceArray, ValueConstructor) {
        // Init instance
        VkInstance instance;
        SKIP_TEST(!instance.init(), FAIL_INIT_VULKAN);

        auto allocator = std::make_shared<vulkan::DeviceAllocator>(instance.device, vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
        vulkan::Array<10, Object> array(Object(15), allocator);

        // Assert content
        ASSERT_EQ(10, array.size());
        for (auto& obj : array) {
            ASSERT_EQ(15, obj.i);
        }
    }

    TEST(HostArray, AccessOperator) {
        // Init instance
        VkInstance instance;
        SKIP_TEST(!instance.init(), FAIL_INIT_VULKAN);

        auto allocator = std::make_shared<vulkan::HostAllocator>(instance.device);
        vulkan::Array<10, Object> array(Object(15), allocator);

        // Assert content
        for (size_t i = 0; i < array.size(); i++) {
            ASSERT_EQ(15, array[i].i);
        }
    }

    TEST(DeviceArray, AccessOperator) {
        // Init instance
        VkInstance instance;
        SKIP_TEST(!instance.init(), FAIL_INIT_VULKAN);

        auto allocator = std::make_shared<vulkan::DeviceAllocator>(instance.device, vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
        vulkan::Array<10, Object> array(Object(15), allocator);

        // Assert content
        for (size_t i = 0; i < array.size(); i++) {
            ASSERT_EQ(15, array[i].i);
        }
    }

    TEST(HostArray, Update) {
        // Init instance
        VkInstance instance;
        SKIP_TEST(!instance.init(), FAIL_INIT_VULKAN);

        auto allocator = std::make_shared<vulkan::HostAllocator>(instance.device);
        vulkan::Array<10, Object> array(Object(15), allocator);

        array.update(3, [](Object& object) { object.i = 10; });

        // Assert content
        ASSERT_EQ(10, array[3].i);
    }

    TEST(DeviceArray, Update) {
        // Init instance
        VkInstance instance;
        SKIP_TEST(!instance.init(), FAIL_INIT_VULKAN);

        auto allocator = std::make_shared<vulkan::DeviceAllocator>(instance.device, vk::CommandBufferUsageFlagBits::eRenderPassContinue);
        vulkan::Array<10, Object> array(Object(15), allocator);

        array.update(3, [](Object& object) { object.i = 10; });

        // Assert content
        ASSERT_EQ(10, array[3].i);
    }

    TEST(HostArray, BufferInfo) {
        // Init instance
        VkInstance instance;
        SKIP_TEST(!instance.init(), FAIL_INIT_VULKAN);

        auto allocator = std::make_shared<vulkan::HostAllocator>(instance.device);
        vulkan::Array<10, Object> array(Object(15), allocator, vk::BufferUsageFlagBits::eStorageBuffer);

        // Assert
        ASSERT_NE(nullptr, array.info().buffer);
        ASSERT_NE(nullptr, *array.info().ptr);
        ASSERT_EQ(core::utilities::calculateAligmentSize(10 * sizeof(Object), instance.minAligment()), array.info().size);
    }

    TEST(DeviceArray, BufferInfo) {
        // Init instance
        VkInstance instance;
        SKIP_TEST(!instance.init(), FAIL_INIT_VULKAN);

        auto allocator = std::make_shared<vulkan::DeviceAllocator>(instance.device, vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
        vulkan::Array<10, Object> array(Object(15), allocator);

        // Assert
        ASSERT_NE(nullptr, array.info().buffer);
        ASSERT_TRUE(array.info().ptr);
        ASSERT_EQ(core::utilities::calculateAligmentSize(10 * sizeof(Object), instance.minAligment()), array.info().size);
    }

    TEST(HostArray, Offset) {
        // Init instance
        VkInstance instance;
        SKIP_TEST(!instance.init(), FAIL_INIT_VULKAN);

        auto allocator = std::make_shared<vulkan::HostAllocator>(instance.device);
        vulkan::Array<10, Object> array(Object(15), allocator);

        // Assert
        ASSERT_EQ(sizeof(Object) * 3, array.offset(3));
    }

    TEST(DeviceArray, Offset) {
        // Init instance
        VkInstance instance;
        SKIP_TEST(!instance.init(), FAIL_INIT_VULKAN);

        auto allocator = std::make_shared<vulkan::DeviceAllocator>(instance.device, vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
        vulkan::Array<10, Object> array(Object(15), allocator);

        // Assert
        ASSERT_EQ(sizeof(Object) * 3, array.offset(3));
    }
}  // namespace ao::test
