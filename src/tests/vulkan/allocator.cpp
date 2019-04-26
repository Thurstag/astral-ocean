// Copyright 2018-2019 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#include <ao/core/utilities/memory.h>
#include <ao/vulkan/memory/allocator/device_allocator.h>
#include <ao/vulkan/memory/allocator/host_allocator.h>
#include <gtest/gtest.h>

#include "../helpers/tests.h"
#include "../helpers/vk_instance.hpp"

namespace ao::test {
    TEST(HostAllocator, Allocate) {
        // 'Mute' logger
        core::Logger::Init();
        core::Logger::SetMinLevel(core::Logger::Level::fatal);

        // Init instance
        VkInstance instance;
        SKIP_TEST(!instance.init(), VULKAN_INIT_FAILURE);

        auto allocator = std::make_shared<vulkan::HostAllocator>(instance.device);

        // Assert
        auto info = allocator->allocate(sizeof(size_t), vk::BufferUsageFlagBits());
        ASSERT_NE(nullptr, info.buffer);
        ASSERT_TRUE(info.ptr);
        ASSERT_EQ(ao::core::utilities::calculateAligmentSize(sizeof(size_t), instance.minAligment()), info.size);
    }

    TEST(DeviceAllocator, Allocate) {
        // Init instance
        VkInstance instance;
        SKIP_TEST(!instance.init(), VULKAN_INIT_FAILURE);

        auto allocator = std::make_shared<vulkan::DeviceAllocator>(instance.device, vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

        // Assert
        auto info = allocator->allocate(sizeof(size_t), vk::BufferUsageFlagBits());
        ASSERT_NE(nullptr, info.buffer);
        ASSERT_TRUE(info.ptr);
        ASSERT_EQ(ao::core::utilities::calculateAligmentSize(sizeof(size_t), instance.minAligment()), info.size);
    }

    TEST(HostAllocator, Free) {
        // Init instance
        VkInstance instance;
        SKIP_TEST(!instance.init(), VULKAN_INIT_FAILURE);

        auto allocator = std::make_shared<vulkan::HostAllocator>(instance.device);
        auto info = allocator->allocate(sizeof(size_t), vk::BufferUsageFlagBits());

        allocator->free(info);

        // Assert
        ASSERT_FALSE(allocator->own(info));
    }

    TEST(DeviceAllocator, Free) {
        // Init instance
        VkInstance instance;
        SKIP_TEST(!instance.init(), VULKAN_INIT_FAILURE);

        auto allocator = std::make_shared<vulkan::DeviceAllocator>(instance.device, vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
        auto info = allocator->allocate(sizeof(size_t), vk::BufferUsageFlagBits());

        allocator->free(info);

        // Assert
        ASSERT_FALSE(allocator->own(info));
    }

    TEST(HostAllocator, Own) {
        // Init instance
        VkInstance instance;
        SKIP_TEST(!instance.init(), VULKAN_INIT_FAILURE);

        auto allocator = std::make_shared<vulkan::HostAllocator>(instance.device);
        auto info = allocator->allocate(sizeof(size_t), vk::BufferUsageFlagBits());

        // Assert
        ASSERT_TRUE(allocator->own(info));
        ASSERT_FALSE(allocator->own(vulkan::Allocator::BufferInfo()));
    }

    TEST(DeviceAllocator, Own) {
        // Init instance
        VkInstance instance;
        SKIP_TEST(!instance.init(), VULKAN_INIT_FAILURE);

        auto allocator = std::make_shared<vulkan::DeviceAllocator>(instance.device, vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
        auto info = allocator->allocate(sizeof(size_t), vk::BufferUsageFlagBits());

        // Assert
        ASSERT_TRUE(allocator->own(info));
        ASSERT_FALSE(allocator->own(vulkan::Allocator::BufferInfo()));
    }

    TEST(HostAllocator, AlignSize) {
        // Init instance
        VkInstance instance;
        SKIP_TEST(!instance.init(), VULKAN_INIT_FAILURE);
        auto uniform_alignment = instance.device->physical().getProperties().limits.minUniformBufferOffsetAlignment;

        auto uniform_allocator = std::make_shared<vulkan::HostAllocator>(instance.device, uniform_alignment);
        auto allocator = std::make_shared<vulkan::HostAllocator>(instance.device);

        // Assert
        ASSERT_EQ(ao::core::utilities::calculateAligmentSize(sizeof(size_t), uniform_alignment), uniform_allocator->alignSize(sizeof(size_t)));
        ASSERT_EQ(sizeof(size_t), allocator->alignSize(sizeof(size_t)));
    }

    TEST(DeviceAllocator, AlignSize) {
        // Init instance
        VkInstance instance;
        SKIP_TEST(!instance.init(), VULKAN_INIT_FAILURE);
        auto uniform_alignment = instance.device->physical().getProperties().limits.minUniformBufferOffsetAlignment;

        auto uniform_allocator =
            std::make_shared<vulkan::DeviceAllocator>(instance.device, vk::CommandBufferUsageFlagBits::eOneTimeSubmit, uniform_alignment);
        auto allocator = std::make_shared<vulkan::DeviceAllocator>(instance.device, vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

        // Assert
        ASSERT_EQ(ao::core::utilities::calculateAligmentSize(sizeof(size_t), uniform_alignment), uniform_allocator->alignSize(sizeof(size_t)));
        ASSERT_EQ(sizeof(size_t), allocator->alignSize(sizeof(size_t)));
    }

    TEST(HostAllocator, Size) {
        // Init instance
        VkInstance instance;
        SKIP_TEST(!instance.init(), VULKAN_INIT_FAILURE);

        auto allocator = std::make_shared<vulkan::HostAllocator>(instance.device);
        auto info = allocator->allocate(sizeof(size_t), vk::BufferUsageFlagBits());

        // Assert
        ASSERT_EQ(info.size, allocator->size());
    }

    TEST(DeviceAllocator, Size) {
        // Init instance
        VkInstance instance;
        SKIP_TEST(!instance.init(), VULKAN_INIT_FAILURE);

        auto allocator = std::make_shared<vulkan::DeviceAllocator>(instance.device, vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
        auto info = allocator->allocate(sizeof(size_t), vk::BufferUsageFlagBits());

        // Assert
        ASSERT_EQ(2 * info.size, allocator->size());
    }

    TEST(DeviceAllocator, FreeHost) {
        // Init instance
        VkInstance instance;
        SKIP_TEST(!instance.init(), VULKAN_INIT_FAILURE);

        auto allocator = std::make_shared<vulkan::DeviceAllocator>(instance.device, vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
        auto info = allocator->allocate(sizeof(size_t), vk::BufferUsageFlagBits());

        // Assert size
        ASSERT_EQ(2 * info.size, allocator->size());

        allocator->freeHost(info);

        // Assert
        ASSERT_EQ(info.size, allocator->size());
        ASSERT_TRUE(allocator->own(info));
    }

    TEST(DeviceAllocator, SizeOnDeviceAndHost) {
        // Init instance
        VkInstance instance;
        SKIP_TEST(!instance.init(), VULKAN_INIT_FAILURE);

        auto allocator = std::make_shared<vulkan::DeviceAllocator>(instance.device, vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
        auto info = allocator->allocate(sizeof(size_t), vk::BufferUsageFlagBits());

        // Assert size
        ASSERT_EQ(allocator->sizeOnHost() + allocator->sizeOnDevice(), allocator->size());

        allocator->freeHost(info);

        // Assert
        ASSERT_EQ(info.size, allocator->sizeOnDevice());
        ASSERT_EQ(0, allocator->sizeOnHost());
    }
}  // namespace ao::test