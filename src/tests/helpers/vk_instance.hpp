// Copyright 2018 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#include <ao/vulkan/engine/settings.h>
#include <ao/vulkan/engine/wrapper/device.h>
#include <ao/vulkan/utilities/vulkan.h>
#include <vulkan/vulkan.hpp>

namespace ao::test {
    struct VkInstance {
       public:
        std::shared_ptr<vk::Instance> instance;
        std::shared_ptr<vulkan::Device> device;

        ~VkInstance();

        /**
         * @brief Init vulkan
         *
         * @return true Vulkan initialized
         * @return false Fail to initialize vulkan
         */
        bool init();
    };

    inline VkInstance::~VkInstance() {
        this->device.reset();
        this->instance.reset();
    }

    bool VkInstance::init() {
        std::shared_ptr<vulkan::EngineSettings> settings = std::make_shared<vulkan::EngineSettings>();
        settings->get<bool>(vulkan::settings::ValidationLayers) = true;

        try {
            // Init volk
            ao::vulkan::utilities::vkAssert(volkInitialize(), "Fail to initialize vulkan loader");

            // Create instance
            this->instance = std::make_shared<vk::Instance>(vulkan::utilities::createVkInstance(settings, {}));
            volkLoadInstance(*this->instance);

            // Get GPUs
            std::vector<vk::PhysicalDevice> devices = this->instance->enumeratePhysicalDevices();

            // Check count
            if (devices.empty()) {
                return false;
            }

            // Select a vk::PhysicalDevice & wrap it
            this->device = std::make_shared<vulkan::Device>(devices.front());

            // Init logical device
            this->device->initLogicalDevice({}, {},
                                            {vulkan::QueueRequest(vk::QueueFlagBits::eGraphics), vulkan::QueueRequest(vk::QueueFlagBits::eTransfer),
                                             vulkan::QueueRequest(vk::QueueFlagBits::eCompute)});
        } catch (...) {
            return false;
        }
        return true;
    }
}  // namespace ao::test
