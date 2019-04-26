// Copyright 2018-2019 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#include <ao/core/utilities/memory.h>
#include <ao/core/utilities/types.h>
#include <ao/vulkan/memory/allocator/device_allocator.h>
#include <ao/vulkan/memory/allocator/host_allocator.h>
#include <gtest/gtest.h>
#include <ao/vulkan/memory/tuple.hpp>

#include "../helpers/tests.h"
#include "../helpers/vk_instance.hpp"

namespace ao::test {
    TEST(Tuple, Types) {
        // 'Mute' logger
        core::Logger::Init();
        core::Logger::SetMinLevel(core::Logger::Level::fatal);

        // Init instance
        VkInstance instance;
        SKIP_TEST(!instance.init(), VULKAN_INIT_FAILURE);

        auto allocator = std::make_shared<vulkan::HostAllocator>(instance.device);
        vulkan::Tuple<size_t, char*> tuple(allocator);

        ASSERT_EQ(typeid(size_t&), typeid(decltype(vulkan::get<0>(tuple))));
        ASSERT_EQ(typeid(char*&), typeid(decltype(vulkan::get<1>(tuple))));
    }

    TEST(HostTuple, BufferInfo) {
        // Init instance
        VkInstance instance;
        SKIP_TEST(!instance.init(), VULKAN_INIT_FAILURE);

        auto allocator = std::make_shared<vulkan::HostAllocator>(instance.device);
        vulkan::Tuple<size_t, char*> tuple(allocator);

        // Assert
        ASSERT_NE(nullptr, tuple.info().buffer);
        ASSERT_NE(nullptr, *tuple.info().ptr);
        ASSERT_EQ(core::utilities::calculateAligmentSize(vk::DeviceSize(sizeof(size_t) + sizeof(char*)), instance.minAligment()), tuple.info().size);
    }

    TEST(DeviceTuple, BufferInfo) {
        // Init instance
        VkInstance instance;
        SKIP_TEST(!instance.init(), VULKAN_INIT_FAILURE);

        auto allocator = std::make_shared<vulkan::DeviceAllocator>(instance.device, vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
        vulkan::Tuple<size_t, char*> tuple(allocator);

        // Assert
        ASSERT_NE(nullptr, tuple.info().buffer);
        ASSERT_TRUE(tuple.info().ptr);
        ASSERT_EQ(core::utilities::calculateAligmentSize(vk::DeviceSize(sizeof(size_t) + sizeof(char*)), instance.minAligment()), tuple.info().size);
    }

    TEST(HostTuple, Offset) {
        // Init instance
        VkInstance instance;
        SKIP_TEST(!instance.init(), VULKAN_INIT_FAILURE);

        auto allocator = std::make_shared<vulkan::HostAllocator>(instance.device);
        vulkan::Tuple<size_t, char*> tuple(allocator);

        // Assert
        ASSERT_EQ(vk::DeviceSize(0), tuple.offset(0));
        ASSERT_EQ(vk::DeviceSize(sizeof(size_t)), tuple.offset(1));
    }

    TEST(DeviceTuple, Offset) {
        // Init instance
        VkInstance instance;
        SKIP_TEST(!instance.init(), VULKAN_INIT_FAILURE);

        auto allocator = std::make_shared<vulkan::DeviceAllocator>(instance.device, vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
        vulkan::Tuple<size_t, char*> tuple(allocator);

        // Assert
        ASSERT_EQ(vk::DeviceSize(0), tuple.offset(0));
        ASSERT_EQ(vk::DeviceSize(sizeof(size_t)), tuple.offset(1));
    }

    TEST(HostTuple, UpdateAndGet) {
        // Init instance
        VkInstance instance;
        SKIP_TEST(!instance.init(), VULKAN_INIT_FAILURE);

        auto allocator = std::make_shared<vulkan::HostAllocator>(instance.device);
        vulkan::Tuple<size_t, size_t[10]> tuple(allocator);

        vulkan::get<0>(tuple) = 11;
        for (size_t i = 0; i < 10; i++) {
            vulkan::get<1>(tuple)[i] = i;
        }
        tuple.invalidateAll();

        // Assert
        ASSERT_EQ(11, vulkan::get<0>(tuple));
        for (size_t i = 0; i < 10; i++) {
            ASSERT_EQ(i, vulkan::get<1>(tuple)[i]);
        }
    }

    TEST(DeviceTuple, UpdateAndGet) {
        // Init instance
        VkInstance instance;
        SKIP_TEST(!instance.init(), VULKAN_INIT_FAILURE);

        auto allocator = std::make_shared<vulkan::DeviceAllocator>(instance.device, vk::CommandBufferUsageFlagBits::eRenderPassContinue);
        vulkan::Tuple<size_t, size_t[10]> tuple(allocator);

        vulkan::get<0>(tuple) = 11;
        for (size_t i = 0; i < 10; i++) {
            vulkan::get<1>(tuple)[i] = i;
        }
        tuple.invalidateAll();

        // Assert
        ASSERT_EQ(11, vulkan::get<0>(tuple));
        for (size_t i = 0; i < 10; i++) {
            ASSERT_EQ(i, vulkan::get<1>(tuple)[i]);
        }
    }
}  // namespace ao::test