// Copyright 2018 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#include <ao/vulkan/utilities/vulkan.h>
#include <gtest/gtest.h>

#include "../helpers/tests.h"

namespace ao {
	namespace test {
		TEST(VulkanUtils, ToResult) {
			ASSERT_EQ(vk::Result::eSuccess, vulkan::utilities::to_result(VkResult::VK_SUCCESS));

			ASSERT_EXCEPTION<core::Exception>([]() {
				vulkan::utilities::to_result(VkResult(-20));
			});
		}

		TEST(VulkanUtils, VkCheck) {
			ASSERT_TRUE(vulkan::utilities::vkCheck(VkResult::VK_SUCCESS));
			ASSERT_FALSE(vulkan::utilities::vkCheck(VkResult::VK_ERROR_DEVICE_LOST));
		}

		TEST(VulkanUtils, VkAssert) {
			vulkan::utilities::vkAssert(vk::Result::eSuccess, "Error");
			ASSERT_EXCEPTION<vk::DeviceLostError>([]() {
				vulkan::utilities::vkAssert(vk::Result::eErrorDeviceLost, "Error");
			}, [](vk::DeviceLostError e) {
				ASSERT_STREQ("Error: ErrorDeviceLost", e.what());
			});

			vulkan::utilities::vkAssert(VkResult::VK_SUCCESS, "Error");
			ASSERT_EXCEPTION<vk::DeviceLostError>([]() {
				vulkan::utilities::vkAssert(VkResult::VK_ERROR_DEVICE_LOST, "Error");
			}, [](vk::DeviceLostError e) {
				ASSERT_STREQ("Error: ErrorDeviceLost", e.what());
			});
		}

		TEST(VulkanUtils, FindQueueFamilyIndex) {
			// TODO: When logger is enhanced
		}
	}
}
