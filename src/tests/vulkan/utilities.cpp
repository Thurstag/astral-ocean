// Copyright 2018 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#include <iostream>

#include <ao/vulkan/utilities/vulkan.h>
#include <gtest/gtest.h>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/formatter_parser.hpp>

#include "../helpers/tests.h"

namespace ao::test {
    TEST(VulkanUtils, VkCheck) {
        ASSERT_TRUE(vulkan::utilities::vkCheck(VkResult::VK_SUCCESS));
        ASSERT_FALSE(vulkan::utilities::vkCheck(VkResult::VK_ERROR_DEVICE_LOST));
    }

    TEST(VulkanUtils, VkAssert) {
        vulkan::utilities::vkAssert(vk::Result::eSuccess, "Error");
        ASSERT_EXCEPTION<vk::DeviceLostError>([]() { vulkan::utilities::vkAssert(vk::Result::eErrorDeviceLost, "Error"); },
                                              [](vk::DeviceLostError e) { ASSERT_STREQ("Error: ErrorDeviceLost", e.what()); });

        vulkan::utilities::vkAssert(VkResult::VK_SUCCESS, "Error");
        ASSERT_EXCEPTION<vk::DeviceLostError>([]() { vulkan::utilities::vkAssert(VkResult::VK_ERROR_DEVICE_LOST, "Error"); },
                                              [](vk::DeviceLostError e) { ASSERT_STREQ("Error: ErrorDeviceLost", e.what()); });
    }

    TEST(VulkanUtils, FindQueueFamilyIndex) {
        // Create a new output for logger
        std::stringbuf ss;
        std::ostream os(&ss);
        boost::log::add_console_log(os, boost::log::keywords::format = "%Message%", boost::log::keywords::auto_flush = true);

        // Create properties
        vk::QueueFamilyProperties queueFamily;
        queueFamily.queueFlags = vk::QueueFlagBits::eCompute;
        std::vector<vk::QueueFamilyProperties> queueFamilyProperties;
        queueFamilyProperties.push_back(queueFamily);

        // Assert function
        ASSERT_EQ(0, vulkan::utilities::findQueueFamilyIndex(queueFamilyProperties, vk::QueueFlagBits::eCompute));
        ASSERT_EXCEPTION<ao::core::Exception>(
            [&queueFamilyProperties]() { vulkan::utilities::findQueueFamilyIndex(queueFamilyProperties, vk::QueueFlagBits::eGraphics); });

        // Assert log
        ASSERT_TRUE(ss.str().find("Found a queue that only supports: Compute") != std::string::npos);
    }
}  // namespace ao::test
