// Copyright 2018-2019 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#include <ao/vulkan/wrapper/fence.h>
#include <gtest/gtest.h>

#include "../helpers/tests.h"
#include "../helpers/vk_instance.hpp"

namespace ao::test {
    TEST(Fence, DefaultConstructor) {
        // 'Mute' logger
        core::Logger::Init();
        core::Logger::SetMinLevel(core::Logger::Level::fatal);

        vulkan::Fence fence;

        ASSERT_FALSE(fence);
    }

    TEST(Fence, Constructor) {
        // Init instance
        VkInstance instance;
        SKIP_TEST(!instance.init(), FAIL_INIT_VULKAN);

        // Create fence
        vulkan::Fence fence(instance.device->logical);

        // Check status
        ASSERT_TRUE(fence);
        ASSERT_EQ(ao::vulkan::FenceStatus::eUnsignaled, fence.status());
        ASSERT_EQ(vk::Result::eNotReady, instance.device->logical->getFenceStatus(fence));
    }

    TEST(Fence, Copy) {
        // Init instance
        VkInstance instance;
        SKIP_TEST(!instance.init(), FAIL_INIT_VULKAN);

        // Create fence
        vulkan::Fence fence(instance.device->logical);

        // Copy
        ([](vulkan::Fence fence) {
            ASSERT_TRUE(fence);
            ASSERT_EQ(ao::vulkan::FenceStatus::eUnsignaled, fence.status());
        })(fence);

        // Check status
        ASSERT_TRUE(fence);
        ASSERT_EQ(ao::vulkan::FenceStatus::eUnsignaled, fence.status());
        ASSERT_EQ(vk::Result::eNotReady, instance.device->logical->getFenceStatus(fence));
    }

    TEST(Fence, Destroy) {
        // Init instance
        VkInstance instance;
        SKIP_TEST(!instance.init(), FAIL_INIT_VULKAN);

        // Create fence
        vulkan::Fence fence(instance.device->logical);

        // Destroy
        fence.destroy();

        ASSERT_FALSE(fence);
        ASSERT_EQ(ao::vulkan::FenceStatus::eDestroyed, fence.status());

        ASSERT_EXCEPTION<ao::core::Exception>([&]() { fence.destroy(); });
        ASSERT_EXCEPTION<ao::core::Exception>([&]() { fence.reset(); });
        ASSERT_EXCEPTION<ao::core::Exception>([&]() { fence.wait(); });
    }

    TEST(Fence, SharedStatus) {
        // Init instance
        VkInstance instance;
        SKIP_TEST(!instance.init(), FAIL_INIT_VULKAN);

        // Create fences
        vulkan::Fence fence(instance.device->logical);
        auto fence2 = fence;

        // Destroy once
        fence.destroy();

        ASSERT_FALSE(fence);
        ASSERT_FALSE(fence2);
        ASSERT_EQ(ao::vulkan::FenceStatus::eDestroyed, fence.status());
        ASSERT_EQ(ao::vulkan::FenceStatus::eDestroyed, fence2.status());
    }
}  // namespace ao::test
